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
#include <sensors/imu.h>
#include "chmtx.h"	//pour le mutex_decl




//include  the files from the given library
#include <spi_comm.h>
#include <leds.h>
#include <i2c_bus.h>
#include <motors.h>

#include <audio/audio_thread.h>
#include <audio/play_melody.h>

//#include <selector.h>
//include our files
#include <main.h>
#include <puck_led.h>
#include <control.h>
//#include <audio_processing.h>
#include <puck_movement.h>
#include <audio_processing.h>


//some static global variables
static int16_t right_speed = 0;					// in [step/s]
static int16_t left_speed = 0;					// in [step/s]
static int16_t speed = 0;

static int led_flag_uhOh = 0; //led_flag


imu_msg_t imu_values;

static int8_t led_flag_panic = 0; //led_flag

static float threshold_accel = 2.0;  //--->>> un define

#define ACC_OFFSET 0.0239
#define NB_ACC_SAMPLES 5

static float puck_orientation = 0;
static float puck_inclination = 0;
//
//static float puck_orientation_f = 0;
//static float puck_inclination_f = 0;
//
//static float puck_orientation2 = 0;
//static float puck_inclination2 = 0;
//
//static float accel_total = 0;

//static uint16_t distance_prox = 0;
static int ambient_testing = 0;

//semaphore
static BSEMAPHORE_DECL(image_ready_sem, TRUE);


//static uint8_t color;

enum {
    RED,
    GREEN,
	BLUE,
//    NOCOLOR
};

//*********PROXIMITY + DISTANCE TO STOP THREAD***********
messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);

//function to start proximity and obstacle encounter threads -> to be called in the main
void initial_proximity(void) {
    // TOF sensor
    VL53L0X_start();

    // inits the Inter Process Communication bus.
    messagebus_init( & bus, & bus_lock, & bus_condvar);	//pas sur de ce que c'est ni si c'est nécessaire
    // Proximity sensors
    proximity_start();
    calibrate_ir();
//    proximityToStop_start();
    chThdSleepMilliseconds(500);	//pas sur de la nécessité
}

int get_ambient_testing(void){
	return  ambient_testing;
}


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

void dancing_puck(void){

//	int16_t dance_counter = 0;

	for(int16_t dance_counter=1; dance_counter<5; dance_counter++){

		if(dance_counter==1 || dance_counter==3){

			motor_set_danse_speed(5,5);
			palTogglePad(GPIOB, GPIOB_LED_BODY);


		}else if(dance_counter ==2 || dance_counter ==4){

			motor_set_danse_speed(-5,-5);
			palTogglePad(GPIOB, GPIOB_LED_BODY);
//			chThdSleepMilliseconds(200);
		}
		chThdSleepMilliseconds(300);
	}

}
//********************************************
//

//*********OBSTACLE ENCOUNTER THREAD***********
//initialization of the obstacle encounter thread
static THD_WORKING_AREA(waObstacleEncounter, 1024);
static THD_FUNCTION(ObstacleEncounter, arg){

	chRegSetThreadName(__FUNCTION__);
	(void)arg;
    systime_t time;

	messagebus_topic_t *imu_topic = messagebus_find_topic_blocking(&bus, "/imu");

    uint16_t distance_mm = 0;

	while(1){

		time = chVTGetSystemTime();
        //wait for new measures to be published
        messagebus_topic_wait(imu_topic, &imu_values, sizeof(imu_values));

        led_flag_panic = get_inclination(&imu_values);


		//need function to modify the value of distance_mm which will be created in file proximity_sensor
		distance_mm  = VL53L0X_get_dist_mm();

		speed = motors_speed(distance_mm);

        if(led_flag_panic == 1) {
//        	dac_play(NOTE_CS7); //-->>> bonne note mais pas pour  les tests lol
//        	dac_play(NOTE_CS3); //-->> en pause parce que c'est chiant pendant les tests lol et  plutot le mettre ici qu'au dessus
//        	PanicMode_LED();
        	// et stop les moteurs
//        	speed = 0;
        	palSetPad(GPIOD, GPIOD_LED_FRONT);
        	palClearPad(GPIOB, GPIOB_LED_BODY);

        }else{

//        	dac_stop();
        	palClearPad(GPIOD, GPIOD_LED_FRONT);
        }

		if(led_flag_uhOh == 1) {
			set_allowed_to_run(0);
			set_allowed_to_move(0);
	    	uint32_t color = get_colors();
			playNote(NOTE_G4, 120);
//	    	dac_play(NOTE_G4);
			if(color==RED){

				clear_leds();
				for(int i=0; i<4; ++i){
				    set_rgb_led(i,LED_RGB_INTENSITY,0,0);
				}

			}else if(color==GREEN){

				clear_leds();
				for(int i=0; i<4; ++i){
		   			set_rgb_led(i,0,LED_RGB_INTENSITY,0);
				}

			}else if(color==BLUE){

				clear_leds();
				for(int i=0; i<4; ++i){
				    set_rgb_led(i,0,0,LED_RGB_INTENSITY);
				}
			}
//	    	dac_stop();
			playNote(NOTE_E4, 120);
//			set_allowed_to_move(0);
		}
//		chThdSleepMilliseconds(1500);
		set_allowed_to_move(1);
		set_allowed_to_run(1); //probably unnecessary

	}
		//fréquence de 100Hz
		chThdSleepUntilWindowed(time, time + MS2ST(10)); //- > mettre dans chaque thread et le 10 c'est la periode
//	}
}

//function to start the obstacle encounter thread
//to be called in thread of process audio for when he needs to be moving, not on it's own !!!
void ObstacleEncounter_start(void){

	chThdCreateStatic(waObstacleEncounter, sizeof(waObstacleEncounter), NORMALPRIO+1, ObstacleEncounter, NULL);

}

//function that checks the distance between the robot and a possible obstacle
int16_t motors_speed(uint16_t distance){

	if(distance > DISTANCE_MIN){

//		speed = SPEED_MAX;
		palSetPad(GPIOB, GPIOB_LED_BODY);//-->>test sans les moteurs
//		speed = 0;

		clear_leds();
		led_flag_uhOh = 0;

	}
	else {
		palClearPad(GPIOB, GPIOB_LED_BODY);
		led_flag_uhOh += 1;
		speed = 0;
	}

	return (int16_t)speed;
}

int get_led_flag_uhOh(void) {
	return led_flag_uhOh;
}

uint32_t get_colors(void){

	uint8_t *img_buff_ptr;

   	uint32_t redred 	= 0;
   	uint32_t greengreen = 0;
   	uint32_t blueblue 	= 0;

	// Gets the pointer to the array filled with the last image in RGB565
   	img_buff_ptr = dcmi_get_last_image_ptr();

	// Extracts and adds all pixels values of one line, by color (format RGB565)
	for(uint16_t i = 0 ; i < (2 * IMAGE_BUFFER_SIZE) ; i+=2){	// pixels are acquired on two bytes

		redred += (img_buff_ptr[i] & 0xF8) >> 2;				// red value scaled to green size

		greengreen += ((img_buff_ptr[i] & 0x07) << 3) + ((img_buff_ptr[i+1] & 0xE0) >> 5);		// green value

		blueblue += (img_buff_ptr[i+1] & 0x1F) << 1;			// blue value scaled to green size
	}

	if(redred >= greengreen && redred >= blueblue){

		return RED;

	}else if(greengreen >= blueblue){

		return GREEN;

	}else {

		return BLUE;
	}
}
//********************************************



int8_t get_inclination(imu_msg_t *imu_values){


	//create a pointer to the array for shorter name
	float *accel = imu_values->acceleration;

//	float *accel_filtered = imu_values->acc_filtered;
//
//
//
//	float *gyro = imu_values->gyro_rate;

	int8_t no_panic = 0;
	int8_t panic = 1;

	float acc_x = accel[X_AXIS];
//
//	float acc_x_2 = get_acc_filtered(accel[X_AXIS],5);
//
//
//
//
	float acc_y = accel[Y_AXIS];
	float acc_z = accel[Z_AXIS];
//
//	float acc_x_f = accel_filtered[X_AXIS];
//	float acc_y_f = accel_filtered[Y_AXIS];
//	float acc_z_f = accel_filtered[Z_AXIS];
//
//	float gyro_z = gyro[Z_AXIS];
//
//	float acc_x_y_f = sqrtf( (float)((acc_x_f * acc_x_f) + (acc_y_f * acc_y_f) ));
//
//	puck_inclination_f = 90.0 - atan2f((float)(acc_z_f),(float)( acc_x_y_f)) * CST_RADIAN;
//	puck_orientation_f = 180 - puck_inclination_f;
////
////
//	accel_total = sqrtf((float)((acc_x * acc_x) + (acc_y * acc_y) + (acc_z * acc_z)));
//
//	chprintf((BaseSequentialStream *)&SDU1, "accel_total   : %f\n", accel_total);
//	chprintf((BaseSequentialStream *)&SDU1, "acc_x   : %f\n", acc_x);
//	chprintf((BaseSequentialStream *)&SDU1, "acc_y   : %f\n", acc_y);
//	chprintf((BaseSequentialStream *)&SDU1, "acc_z   : %f\n", acc_z);
//
//
////	chprintf((BaseSequentialStream *)&SDU1, "puck_orientation_f   : %d\n", puck_orientation_f);
//
	float acc_x_y = sqrtf( (float)((acc_x * acc_x) + (acc_y * acc_y) ));

	puck_inclination = 90.0 - atan2f((float)(acc_z),(float)( acc_x_y)) * CST_RADIAN;
	puck_orientation = 180 - puck_inclination;

//	puck_inclination1 = (atan2f((float)(acc_x), (float)(acc_y)) * CST_RADIAN) + 180.0;
//	puck_orientation1 = 180 - puck_inclination1;
//
//	puck_inclination2 = 90.0 - atan2f((float)(acc_z),(float)(acc_y)) * CST_RADIAN;
//	puck_orientation2 = 180 - puck_inclination2;
//
//	chprintf((BaseSequentialStream *)&SDU1, "acc x y  : %f\n", acc_x_y);
////
//	chprintf((BaseSequentialStream *)&SDU1, "puck_inclination  : %f\n", puck_inclination);
////
//	chprintf((BaseSequentialStream *)&SDU1, "puck_inclination 1 : %f\n", puck_inclination1);

	if(fabs(accel[X_AXIS]) > threshold_accel || fabs(accel[Y_AXIS]) > 7.0){

//	if(puck_orientation > 10 && fabs(acc_z) > 12){

		return panic;

	}else{
		return no_panic;
	}
}



