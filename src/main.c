#include "platform.h"
#include "hardware.h"
#include "motor.h"
#include "leds.h"
#include "delay.h"
#include "button.h"

static const int ERROR_BLINK_TIME = 500;
static const int MIN_RUN_TIME = 500;
static const int MAX_RUN_TIME = 3000;
static const int STALL_WAIT_TIME = 1000;
static const int RETRY_RUN_TIME = 300;
static const int MAX_STALLS = 10;

int main(void)
{
    enum { IDLE, RUNNING, STALLED, ERROR } status = IDLE;

    int run_time = 0;
    int stall_time = 0;
    int stall_count = 0;

    setup_gpio();
    setup_motor();
    setup_button();
    setup_leds();

    while (1) {
        if (status == IDLE) {
            stall_count = 0;
            stop_motor();
            if (gpio_get_pin_state(&BUTTON_PIN)) {
                status = RUNNING;
            }
        }

        if (status == RUNNING) {
            turn_on_green_led();
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
        } else {
            turn_off_green_led();
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
            toggle_blue_led();
            delay_ms(ERROR_BLINK_TIME);
        }

        delay_ms(1);
    }
}
