#ifndef CATFEEDER_MOTOR_H
#define CATFEEDER_MOTOR_H

#include <stdbool.h>

void setup_motor();
void start_motor();
void stop_motor();
bool is_motor_at_index_position();

#endif
