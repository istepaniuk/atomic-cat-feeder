# atomic-cat-feeder
An automated cat food dispenser controlled with a caesium atomic fountain clock

The brain of this hack uses an STM32F100, a Cortex-M3 ARM microcontroller from ST

It uses a DCF77 module to tune into the german 77.5 kHz atomic clock signal to schedule the feeding interval.

