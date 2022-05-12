#include "ch.h"
#include "hal.h"
#include <main.h>
#include <usbcfg.h>
#include <chprintf.h>

#include <motors.h>
#include <audio/microphone.h>
#include <audio_processing.h>
#include <communications.h>
#include <fft.h>
#include <arm_math.h>
#include <danse_mode.h>
#include <puck_led.h>
#include <motors.h> //?


//semaphore
static BSEMAPHORE_DECL(sendToComputer_sem, TRUE);

static float micLinput[MICSAMPLESIZE];
static float micRinput[MICSAMPLESIZE];
static float micFinput[MICSAMPLESIZE];
static float micBinput[MICSAMPLESIZE];
//static int8_t dance_flag = 0;

static uint8_t mode_of_the_robot; //global bc will be set inside processAudioData


#define MIN_VALUE_THRESHOLD	10000


/*
*	Callback called when the demodulation of the four microphones is done.
*	We get 160 samples per mic every 10ms (16kHz)
*	
*	params :
*	int16_t *data			Buffer containing 4 times 160 samples. the samples are sorted by micro
*							so we have [micRight1, micLeft1, micBack1, micFront1, micRight2, etc...]
*	uint16_t num_samples	Tells how many data we get in total (should always be 640)
*	*/

void processAudioData(int16_t *data, uint16_t num_samples) {		//ask about num_samples (how much is it?) bc i never call it w/ something

	/*
	*	We get 160 samples per mic every 10ms
	*	So we fill the samples buffers to reach
	*	1024 samples, then we compute the FFTs. //can change to MICSAMPLESIZE bc idc
	*/

	#ifdef TESTING
		static uint16_t mustSend = 0;
	#endif //TESTING

	static uint16_t total_samples = 0;
	static uint16_t sample_number = 0;
	static float32_t micR_rms_value = 0;							//demander pour static declaration inside function
	static uint16_t rms_above_event = 0;
	static int direction = 0;
	static int16_t count = 0;
	float32_t current_micR_rms = 0;
	static float32_t correlation[CORRELATIONSIZE] = {0}; 			//careful, after computing direction, reinitialise to 0!!! (done in function)

//	int32_t percentage_above_loud = 0; //tells % of data points above a certain predefined level

	//loop to fill the buffers
	for(uint16_t i = 0 ; i < num_samples ; i+=4) {
		//construct an array of numbers from PCM data (can give negative values!)
		micRinput[total_samples] = (float)data[i + MIC_RIGHT];
		micLinput[total_samples] = (float)data[i + MIC_LEFT];
		micBinput[total_samples] = (float)data[i + MIC_BACK];
//		micFinput[total_samples] = (float)data[i + MIC_FRONT]; 		//dont think it's necessary bc we dont want 3D location (only 2d)

		if (micRinput[total_samples] > LOUD) { //LOUD or EVENT??
			count++;
		}

		total_samples++;

//		chprintf((BaseSequentialStream *)&SDU1, "micRinput: %f\n", micRinput[total_samples]);

	#ifdef TESTING
		mustSend++;
		if (mustSend > 20) {
			//signals to send the result to the computer
			chBSemSignal(&sendToComputer_sem);
			mustSend=0;
		}
	#endif //TESTING

		//stop when buffer is full
		if(total_samples >= (MICSAMPLESIZE)) {
			sample_number++;
			if (sample_number == 10) {
				sample_number = 0;
			}
			total_samples = 0; 	//to refill buffer later
			current_micR_rms = 0;
			break;
		}
	}

//	percentage_above_loud = 100*count/MICSAMPLESIZE;

	arm_rms_f32(micRinput, MICSAMPLESIZE, &current_micR_rms);
//	chprintf((BaseSequentialStream *)&SDU1, "current_micR_rms: %f\n", current_micR_rms);

	if (current_micR_rms > EVENT && sample_number < 10) {

		rms_above_event = rms_above_event + 1;

//		chprintf((BaseSequentialStream *)&SDU1, "rms_above_event INSIDE IF: %d\n", rms_above_event);

		if (current_micR_rms > micR_rms_value) {
			micR_rms_value = current_micR_rms;

			//calculate direction of sound -- will only follow it if later finds out it was only a call and not music playing
			arm_correlate_f32(micRinput, MICSAMPLESIZE, micLinput, MICSAMPLESIZE, correlation);

			int32_t shiftRL = 0;
			shiftRL = get_shift(correlation);
			//chprintf((BaseSequentialStream *)&SDU1, "shiftRL: %d\n", shiftRL);
			//chprintf((BaseSequentialStream *)&SDU1, "here\n");

			arm_correlate_f32(micLinput, MICSAMPLESIZE, micBinput, MICSAMPLESIZE, correlation);
			int32_t shiftLB = 0;
			shiftLB = get_shift(correlation);
			//chprintf((BaseSequentialStream *)&SDU1, "shiftLB: %d\n", shiftLB);

			arm_correlate_f32(micRinput, MICSAMPLESIZE, micBinput, MICSAMPLESIZE, correlation);
			int32_t shiftRB = 0;
			shiftRB = get_shift(correlation);
			//chprintf((BaseSequentialStream *)&SDU1, "shiftRB: %d\n", shiftRB);

			direction = get_direction(shiftRL, shiftLB, shiftRB);
//			chprintf((BaseSequentialStream *)&SDU1, "direction is: %d\n", direction);
//			chThdSleepMilliseconds(2000);
		}
	}

//	chprintf((BaseSequentialStream *)&SDU1, "rms_above_event OUTSIDE IF: %d\n", rms_above_event);


	if (sample_number == 9) {

		//if music:
		if (rms_above_event > 8) { // && percentage_above_loud > 0.4) 		//!!! use DEFINE for the 10 (in case MICSAMPLESIZE is changed
			sample_number = 0;
			//chprintf((BaseSequentialStream *)&SDU1, "cond 1\n");
			//if call or whistle
			//set mode_of_the_robot = MOT
		} else if (rms_above_event > 0) {
			chprintf((BaseSequentialStream *)&SDU1, "cond 2\n");
			//set mode_of_the_robot = MOT
			chprintf((BaseSequentialStream *)&SDU1, "direction is: %d\n", direction);
			run_to_direction(direction);
			direction = 0;					//reset the direction
			sample_number = 0;
//			chThdSleepMilliseconds(2000);
		} else {
			//chprintf((BaseSequentialStream *)&SDU1, "cond 3\n");
			sample_number = 0;
//			stay_put();
		}
		rms_above_event = 0;
		micR_rms_value = 0;
		count = 0;
	}

}

uint8_t check_for_call(float *data, uint16_t num_samples, int32_t stream_avg) {
	uint8_t hereboy = 0;
	int32_t signalR_max = stream_avg + CLAP; //value to add? idk
	int32_t signalR_min = stream_avg - CLAP;
//	float signalR_min = stream_avg * 0.10;
		for (uint32_t i = 0; i < num_samples; i++) {
			hereboy = !((data[i] < signalR_max) && (data[i] > signalR_min));
		}
	return hereboy;
}

int32_t get_micro_average(float *micro_ID, uint16_t sample_size)
{
	long sum = 0;
	int i;

		for (i=0; i<sample_size+1; i++)
		{
			sum += micro_ID[i];
		}
	return ((int)(sum/sample_size));
}

void wait_send_to_computer(void) {
	chBSemWait(&sendToComputer_sem);
}

float* get_audio_buffer_ptr(BUFFER_NAME_t name){ //to try and get PCM data
	if(name == MIC_R_INPUT){
		return micRinput;
	}
	if(name == MIC_L_INPUT){
			return micLinput;
	}
	if(name == MIC_F_INPUT){
			return micFinput;
	}
	if(name == MIC_B_INPUT){
			return micBinput;
	} else {
		return NULL;
	}
}

int get_direction(int32_t shift1, int32_t shift2, int32_t shift3) {
	int direction = 0;
	//shift1: RL //shift2: LB //shift3: RB

	if (shift1 > MAXDELTA1) { //R lags L
		direction = 7; //was3
	} else if (shift1 < -MAXDELTA1) { //L lags R
		direction = 3; //was7
	} else if (shift1 > 0) { //gets to L first
		if (shift2 > 0) { 	 //L lags B
			direction = 6;
		} else {
			direction = 8;
		}
	} else if (shift1 < 0) { //gets to R first
		if (shift3 > 0) {	 //R lags B
			direction = 4;
		} else {
			direction = 2;
		}
	} else {//if (shift1 == 0) {
		if (shift2 < 0 || shift3 < 0) { //B lags R or L
			direction = 1;
		} else {
			direction = 5;
		}
	}
	return direction;
}

int32_t get_shift(float *carray) {
	float cmax = 0;
	int32_t shift = 0;
	for (int32_t i = 0 ; i < CORRELATIONSIZE ; i++) {
		if (carray[i] > cmax) {
			shift = i - MICSAMPLESIZE;	// - 1;
			cmax = carray[i];
		}
		carray[i] = 0;
	}
	return shift;
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

void run_to_direction(int direction) {
	if (0 < direction && direction < 9) {
		rotate_to_led(direction);
		move_straight();
	}
	else {
		stay_put();
	}
}
void stay_put(void) {
	right_motor_set_speed(STOP);
	left_motor_set_speed(STOP);
}
void rotate_to_angle(int angle) {

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
	right_motor_set_pos(STOP);
	left_motor_set_pos(STOP);
	right_motor_set_speed(STOP);
	left_motor_set_speed(STOP);
}


void set_mode_of_the_robot(uint8_t new_mode){
	mode_of_the_robot = new_mode;
}

