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
#include <obstacle_encounter.h>
#include <puck_led.h>


#include <panic_mode.h>

//l'axe Z est l'axe en direction de  devant
//l'axe Y est l'axe en direction de coté
//l'axe X est l'axe en direction


imu_msg_t imu_values;

static int led_flag_panic = 0; //led_flag

static THD_WORKING_AREA(waPanicMode, 1024);
static THD_FUNCTION(PanicMode, arg)
{
	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	systime_t time;
	messagebus_topic_t *imu_topic = messagebus_find_topic_blocking(&bus, "/imu");

	//threshold value to not use the leds when the robot is too horizontal
	float threshold_accel = 2.0;

	//create a pointer to the array for shorter name
	float *accel = imu_values.acceleration;

	while(1)
	{
		time = chVTGetSystemTime();
        //wait for new measures to be published
        messagebus_topic_wait(imu_topic, &imu_values, sizeof(imu_values));

//        if(fabs(accel[X_AXIS]) > threshold_accel || fabs(accel[Y_AXIS]) > threshold_accel){

        if(fabs(accel[X_AXIS]) > threshold_accel){

        	led_flag_panic = 1;
//        	dac_play(NOTE_CS7); //-->>> bonne note mais pas pour  les tests lol
//        	dac_play(NOTE_CS3);

        }else{

        	led_flag_panic = 0;
        	dac_stop();
        }
        if(led_flag_panic == 1){
//        	dac_play(NOTE_CS3); //-->> en pause parce que c'est chiant pendant les tests lol
        	Led_panic_mode();
        }
	}
    //100Hz
    chThdSleepUntilWindowed(time, time + MS2ST(10));
}

void PanicMode_start(void)
{
	chThdCreateStatic(waPanicMode, sizeof(waPanicMode), NORMALPRIO , PanicMode, NULL) ;
}
