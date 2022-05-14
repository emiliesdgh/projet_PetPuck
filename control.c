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

static THD_WORKING_AREA(waControl, 2048);
//static thread_t *Controlp;
static THD_FUNCTION(Control, arg) {

	chRegSetThreadName(__FUNCTION__);
	(void)arg;
	systime_t time;
	//Controlp=chThdGetSelfX();
//	uint8_t printing = 0;

	while(1) {
		time = chVTGetSystemTime();
//		msg_t msg;
		switch (get_robot_moves()) {
			case MIC:
//				chprintf((BaseSequentialStream *)&SDU1, "casemic: %d \n", time);
				right_motor_set_speed(STOP);
				left_motor_set_speed(STOP);
				set_position_reached(1);
				//do nothing bc getting mic values?
				break;
			case HEREBOY:
//				position_reached = 0;
//				if (get_led_flag_uhOh() == 1) {
//					break;
//				}
//				while(!get_position_reached()) {
				//three lines to test thd
//					printing = get_direction_to_follow();
//					chprintf((BaseSequentialStream *)&SDU1, "printing inside thread: %d\n", printing);
					run_to_direction(get_direction_to_follow());
					set_position_reached(1);
//					set_direction_to_follow(0);
//					if(get_puck_playing_sound()){
//						break;
//					}
//				}
				break;
			case DANCE:
				dancing_puck();

				if(get_reset_direction() == 1){
	//							direction = 0;
					set_direction_to_follow(0);
		//			chprintf((BaseSequentialStream *)&SDU1, "direction in if  get_reset_direction  : %d\n", direction);

		//			chprintf((BaseSequentialStream *)&SDU1, "direction_to_follow  in if  get_reset_direction  : %d\n", get_direction_to_follow());

					clear_reset_direction();
				}
//				right_motor_set_speed(STOP);
//				left_motor_set_speed(STOP);
				set_position_reached(1);
				//call dance function here
				break;
			default:
				set_position_reached(1);
				break;

		}
//		chprintf((BaseSequentialStream *)&SDU1, "sample num in thread: %d\n", get_sample_number());
		if (get_sample_number() == 9) {
			set_sample_number(0);
		}
		set_allowed_to_move(1);
//		chprintf((BaseSequentialStream *)&SDU1, "sample# inside thread: %d\n", get_sample_number());
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
//	chprintf((BaseSequentialStream *)&SDU1, "inside run \n");
	if (0 < direction && direction < 9){// && get_puck_playing_sound() ==0) {// && !get_led_flag_uhOh()) {

		if(get_reset_direction() == 1){
			direction = 0;
			set_direction_to_follow(0);
//			chprintf((BaseSequentialStream *)&SDU1, "direction in if  get_reset_direction  : %d\n", direction);

//			chprintf((BaseSequentialStream *)&SDU1, "direction_to_follow  in if  get_reset_direction  : %d\n", get_direction_to_follow());

			clear_reset_direction();
		}

		rotate_to_led(direction);
		move_straight();
//		chprintf((BaseSequentialStream *)&SDU1, "after straight \n");
//		set_position_reached(1);
	} else {
		stay_put();
//		chprintf((BaseSequentialStream *)&SDU1, "didnt do shit \n");
//		set_position_reached(1);
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

//	chprintf((BaseSequentialStream *)&SDU1, "direction in move straight  : %d\n", get_direction_to_follow());

	while (cm < XCMSTEP && !get_led_flag_uhOh()){// && get_direction_to_follow()!=0) {
    	right_motor_set_speed(+2*TURNSPEED);
    	left_motor_set_speed(+2*TURNSPEED);
    	cm++;
    }
//	right_motor_set_pos(STOP);
//	left_motor_set_pos(STOP);
	right_motor_set_speed(STOP);
	left_motor_set_speed(STOP);
}
