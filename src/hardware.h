#ifndef CATFEEDER_HARDWARE_H
#define CATFEEDER_HARDWARE_H

#include <stm32f10x.h>
#include "platform.h"


#define BLUE_LED_PIN GPIO_PIN_C8
#define GREEN_LED_PIN GPIO_PIN_C9

#define USART_TX_PIN GPIO_PIN_A9
#define USART_RX_PIN GPIO_PIN_A10

#define BUTTON_PIN GPIO_PIN_A0

#define MOTOR_PIN GPIO_PIN_B5
#define INDEX_PIN GPIO_PIN_B1

#define DCF77_IN_PIN GPIO_PIN_D2
#define DCF77_POWER_PIN GPIO_PIN_C12

void setup_gpio();

#endif
