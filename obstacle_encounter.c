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

//include  the files from the given library
#include <spi_comm.h>
#include <leds.h>
#include <motors.h>

#include <audio/audio_thread.h>
#include <audio/play_melody.h>

//#include <selector.h>
//include our files
#include <main.h>
#include <proximity_sensor.h>
#include <puck_led.h>

#include <obstacle_encounter.h>


static int16_t speed = 0;
static int led_flag_uhOh = 0; //led_flag

//function that checks the distance between the robot and a possible obstacle
int16_t motors_speed(uint16_t distance){

	if(distance > DISTANCE_MIN){

		speed = SPEED_MAX;
		led_flag_uhOh = 0;
	}
	else{
		led_flag_uhOh += 1;
		speed = 0;
		//call 'uh-oh'
	}

	return (int16_t)speed;
}

//initialization of the obstacle encounter thread
static THD_WORKING_AREA(waObstacleEncounter, 256);
static THD_FUNCTION(ObstacleEncounter, arg){

	chRegSetThreadName(__FUNCTION__);
	(void)arg;
    systime_t time;

    uint16_t distance_mm = 0;

	while(1){
		time = chVTGetSystemTime();

		//need function to modify the value of distance_mm which will be created in file proximity_sensor
		distance_mm = get_distance_toStop();

		speed = motors_speed(distance_mm);

		left_motor_set_speed(speed);
		right_motor_set_speed(speed);

		if(led_flag_uhOh == 1){
			playNote(NOTE_G4, 120);
			Led_uhOh();
			playNote(NOTE_E4, 120);
		}

		//fréquence de 100Hz
		chThdSleepUntilWindowed(time, time + MS2ST(10)); //- > mettre dans chaque thread et le 10 c'est la periode
	}
}

//function to start the obstacle encounter thread
void ObstacleEncounter_start(void){

	chThdCreateStatic(waObstacleEncounter, sizeof(waObstacleEncounter), NORMALPRIO, ObstacleEncounter, NULL);

}
