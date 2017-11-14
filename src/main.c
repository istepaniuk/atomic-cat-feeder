#include <stdbool.h>
#include <stm32f10x.h>
#include "platform.h"
#include "hardware.h"

void setup_ports();
void delay(unsigned long delay);
void stop_motor();
void start_motor();


static const int ERROR_BLINK_TIME = 500;
static const int MIN_RUN_TIME = 500;
static const int MAX_RUN_TIME = 3000;
static const int STALL_WAIT_TIME = 1000;
static const int RETRY_RUN_TIME = 300;
static const int MAX_STALLS = 10;
static const int TIME_UNIT_LOOPS = 2500;

bool is_motor_at_index_position();
int main(void)
{
    enum { IDLE, RUNNING, STALLED, ERROR } status = IDLE;

    int run_time = 0;
    int stall_time = 0;
    int error_time = 0;
    int stall_count = 0;

    setup_ports();

    while (1) {
        if (status == IDLE) {
            stall_count = 0;
            stop_motor();
            if (gpio_get_pin_state(&BUTTON_PIN)) {
                status = RUNNING;
            }
        }

        if (status == RUNNING) {
            start_motor();
            if (is_motor_at_index_position() && run_time > MIN_RUN_TIME) {
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
            run_time++;
        }

        if (status == STALLED) {
            stop_motor();
            if (stall_time > STALL_WAIT_TIME) {
                status = RUNNING;
                stall_time = 0;
                run_time = MAX_RUN_TIME - RETRY_RUN_TIME;
            }
            stall_time++;
        }

        if (status == ERROR) {
            stop_motor();
            if (error_time > ERROR_BLINK_TIME) {
                gpio_set_pin_high(&BLUE_LED_PIN);
            } else {
                gpio_set_pin_low(&BLUE_LED_PIN);
            }

            if (error_time > 2 * ERROR_BLINK_TIME) {
                error_time = 0;
            }
            error_time++;
        }

        delay(TIME_UNIT_LOOPS);
    }
}

bool is_motor_at_index_position()
{
    return !gpio_get_pin_state(&INDEX_PIN);
}

void start_motor()
{
    gpio_set_pin_high(&MOTOR_PIN);
    gpio_set_pin_high(&GREEN_LED_PIN);
}

void stop_motor()
{
    gpio_set_pin_low(&MOTOR_PIN);
    gpio_set_pin_low(&GREEN_LED_PIN);
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

    gpio_set_pin_low(&MOTOR_PIN);
    gpio_set_pin_low(&BLUE_LED_PIN);
    gpio_set_pin_low(&GREEN_LED_PIN);
}

void delay(unsigned long delay)
{
    while (delay) {
        delay--;
    }
}
