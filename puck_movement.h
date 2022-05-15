#ifndef PUCK_MOVEMENT_H
#define PUCK_MOVEMENT_H

#define THRESHOLD_ACC_X	2.0
#define THRESHOLD_ACC_Y	7.0
#define CST_RADIAN		(180.0/3.1415)	//used to convert radian in degrees
//define for obstacle encounter
#define	DISTANCE_MIN	75.0f
// C standard header files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
// ChibiOS headers
#include "ch.h"
#include <hal.h>
#include "memory_protection.h"
#include "msgbus/messagebus.h"
//e-puck 2 main processor headers
#include "sensors/VL53L0X/VL53L0X.h"
#include <camera/po8030.h>
#include <camera/dcmi_camera.h>
#include "sensors/proximity.h"
#include <sensors/imu.h>
#include "chmtx.h"				//for mutex decl
#include "sensors/mpu9250.h"
#include <sensors/imu.h>
#include <spi_comm.h>
#include <leds.h>
#include <i2c_bus.h>
#include <motors.h>
#include <audio/audio_thread.h>
#include <audio/play_melody.h>
//include our files
#include <puck_led.h>
#include <control.h>
#include <audio_processing.h>
#include <process_image.h>
#include <selector.h>



void initial_proximity(void);
//int get_ambient_testing(void);
void motor_set_danse_speed(float speed_r, float speed_l);
void dancing_puck(void);
void ObstacleEncounter_start(void);
//void obstacleEncounter_stop(void);
//
//msg_t ObstacleEncounter_suspend(void);
//void ObstacleEncounter_resume(msg_t suspend_return);


void motors_speed(uint16_t distance_mm); // int16_t *speed);
int get_led_flag_uhOh(void);
int8_t get_reset_direction(void);
void clear_reset_direction(void);
uint32_t get_colors(void);
void PanicMode_start(void);
void PanicMode_stop(void);
int8_t get_inclination(imu_msg_t *imu_values);
void sum_pixels(uint8_t *buffer1,uint8_t *buffer2, uint8_t *buffer3, uint32_t* redsum, uint32_t* greensum, uint32_t* bluesum);


#endif
