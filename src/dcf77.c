#include <stm32f10x.h>
#include <stdbool.h>
#include "dcf77.h"
#include "hardware.h"
#include "usart.h"
#include "timer.h"
#include "interrupts.h"


static bool minute_mark_detected;
static int its1 = 0;

static void acquire_sample_for_demodulation()
{
    bool signal = gpio_get_pin_state(&DCF77_IN_PIN);
    int ticks = tim2_get_timer_value();
    int samples_1;

    if(ticks == 0){
        if(samples_1 > 50){
            usart_putc('1');
        } else {
            usart_putc('0');
        }

        samples_1 = 0;
    }

    if(signal){
        if(ticks > 110 && ticks < 190){
            samples_1++;
        }
    }
}

static void acquire_sample_for_minute_mark()
{
    static unsigned int signal_high_samples = 0;
    bool signal = gpio_get_pin_state(&DCF77_IN_PIN);

    if(signal){
        signal_high_samples++;
    }
    else {
        signal_high_counter = 0
    }

    if(signal_high_counter > 990){
        minute_mark_detected = true;
        timer2_stop();
    }
}

void setup_dcf77()
{
    gpio_set_pin_mode(&DCF77_IN_PIN, GPIO_MODE_IN_FLOATING);
    gpio_set_pin_mode(&DCF77_POWER_PIN, GPIO_MODE_OUT_PUSH_PULL);
    gpio_set_pin_high(&DCF77_POWER_PIN);

    //F = CLK/((PSC + 1)*(ARR + 1))
    timer2_init(1000-1, 24-1);
    timer2_stop();

    dcf77_acquire();
}

void dcf77_acquire()
{
    bool signal = 0;
    int previous_reading = 0;
    int signal_high_time = 0;


    wait_for_minute_mark();

    set_timer2_interrupt_callback(acquire_sample_for_demodulation);
    timer2_start();
}

static void wait_for_minute_mark()
{
    usart_puts("Waiting for DCF minute mark...\n");
    gpio_set_pin_high(&GREEN_LED_PIN);

    minute_mark_detected = false;
    set_timer2_interrupt_callback(acquire_sample_for_minute_mark);

    while (!minute_mark_detected) {
    }

    gpio_set_pin_low(&GREEN_LED_PIN);
}
