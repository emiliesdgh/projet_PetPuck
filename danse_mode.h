#ifndef DANSE_MODE_H
#define DANSE_MODE_H

#include "main.h"
#include <motors.h>

#define POSITION_NOT_REACHED    0
#define POSITION_REACHED        1


void motor_set_danse_speed(float speed_r, float speed_l);

void dancing_puck(void);

void GoodMorning_motors(void);


#endif
