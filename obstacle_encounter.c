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

#include <leds.h>
#include <spi_comm.h>

#include <selector.h>
#include <main.h>
#include <motors.h>

#include <obstacle_encounter.h>
#include <proximity_sensor.h>

static int16_t speed = 0;

//fonction impliquant la condition de l'état des moteurs en fonction de si y'a un obstacle ou pas
int16_t motors_speed(uint16_t distance){

	if(distance > DISTANCE_MIN){

		speed = SPEED_MAX;
	}
	else{

		speed = 0;
		//call 'uh-oh'
	}

	return (int16_t)speed;
}

//céation/définition du threads obstacle encounter
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

		//fréquence de 100Hz
		chThdSleepUntilWindowed(time, time + MS2ST(10)); //- > mettre dans chaque thread et le 10 c'est la periode
	}
}

void ObstacleEncounter_start(void){

    //création/ initialisation des threads dans la fonction main
    chThdCreateStatic(waObstacleEncounter, sizeof(waObstacleEncounter), NORMALPRIO, ObstacleEncounter, NULL);

}
