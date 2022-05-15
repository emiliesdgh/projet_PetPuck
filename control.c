#include "ch.h"
#include "hal.h"
#include <main.h>
#include <usbcfg.h>
#include <chprintf.h>
#include <control.h>
#include <puck_movement.h>
#include <motors.h>
#include <audio_processing.h>
#include <selector.h>

static uint8_t robot_moves;
static uint8_t direction_to_follow;
static uint8_t position_reached = 1;

static THD_WORKING_AREA(waControl, 2048);
static THD_FUNCTION(Control, arg) {

	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	systime_t time;

	while(1) {

		while(get_selector()==0){
					chThdYield();
		//			chThdSleepMilliseconds(1000);
		}
		time = chVTGetSystemTime();

		switch (get_robot_moves()) {
			case MIC:
				right_motor_set_speed(STOP);
				left_motor_set_speed(STOP);
				set_position_reached(1);
				break;
			case HEREBOY:
				run_to_direction(get_direction_to_follow());
				set_position_reached(1);

				break;
			case DANCE:
				dancing_puck();
				right_motor_set_speed(STOP);
				left_motor_set_speed(STOP);

				if(get_reset_direction() == 1){
					set_direction_to_follow(0);
					clear_reset_direction();
				}

				set_position_reached(1);
				//call dance function here
				break;
			default:
				set_position_reached(1);
				break;

		}
		if (get_sample_number() == 9) {
			set_sample_number(0);
		}
		set_allowed_to_move(1);
		//100Hz
		chThdSleepUntilWindowed(time, time + MS2ST(10));
	}

}


void Control_start(void) {
	chThdCreateStatic(waControl, sizeof(waControl), NORMALPRIO, Control, NULL) ;
}

void set_robot_moves(uint8_t new_mode) {
	robot_moves = new_mode;
}

uint8_t get_robot_moves(void) {
	return robot_moves;
}

void set_direction_to_follow(uint8_t new_direction) {
	direction_to_follow = new_direction;
}

uint8_t get_direction_to_follow(void) {
	return direction_to_follow;
}

void set_position_reached(uint8_t reached) {
	position_reached = reached;
}

uint8_t get_position_reached(void) {
	return position_reached;
}

void rotate_to_led(int led) {
    switch (led) {
        case 1:
        	rotate_to_angle(LED1ANGLE);
            break;
        case 2:
            rotate_to_angle(LED2ANGLE);
            break;
        case 3:
            rotate_to_angle(LED3ANGLE);
            break;
        case 4:
            rotate_to_angle(LED4ANGLE);
            break;
        case 5:
            rotate_to_angle(LED5ANGLE);
            break;
        case 6:
            rotate_to_angle(LED6ANGLE);
            break;
        case 7:
            rotate_to_angle(LED7ANGLE);
            break;
        case 8:
            rotate_to_angle(LED8ANGLE);
            break;
        default:
            break;
    }
}

void run_to_direction(uint8_t direction) {

	if (0 < direction && direction < 9){// && get_puck_playing_sound() ==0) {// && !get_led_flag_uhOh()) {

		if(get_reset_direction() == 1){
			direction = 0;
			set_direction_to_follow(0);
			stay_put();

			clear_reset_direction();
		}

		rotate_to_led(direction);
		move_straight();

	} else {
		stay_put();
	}
}
void stay_put(void) {
	right_motor_set_speed(STOP);
	left_motor_set_speed(STOP);
}


void rotate_to_angle(int angle) {
	right_motor_set_pos(STOP);
	left_motor_set_pos(STOP);

	int32_t steps = 0;
    steps = angle_to_step(angle);

    if (steps != 0) { 					//if = 0, only needs to go straight
    	if (angle > LED5ANGLE) {
			//turn anticlockwise
    		steps = ONETURNROBOT - steps;
			while (right_motor_get_pos() <= steps) {
				right_motor_set_speed(+TURNSPEED);
				left_motor_set_speed(-TURNSPEED);
			}
		} else {
			//turn clockwise
			while (left_motor_get_pos() <= steps) {
				right_motor_set_speed(-TURNSPEED);
				left_motor_set_speed(+TURNSPEED);
			}
		}
    }
	right_motor_set_pos(STOP);
	left_motor_set_pos(STOP);
}


int angle_to_step(int angle) {
    return (int)(STEPPERDEGREE*angle);
}

void move_straight(void) {
	int cm = 0;
	// move straight
	right_motor_set_speed(STOP);
	left_motor_set_speed(STOP);

	while (cm < XCMSTEP && !get_led_flag_uhOh()){// && get_direction_to_follow()!=0) {
    	right_motor_set_speed(+2*TURNSPEED);
    	left_motor_set_speed(+2*TURNSPEED);
    	cm++;
    }
	right_motor_set_speed(STOP);
	left_motor_set_speed(STOP);
}
