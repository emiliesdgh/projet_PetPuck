// C standard header files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// ChibiOS headers
#include "ch.h"
#include "hal.h"
#include "memory_protection.h"
#include "msgbus/messagebus.h"
#include "chprintf.h"
#include <usbcfg.h>

// e-puck 2 main processor headers
#include "sensors/VL53L0X/VL53L0X.h"
#include "sensors/proximity.h"
#include <sensors/imu.h>

//include  the files from the given library
#include <spi_comm.h>
#include <i2c_bus.h>
#include <leds.h>
#include <motors.h>

#include <audio/audio_thread.h>
#include <audio/play_melody.h>

//#include "epuck1x/a_d/advance_ad_scan/e_acc.h"

//include our files
#include <main.h>
#include <puck_led.h>


#include <panic_mode.h>
#include <puck_movement.h>

//l'axe Z est l'axe en direction de devant
//l'axe Y est l'axe en direction de coté
//l'axe X est l'axe en direction de haut/bas


imu_msg_t imu_values;


static int led_flag_panic = 0; //led_flag
#define ACC_OFFSET 0.0239
#define NB_ACC_SAMPLES 5

static float puck_orientation = 0;
static float puck_inclination = 0;


//
//void get_orientation(imu_msg_t *imu_values){
//
//	//create a pointer to the array for shorter name
//	float *accel = imu_values->acceleration;
////	float *gyro = imu_values->gyro_rate;
//
//	float acc_x = accel[X_AXIS];
//	float acc_y = accel[Y_AXIS];
//	float acc_z = accel[Z_AXIS];
//
//	puck_inclination = 90.0 - atan2f((float)(acc_z), sqrtf( (float)((acc_x * acc_x) + (acc_y * acc_y) ))) * CST_RADIAN;
//
////	if(puck_inclination<5 ||  puck_inclination>160){
////		puck_orientation = 0;
////	}else{
////		puck_orientation = (atan2f((float)(acc_x), (float)(acc_y)) * CST_RADIAN) + 180.0;		// 180 is added to have 0 to 360 degrees range
////	}
//	if(puck_inclination>5 || puck_inclination < -5){
//
//		PanicMode_LED();
//
//	}
//}


int get_inclination(imu_msg_t *imu_values){

	//create a pointer to the array for shorter name
	float *accel = imu_values->acceleration;
//	float *gyro = imu_values->gyro_rate;

	int no_panic = 0;
	int panic = 1;

	float acc_x = accel[X_AXIS];
	float acc_y = accel[Y_AXIS];
	float acc_z = accel[Z_AXIS];

	puck_inclination = 90.0 - atan2f((float)(acc_z), sqrtf( (float)((acc_x * acc_x) + (acc_y * acc_y) ))) * CST_RADIAN;
	puck_orientation = 180 - puck_inclination;
//	if(puck_inclination<5 ||  puck_inclination>160){
//		puck_orientation = 0;
//	}else{
//		puck_orientation = (atan2f((float)(acc_x), (float)(acc_y)) * CST_RADIAN) + 180.0;		// 180 is added to have 0 to 360 degrees range
//	}
//	chprintf((BaseSequentialStream *)&SDU1, "inclinaison = %f\n", puck_inclination);

//	chprintf((BaseSequentialStream *)&SDU1, "inclinaison test 2 = %f\n", puck_orientation);


	if(puck_orientation > 4){

		return panic;

	}else{
		return no_panic;
	}
}


static THD_WORKING_AREA(waPanicMode, 1024);
static THD_FUNCTION(PanicMode, arg){

	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	systime_t time;
	messagebus_topic_t *imu_topic = messagebus_find_topic_blocking(&bus, "/imu");

	//threshold value to not use the leds when the robot is too horizontal
//	float threshold_accel = 2.0;
//	get_initial_position();


	while(1)
	{
		time = chVTGetSystemTime();
        //wait for new measures to be published
        messagebus_topic_wait(imu_topic, &imu_values, sizeof(imu_values));

        led_flag_panic = get_inclination(&imu_values);
////        	dac_play(NOTE_CS7); //-->>> bonne note mais pas pour  les tests lol
////        	dac_play(NOTE_CS3);
//
//        }
        if(led_flag_panic == 1){
//        	dac_play(NOTE_CS3); //-->> en pause parce que c'est chiant pendant les tests lol et  plutot le mettre ici qu'au dessus
//        	PanicMode_LED();
        	palSetPad(GPIOB, GPIOB_LED_BODY);
        }else{

//        	led_flag_panic = 0;
        	dac_stop();
        	palClearPad(GPIOB, GPIOB_LED_BODY);
        }
	}
    //100Hz
    chThdSleepUntilWindowed(time, time + MS2ST(10));
}

void PanicMode_start(void)
{
	chThdCreateStatic(waPanicMode, sizeof(waPanicMode), NORMALPRIO , PanicMode, NULL) ;
}
