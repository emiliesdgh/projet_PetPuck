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

 //// e-puck 2 main processor headers
 //#include "sensors/VL53L0X/VL53L0X.h"
 //#include "sensors/proximity.h"
 //
#include <leds.h>

#include <main.h>
#include <motors.h>
#include <danse_mode.h>


//some static global variables
static int16_t right_speed = 0;					// in [step/s]
static int16_t left_speed = 0;					// in [step/s]

// fonction qui fait bouger le moteur avec la vitesse demandée
void motor_set_danse_speed(float speed_r, float speed_l)
{
	 int speed_r_step_s,speed_l_step_s;

	 //transform the speed from cm/s into step/s
	 speed_r_step_s = -speed_r * NSTEP_ONE_TURN / WHEEL_PERIMETER;
	 speed_l_step_s = speed_l * NSTEP_ONE_TURN / WHEEL_PERIMETER;

	 right_speed = speed_r_step_s;
	 left_speed = speed_l_step_s;

	 left_motor_set_speed(left_speed);
	 right_motor_set_speed(right_speed);

}

//void dancing_puck(void){
//
//	motor_set_danse_speed(10, 10);
//
//	palTogglePad(GPIOB, GPIOB_LED_BODY);
//	chThdSleepMilliseconds(200);
//	palTogglePad(GPIOB, GPIOB_LED_BODY);
//	chThdSleepMilliseconds(200);
//
//	motor_set_danse_speed(-10, -10);
////	chThdSleepMilliseconds(400); //--> a ajouter seulement si  on met  pas les leds  et changer le premier sleep  a 400 aussi
//
//	palTogglePad(GPIOB, GPIOB_LED_BODY);
//	chThdSleepMilliseconds(200);
//	palTogglePad(GPIOB, GPIOB_LED_BODY);
//	chThdSleepMilliseconds(200);
//
//	motor_set_danse_speed(0, 0);
//}


void dancing_puck(int8_t value){

	if(value==1){

		motor_set_danse_speed(10, 10);

		palTogglePad(GPIOB, GPIOB_LED_BODY);
		chThdSleepMilliseconds(200);
		palTogglePad(GPIOB, GPIOB_LED_BODY);
		chThdSleepMilliseconds(200);

		motor_set_danse_speed(-10, -10);
//	chThdSleepMilliseconds(400); //--> a ajouter seulement si  on met  pas les leds  et changer le premier sleep  a 400 aussi

		palTogglePad(GPIOB, GPIOB_LED_BODY);
		chThdSleepMilliseconds(200);
		palTogglePad(GPIOB, GPIOB_LED_BODY);
		chThdSleepMilliseconds(200);
	}else{

		motor_set_danse_speed(0, 0);
	}
}


void GoodMorning_motors(void){

	motor_set_danse_speed(10, 10);


}
