#include "stm32f10x_conf.h"
#include "platform.h"
#include "hardware.h"

void delay(unsigned long delay)
{
    while(delay) delay--;
}

int main(void)
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

    while(1)
    {
        if(gpio_get_pin_state(&INDEX_PIN)){
            gpio_set_pin_high(&MOTOR_PIN);
            gpio_set_pin_high(&GREEN_LED_PIN);
        } else {
            if(gpio_get_pin_state(&BUTTON_PIN)) {
                gpio_set_pin_high(&MOTOR_PIN);
                delay(500000);
            } else {
                gpio_set_pin_low(&MOTOR_PIN);
            }
            gpio_set_pin_low(&GREEN_LED_PIN);
            gpio_set_pin_low(&MOTOR_PIN);
        }

        if(gpio_get_pin_state(&INDEX_PIN)){
            gpio_set_pin_low(&BLUE_LED_PIN);
        } else {
            gpio_set_pin_high(&BLUE_LED_PIN);
        }
    }
}

