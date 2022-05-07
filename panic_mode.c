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

//#include <selector.h>
//include our files
#include <main.h>
#include <proximity_sensor.h>
#include <puck_led.h>


#include <panic_mode.h>




imu_msg_t imu_values;

static int led_flag_panic = 0; //led_flag

static THD_WORKING_AREA(waPanicMode, 1024);
static THD_FUNCTION(PanicMode, arg)
{
	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	systime_t time;
	messagebus_topic_t *imu_topic = messagebus_find_topic_blocking(&bus, "/imu");

	//******TP3********
	//threshold value to not use the leds when the robot is too horizontal
	float threshold_accel = 2.0;
	float threshold_gyro = 0.2;

	//create a pointer to the array for shorter name
	float *accel = imu_values.acceleration;

	float *gyro = imu_values.gyro_rate;
	//*****************
	float init_gyro = get_gyro_rate(X_AXIS);

	while(1)
	{
		time = chVTGetSystemTime();
        //wait for new measures to be published
        messagebus_topic_wait(imu_topic, &imu_values, sizeof(imu_values));
        stopCurrentMelody();

        if(fabs(accel[X_AXIS]) > threshold_accel || fabs(accel[Y_AXIS]) > threshold_accel){

//        	palSetPad(GPIOD, GPIOD_LED_FRONT);
        	led_flag_panic = 1;

        }else{

//        	palClearPad(GPIOD, GPIOD_LED_FRONT);
        	led_flag_panic = 0;
        }
        if(led_flag_panic == 1){
        	Led_panic_mode();
        }
//        if(fabs(gyro[X_AXIS]) > threshold_gyro || fabs(gyro[Y_AXIS]) > threshold_gyro){
//
//        	palSetPad(GPIOB, GPIOB_LED_BODY);
//
//        }else{
//
//        	palClearPad(GPIOB, GPIOB_LED_BODY);
//        }


//        if (abs( get_gyro_rate(X_AXIS) - init_gyro)  > ANGULAR_ACC_DEATH )
//        {
//        	led_flag_panic = 1;
//        }

//l'axe Z est l'axe en direction de  devant
//l'axe Y est l'axe en direction de coté
//l'axe X est l'axe en direction
////       if( get_acceleration(X_AXIS) !=0){
//       if( get_acceleration(X_AXIS) > ANGULAR_ACC_DEATH){
//
//    	   palTogglePad(GPIOD, GPIOD_LED_FRONT);
//
//    	   led_flag_panic = 1;
//       }
//
//
////        if (get_gyro_rate(X_AXIS) != 0 )
////                {
////                	led_flag_panic = 1;
////        }
//       else if(get_acceleration(X_AXIS) < ANGULAR_ACC_DEATH){
//
//        	led_flag_panic = 0;
//        	palTogglePad(GPIOB, GPIOB_LED_BODY);
//        }
//        if(led_flag_panic == 1){
//        	Led_panic_mode();
//        }
	}
    //100Hz
    chThdSleepUntilWindowed(time, time + MS2ST(10));
}

void PanicMode_start(void)
{
	chThdCreateStatic(waPanicMode, sizeof(waPanicMode), NORMALPRIO , PanicMode, NULL) ;
}
