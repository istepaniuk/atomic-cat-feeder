#include <stm32f10x.h>
#include <stdbool.h>
#include "dcf77.h"
#include "hardware.h"
#include "usart.h"
#include "timer.h"
#include "interrupts.h"

static const int SAMPLE_SIZE = 64;

int get_average_signal_value(int samples)
{
    int high_sample_count = 0;
    for (int i = 0; i < SAMPLE_SIZE; i++) {
        high_sample_count += (samples >> i) & 1;
    }

    return high_sample_count > 32;
}

static int ticks = 0;
static int its1 = 0;

void timer2_interrupt_handler()
{
    ticks++;
    if(ticks >= 100 && ticks < 200){
        if(gpio_get_pin_state(&DCF77_IN_PIN)){
            its1++;
        }
    } else if(ticks >= 1000){
        if(its1 > 50){
            usart_putc('1');
        } else {
            usart_putc('0');
        }
        its1 = 0;
        ticks = 0;
    }
}

void setup_dcf77()
{
    gpio_set_pin_mode(&DCF77_IN_PIN, GPIO_MODE_IN_FLOATING);
    gpio_set_pin_mode(&DCF77_POWER_PIN, GPIO_MODE_OUT_PUSH_PULL);
    gpio_set_pin_high(&DCF77_POWER_PIN);

    int signal = 0;
    int samples = 0;
    int sample_index = 0;
    int previous_reading = 0;
    int signal_high_time = 0;

    //Update_event = TIM_CLK/((PSC + 1)*(ARR + 1))
    timer2_init(1000-1, 24-1);
    timer2_stop();
    set_timer2_interrupt_callback(timer2_interrupt_handler);

    usart_puts("Waiting for DCF minute mark...\n");

    while (1) {
        bool pin_state = !gpio_get_pin_state(&DCF77_IN_PIN);
        samples = (samples & (~(1 << sample_index))) | (pin_state << sample_index);
        if (++sample_index >= SAMPLE_SIZE) {
            signal = get_average_signal_value(samples);
            sample_index = 0;
        }

        //falling edge
        if (signal < previous_reading) {
            if (signal_high_time > 60000) {
                timer2_start();
                usart_puts("\nM ");
            }
            signal_high_time = 0;
        }

        if (signal) {
            signal_high_time++;
            gpio_set_pin_high(&BLUE_LED_PIN);
        } else {
            gpio_set_pin_low(&BLUE_LED_PIN);
        }

        if (gpio_get_pin_state(&DCF77_IN_PIN)) {
            gpio_set_pin_low(&GREEN_LED_PIN);
        } else {
            gpio_set_pin_high(&GREEN_LED_PIN);
        }

        previous_reading = signal;
    }
}
