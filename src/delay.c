#include "delay.h"

static const int LOOPS_FOR_1_MS = 2500;

void delay_ms(unsigned long delay)
{
    delay *= LOOPS_FOR_1_MS;

    while (delay) {
        delay--;
    }
}
