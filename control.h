#ifndef CONTROL_H
#define CONTROL_H

#define MIC 		0
#define HEREBOY 	1
#define DANCE		2

#define WHEELPERIMETER	12.9 //in cm
#define CIRCLE			360
#define ONETURNSTEP		1000 //20 steps/rev for the motor, 50:1 reduction gear: 20*50 steps for a complete wheel turn
#define ONETURNROBOT	1290 //the two wheels are 53mm apart so one full rotation is 53*pi, which is 166.5mm.
							 //Divided by the wheel perimeter, times the number of step in one revolution //not sure about the value
							 //pi*5.3*(1/12.9)*1000
#define STEPPERDEGREE	3.58 // 3.58  = (ONETURNROBOT/CIRCLE)
#define STEPPERCM		77.5 // 77.5 = (ONETURNSTEP/WHEELPERIMETER)
#define XCMSTEP			(STEPPERCM*500)
#define HALFTURNROBOT	646
#define TURNSPEED		500
#define LED1ANGLE		0
#define LED2ANGLE		45
#define LED3ANGLE		90
#define LED4ANGLE		135
#define LED5ANGLE		180
#define LED6ANGLE		225
#define LED7ANGLE		270
#define LED8ANGLE		315
#define RIGHTTURNLIMIT	(LED5ANGLE*STEPPERDEGREE)
#define STOP			0 //stop speed (stop moving motors)

void Control_start(void);

void set_robot_moves(uint8_t new_mode);
uint8_t get_robot_moves(void);
void set_direction_to_follow(uint8_t new_direction);
uint8_t get_direction_to_follow(void);
void set_position_reached(uint8_t reached);
uint8_t get_position_reached(void);


void run_to_direction(uint8_t direction);
void rotate_to_angle(int angle);
int angle_to_step(int angle);
void move_straight(void);
void stay_put(void);

#endif
