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

#include <proximity.h>
#include <leds.h>
#include <spi_comm.h>

#include <selector.h>
#include <main.h>
#include <motors.h>

#include <obstacle_encounter.h>
#include <proximity_sensor.h>


static uint16_t distance = 0;

//céation/définition du threads obstacle encounter

static THD_WORKING_AREA(waProximityToStop, 256);
static THD_FUNCTION(ProximityToStop, arg){

	chRegSetThreadName(__FUNCTION__);
	(void)arg;
    systime_t time;

	proximity_start();

	while(1){
		time = chVTGetSystemTime();




		//fréquence de 100Hz
		chThdSleepUntilWindowed(time, time + MS2ST(10)); //- > mettre dans chaque thread et le 10 c'est la periode
	}
}

void ProximityToStop_start(void){

    //création/ initialisation des threads dans la fonction main
    chThdCreateStatic(waProximityToStop, sizeof(ProximityToStop), NORMALPRIO, ObstacleEncounter, NULL);

}


uint16_t get_distance(void){
	return distance;
}


