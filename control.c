#include "ch.h"
#include "hal.h"
#include <main.h>
#include <usbcfg.h>
#include <chprintf.h>


#include <obstacle_encounter.h>

static uint8_t robot_moves; //global bc

void set_mode_of_the_robot(uint8_t new_mode) {
	robot_moves = new_mode;
}
static THD_WORKING_AREA(waControl, 1024);
static THD_FUNCTION(Control, arg) {

	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	while(1) {
		time = chVTGetSystemTime();

		switch (robot_moves) {
			case MIC:
				//do nothing bc getting mic values
			case MOT:
				if(/*pos not reached*/) {
					//set speed non 0
				} else {
					//set speed to 0
				}
				robot_moves = MIC; //not necessary
		}
		 //100Hz
		 chThdSleepUntilWindowed(time, time + MS2ST(10));
	 }
}


void Control_start(void)
{
	chThdCreateStatic(waControl, sizeof(waControl), NORMALPRIO, Control, NULL) ;
}
