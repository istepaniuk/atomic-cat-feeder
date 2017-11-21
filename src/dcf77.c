#include <stm32f10x.h>
#include <stdbool.h>
#include "dcf77.h"
#include "hardware.h"
#include "usart.h"
#include "timer.h"
#include "interrupts.h"

static const int SAMPLES_PER_SECOND = 1000;
static bool minute_mark_detected;
static int acquired_bits;
static int ticks;

static void acquire_sample_for_demodulation()
{
    static int samples_high_in_1_region_count = 0;
    bool signal = !gpio_get_pin_state(&DCF77_IN_PIN);

    if(ticks > 10 && ticks < 90) {
        gpio_set_pin_high(&BLUE_LED_PIN);
    }else{
        gpio_set_pin_low(&BLUE_LED_PIN);
    }

    if(ticks > 110 && ticks < 190) {
        gpio_set_pin_high(&BLUE_LED_PIN);
        if(signal)
            samples_high_in_1_region_count++;
    }else{
        gpio_set_pin_low(&BLUE_LED_PIN);
    }

    if(signal){
        gpio_set_pin_high(&GREEN_LED_PIN);
    }
    else {
        gpio_set_pin_low(&GREEN_LED_PIN);
    }

    ticks++;

    if(ticks > SAMPLES_PER_SECOND){
        if(samples_high_in_1_region_count > 50){
            usart_putc('1');
        } else {
            usart_putc('0');
        }
        ticks = 0;
        acquired_bits++;
        samples_high_in_1_region_count = 0;
    }
}

static void acquire_sample_for_minute_mark()
{
    static unsigned int samples_high_count = 0;
    bool signal = !gpio_get_pin_state(&DCF77_IN_PIN);

    if(signal){
        samples_high_count++;
        gpio_set_pin_high(&BLUE_LED_PIN);
    }
    else {
        samples_high_count = 0;
        gpio_set_pin_low(&BLUE_LED_PIN);
    }

    if(samples_high_count > 1000){
        minute_mark_detected = true;
        timer2_stop();
    }
}

static void wait_for_minute_mark()
{
    usart_puts("Waiting for DCF minute mark...\n");
    gpio_set_pin_high(&GREEN_LED_PIN);

    minute_mark_detected = false;
    set_timer2_interrupt_callback(acquire_sample_for_minute_mark);
    timer2_start();

    while (!minute_mark_detected) {
    }

    gpio_set_pin_low(&GREEN_LED_PIN);
}

static void dcf77_start_acquire()
{
    wait_for_minute_mark();

    acquired_bits = 0;
    ticks = 0;
    set_timer2_interrupt_callback(acquire_sample_for_demodulation);
    timer2_start();

    usart_putc('>');

    while(acquired_bits < 58){
    }

    timer2_stop();
    usart_puts("<\n");
}


void setup_dcf77()
{
    gpio_set_pin_mode(&DCF77_IN_PIN, GPIO_MODE_IN_FLOATING);
    gpio_set_pin_mode(&DCF77_POWER_PIN, GPIO_MODE_OUT_PUSH_PULL);
    gpio_set_pin_high(&DCF77_POWER_PIN);

    //F = CLK/((PSC + 1)*(ARR + 1))
    //24MHz CLK/(1000-1)*(24-1) = 1 KHz ISR
    timer2_init(1000-1, 24-1);
    timer2_stop();

    while(1) {
        dcf77_start_acquire();
    }
}