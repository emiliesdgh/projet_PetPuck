#include "ch.h"
#include "hal.h"
#include <main.h>
#include <usbcfg.h>
#include <chprintf.h>
#include <control.h>
#include <puck_movement.h>
#include <motors.h>
#include <audio_processing.h>

static uint8_t robot_moves;
static uint8_t direction_to_follow;
static uint8_t position_reached = 1;

static THD_WORKING_AREA(waControl, 1024);
//static thread_t *Controlp;
static THD_FUNCTION(Control, arg) {

	chRegSetThreadName(__FUNCTION__);
	(void)arg;
	systime_t time;
	//Controlp=chThdGetSelfX();
	uint8_t printing = 0;

	while(1) {
		time = chVTGetSystemTime();
//		msg_t msg;
		switch (get_robot_moves()) {
			case MIC:
//				chprintf((BaseSequentialStream *)&SDU1, "casemic: %d \n", time);
				set_position_reached(1);
				//do nothing bc getting mic values?
				break;
			case HEREBOY:
//				position_reached = 0;
				while(!get_position_reached()) {
				//three lines to test thd
					printing = get_direction_to_follow();
					chprintf((BaseSequentialStream *)&SDU1, "printing inside thread: %d\n", printing);
					run_to_direction(get_direction_to_follow());
				}
				break;
			case DANCE:
				dancing_puck();
				right_motor_set_speed(STOP);
				left_motor_set_speed(STOP);
				set_position_reached(1);
				//call dance function here
				break;
			default:
				set_position_reached(1);
				break;

		}
		set_allowed_to_move(1);
		//100Hz
		chThdSleepUntilWindowed(time, time + MS2ST(10));
	}

}


void Control_start(void) {
	chThdCreateStatic(waControl, sizeof(waControl), NORMALPRIO+1, Control, NULL) ;
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
	chprintf((BaseSequentialStream *)&SDU1, "inside run \n");
	if (0 < direction && direction < 9) {
		rotate_to_led(direction);
		move_straight();
		chprintf((BaseSequentialStream *)&SDU1, "after straight \n");
		set_position_reached(1);
	}
	else {
		stay_put();
		chprintf((BaseSequentialStream *)&SDU1, "didnt do shit \n");
		set_position_reached(1);

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
    //chprintf((BaseSequentialStream *)&SDU1, "steps to rot: %d\n", steps);

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

	while (cm < XCMSTEP) {
    	right_motor_set_speed(+2*TURNSPEED);
    	left_motor_set_speed(+2*TURNSPEED);
    	cm++;
    }
//	right_motor_set_pos(STOP);
//	left_motor_set_pos(STOP);
	right_motor_set_speed(STOP);
	left_motor_set_speed(STOP);
}
