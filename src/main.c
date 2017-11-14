#include "stm32f10x_conf.h"
#include "platform.h"
#include "hardware.h"

void setup_ports();
void delay(unsigned long delay);

int main(void)
{
    enum { IDLE, RUNNING, STALLED, ERROR } status = IDLE;
    const int MIN_RUN_TIME = 200;
    const int MAX_RUN_TIME = 3000;
    const int STALL_WAIT_TIME = 1000;
    const int RETRY_RUN_TIME = 300;
    const int MAX_STALLS = 10;
    const int TIME_UNIT_LOOPS = 2500;

    int run_time = 0;
    int stall_time = 0;
    int error_time = 0;
    int stall_count = 0;

    setup_ports();

    while (1) {
        if (status == IDLE) {
            stall_count = 0;
            gpio_set_pin_low(&MOTOR_PIN);
            gpio_set_pin_low(&GREEN_LED_PIN);
            gpio_set_pin_low(&BLUE_LED_PIN);
            if (gpio_get_pin_state(&BUTTON_PIN)) {
                status = RUNNING;
            }
        }

        if (status == RUNNING) {
            run_time++;
            gpio_set_pin_high(&MOTOR_PIN);
            gpio_set_pin_high(&GREEN_LED_PIN);
            gpio_set_pin_low(&BLUE_LED_PIN);
            if (!gpio_get_pin_state(&INDEX_PIN) && run_time > MIN_RUN_TIME) {
                status = IDLE;
                run_time = 0;
            }
            if (run_time > MAX_RUN_TIME) {
                status = STALLED;
                stall_count++;
                if (stall_count > MAX_STALLS) {
                    status = ERROR;
                }
            }
        }

        if (status == STALLED) {
            stall_time++;
            gpio_set_pin_low(&MOTOR_PIN);
            gpio_set_pin_low(&GREEN_LED_PIN);
            gpio_set_pin_high(&BLUE_LED_PIN);
            if (stall_time > STALL_WAIT_TIME) {
                status = RUNNING;
                stall_time = 0;
                run_time = MAX_RUN_TIME - RETRY_RUN_TIME;
            }
        }

        if (status == ERROR) {
            error_time++;
            gpio_set_pin_low(&MOTOR_PIN);
            gpio_set_pin_low(&GREEN_LED_PIN);
            if (error_time > 500) {
                gpio_set_pin_high(&BLUE_LED_PIN);
            } else {
                gpio_set_pin_low(&BLUE_LED_PIN);
            }

            if (error_time > 1000) {
                error_time = 0;
            }
        }

        delay(TIME_UNIT_LOOPS);
    }
}

void setup_ports()
{
    // Enable GPIO clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    gpio_set_pin_mode(&INDEX_PIN, GPIO_MODE_IN_FLOATING);
    gpio_set_pin_mode(&BUTTON_PIN, GPIO_MODE_IN_FLOATING);
    gpio_set_pin_mode(&MOTOR_PIN, GPIO_MODE_OUT_PUSH_PULL);
    gpio_set_pin_mode(&BLUE_LED_PIN, GPIO_MODE_OUT_PUSH_PULL);
    gpio_set_pin_mode(&GREEN_LED_PIN, GPIO_MODE_OUT_PUSH_PULL);
}

void delay(unsigned long delay)
{
    while (delay) {
        delay--;
    }
}
