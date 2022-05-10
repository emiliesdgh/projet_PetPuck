#ifndef DANSE_MODE_H
#define DANSE_MODE_H

#include "main.h"
#include <motors.h>

#define POSITION_NOT_REACHED    0
#define POSITION_REACHED        1


#define PI                  3.1415926536f
//TO ADJUST IF NECESSARY. NOT ALL THE E-PUCK2 HAVE EXACTLY THE SAME WHEEL DISTANCE
#define WHEEL_DISTANCE      5.35f    //cm
#define PERIMETER_EPUCK     (PI * WHEEL_DISTANCE)


void motor_set_danse_speed(float speed_r, float speed_l);

void motor_set_danse(float speed_r, float speed_l,float position);
int8_t get_value_test_in_dance(void);

void dancing_puck_withStep(void);
void dancing_puck(void);

void motor_set_danse_position(float position_r, float position_l, float speed_r, float speed_l);

void motor_stop(void);

uint8_t motor_position_reached(void);

void motor_set_position(float position_r, float position_l, float speed_r, float speed_l);

void motor_set_speed(float speed_r, float speed_l); // fonction qui  fait bouger le moteur


void GoodMorning_motors(void);


#endif
