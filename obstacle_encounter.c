#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ch.h"
#include "hal.h"
#include "memory_protection.h"
#include <usbcfg.h>
#include <main.h>
#include <motors.h>

#include <obstacle_encounter.h>






//céation/définition du threads des Good night LEDS
static THD_WORKING_AREA(waObstacleEncounter, 256);
static THD_FUNCTION(ObstacleEncounter, arg){

	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	while(1){

	}

}

void ObstacleEncounter_start(void){

    //création/ initialisation des threads dans la fonction main
    chThdCreateStatic(waObstacleEncounter, sizeof(ObstacleEncounter), NORMALPRIO, ObstacleEncounter, NULL);

}
