#include <stdbool.h>
#include <stm32f10x.h>
#include "hardware.h"

void setup_motor()
{
    gpio_set_pin_mode(&MOTOR_PIN, GPIO_MODE_OUT_PUSH_PULL);
    gpio_set_pin_mode(&INDEX_PIN, GPIO_MODE_IN_FLOATING);
}

void start_motor()
{
    gpio_set_pin_high(&MOTOR_PIN);
}

void stop_motor()
{
    gpio_set_pin_low(&MOTOR_PIN);
}

bool is_motor_at_index_position()
{
    return !gpio_get_pin_state(&INDEX_PIN);
}