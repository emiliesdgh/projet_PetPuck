#ifndef DANSE_MODE_H
#define DANSE_MODE_H

#include "main.h"
#include <motors.h>

#define POSITION_NOT_REACHED    0
#define POSITION_REACHED        1


//void motor_set_speed(float speed_r, float speed_l); => elle existe dans motors.c


void motor_set_danse_speed(float speed_r, float speed_l);
void motor_set_danse_position(float position_r, float position_l, float speed_r, float speed_l);

void danseMode(float danse_speed); //--> pourrait etre void  pour  choisir ns  la vitesse

#endif
