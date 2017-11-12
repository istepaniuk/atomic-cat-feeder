#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "platform.h" 

void delay(unsigned long delay)
{
    while(delay) delay--;
}

int main(void)
{
    // Enable GPIO clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    gpio_set_pin_mode(&GPIO_PIN_B5, GPIO_Mode_Out_PP);
    gpio_set_pin_mode(&GPIO_PIN_C8, GPIO_Mode_Out_PP);

    while(1)
    {
        GPIOB->ODR ^= GPIO_Pin_5;
        GPIOC->ODR ^= GPIO_Pin_8;
        delay(1000000);
    }
}

