#include <stm32f10x.h>
#include <stdbool.h>
#include "dcf77.h"
#include "hardware.h"
#include "usart.h"
#include "delay.h"

const int SAMPLE_SIZE = 8;

int get_average_signal_value(int samples);
void setup_dcf77()
{
    gpio_set_pin_mode(&DCF77_IN_PIN, GPIO_MODE_IN_FLOATING);
    gpio_set_pin_mode(&DCF77_POWER_PIN, GPIO_MODE_OUT_PUSH_PULL);
    gpio_set_pin_high(&DCF77_POWER_PIN);

    usart_puts("DCF\n");

    int signal = 0;
    int samples = 0;
    int sample_index = 0;
    int previous_reading = 0;
    int signal_high_time = 0;
    int signal_low_time = 0;

    while (1) {
        bool pin_state = !gpio_get_pin_state(&DCF77_IN_PIN);
        samples =
            (samples & (~(1 << sample_index))) | (pin_state <<
                                                  sample_index);
        if (++sample_index >= SAMPLE_SIZE) {
            signal = get_average_signal_value(samples);
            sample_index = 0;
        }
        //raising edge
        if (signal > previous_reading) {
            signal_high_time = 0;

            if (signal_low_time > 180 && signal_low_time < 220) {
                usart_putc('1');
            } else if (signal_low_time > 80 && signal_low_time < 120) {
                usart_putc('0');
            } else {
                usart_putc('?');
                usart_putc(' ');
                printint(signal_low_time);
                usart_putc(' ');
            }

        }
        //falling edge
        if (signal < previous_reading) {
            signal_low_time = 0;
            if (signal_high_time > 1000) {
                usart_putc('\n');
            }
        }

        if (signal) {
            gpio_set_pin_high(&BLUE_LED_PIN);
            signal_high_time += 1;
        } else {
            gpio_set_pin_low(&BLUE_LED_PIN);
            signal_low_time += 1;
        }

        if (gpio_get_pin_state(&DCF77_IN_PIN)) {
            gpio_set_pin_low(&GREEN_LED_PIN);
        } else {
            gpio_set_pin_high(&GREEN_LED_PIN);
        }

        previous_reading = signal;
        delay_ms(1);
    }
}

int get_average_signal_value(int samples)
{
    int high_sample_count = 0;
    for (int i = 0; i < SAMPLE_SIZE; i++) {
        high_sample_count += (samples >> i) & 1;
    }

    return high_sample_count > 4;
}
