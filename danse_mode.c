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
#include <audio_processing.h>


//some static global variables
static int16_t right_speed = 0;					// in [step/s]
static int16_t left_speed = 0;					// in [step/s]
static uint8_t state_motor = 0;

static int16_t counter_step_right = 0;			// in [step]
static int16_t counter_step_left = 0;

static int16_t dance_counter = 0;

static int16_t position_to_reach_right = 0; 	// in [step]
static int16_t position_to_reach_left = 0;	 	// in [step]
static uint8_t position_right_reached = 0;
static uint8_t position_left_reached = 0;



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
//
//	 //flag for speed control
//	 	 state_motor = SPEED_CONTROL;

}


void motor_set_danse(float speed_r, float speed_l,float position)
{
	 int speed_r_step_s,speed_l_step_s;

	 //transform the speed from cm/s into step/s
	 speed_r_step_s = -speed_r * NSTEP_ONE_TURN / WHEEL_PERIMETER;
	 speed_l_step_s = speed_l * NSTEP_ONE_TURN / WHEEL_PERIMETER;

	 right_speed = speed_r_step_s;
	 left_speed = speed_l_step_s;

	 if(dance_counter < position/2){

		 left_motor_set_speed(speed_l_step_s);
		 right_motor_set_speed(speed_r_step_s);

	 }else if(dance_counter > position/2 && dance_counter < position){

		 left_motor_set_speed(-speed_l_step_s);
		 right_motor_set_speed(-speed_r_step_s);

	 }else if(dance_counter == position){

		 dance_counter = 0;

	 }
//	 dance_counter ++;


//
//	 //flag for speed control
//	 	 state_motor = SPEED_CONTROL;

}

void motor_set_danse_position(float position_r, float position_l, float speed_r, float speed_l)
{
    //reinit global variable
    counter_step_left = 0;
    counter_step_right = 0;

    position_right_reached = 0;
    position_left_reached = 0;

    //Set global variable with position to reach in step
    position_to_reach_left = position_l * NSTEP_ONE_TURN / WHEEL_PERIMETER;
    position_to_reach_right = -position_r * NSTEP_ONE_TURN / WHEEL_PERIMETER;

    motor_set_danse_speed(speed_r, speed_l);

    //flag for position control, will erase flag for speed control only
    state_motor = POSITION_CONTROL;


}

void dancing_puck(void){

//	dance_counter = 0;

	int8_t value = get_value_test_in_dance(); //faire fonction qui retourne la valeur du dance_flag

	if(value==1){

		for(dance_counter=1; dance_counter<5; dance_counter++){
			if(dance_counter==1 || dance_counter==3){

				motor_set_danse_speed(5,5);


			}else if(dance_counter ==2 || dance_counter ==4){

				motor_set_danse_speed(-5,-5);
//				chThdSleepMilliseconds(200);
			}
			chThdSleepMilliseconds(300);
		}

	}else if(value==0){

		motor_set_danse_speed(0,0);
//		dance_counter =0;
	}
//
//	motor_set_danse(5,5,10);
//
//	motor_set_danse_position(PERIMETER_EPUCK/2, PERIMETER_EPUCK/2, 5, 5);
//	 while(motor_position_reached() != POSITION_REACHED);

//	motor_set_danse_speed(10, 10);

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
}

uint8_t motor_position_reached(void)
{
    if(state_motor == POSITION_CONTROL && position_right_reached && position_left_reached){

        return POSITION_REACHED;

    }else{

        return POSITION_NOT_REACHED;
    }
}


void GoodMorning_motors(void){

	motor_set_danse_speed(10, 10);


}
