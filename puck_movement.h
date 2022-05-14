#ifndef PUCK_MOVEMENT_H
#define PUCK_MOVEMENT_H

//#include "main.h" ??		--<>>>mettre dans main

#include <hal.h>
#include "sensors/mpu9250.h"

#include <sensors/imu.h>

#define THRESHOLD_ACC_X	2.0
#define THRESHOLD_ACC_Y	7.0
#define CST_RADIAN		(180.0/3.1415)	// used to convert radian in degrees


void initial_proximity(void);
int get_ambient_testing(void);
void motor_set_danse_speed(float speed_r, float speed_l);
void dancing_puck(void);
void ObstacleEncounter_start(void);
int16_t motors_speed(uint16_t distance_mm);
int get_led_flag_uhOh(void);
int8_t get_reset_direction(void);
void clear_reset_direction(void);
uint32_t get_colors(void);
void PanicMode_start(void);
int8_t get_inclination(imu_msg_t *imu_values);


#endif
