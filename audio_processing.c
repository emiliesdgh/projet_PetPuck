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

#include <puck_led.h>
#include <motors.h> //?
#include <puck_movement.h>


//semaphore
static BSEMAPHORE_DECL(sendToComputer_sem, TRUE);

static float micLinput[MICSAMPLESIZE];
static float micRinput[MICSAMPLESIZE];
static float micFinput[MICSAMPLESIZE];
static float micBinput[MICSAMPLESIZE];

//tableau de valeurs des micro

static int8_t dance_flag = 0;


#define MIN_VALUE_THRESHOLD	10000

int8_t get_value(void){
	return dance_flag;
}

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


	static uint16_t total_samples = 0;  //nb total de sample on check que c'est égal a  1600
//	#ifdef TESTING
//		static uint16_t mustSend = 0;
//	#endif //TESTING
	static uint16_t sample_number = 0;	//nb de sample, le chiffre qui compte les paquets jusqu'à 10



	static float32_t micR_rms_value = 0;	//rms value de 1 bloc de 1600 samples on store le plus grand		//demander pour static declaration inside function
	float32_t current_micR_rms = 0;			// meme chose  mais actuel

	static uint8_t rms_above_event = 0;		//si  rms du mic est plus grand  que event -> un threashold  pour filtrer le bruit

	static int8_t direction = 0;



	int16_t count = 0;	//un compteur  qui compte chaque valeur de micro audessus  de loud

	int16_t percentage_above_loud = 0; // le poucentage de count  //tells % of data points above a certain predefined level

	//loop to fill the buffers
	for(uint16_t i = 0 ; i < num_samples ; i+=4) {
		//construct an array of numbers from PCM data (can give negative values!)
		micRinput[total_samples] = (float)data[i + MIC_RIGHT];
		micLinput[total_samples] = (float)data[i + MIC_LEFT];
		micBinput[total_samples] = (float)data[i + MIC_BACK];
//		micFinput[total_samples] = (float)data[i + MIC_FRONT]; //dont think it's necessary bc we dont want 3D location (only 2d)
		chprintf((BaseSequentialStream *)&SDU1, "R  mic input   : %d\n", micRinput);

//		chprintf((BaseSequentialStream *)&SDU1, "L  mic input   : %f\n", micLinput);

		chprintf((BaseSequentialStream *)&SDU1, "B  mic input   : %f\n", micBinput);

//		chprintf((BaseSequentialStream *)&SDU1, "F  mic input   : %f\n", micFinput);

		if (micRinput[total_samples] > LOUD) { //LOUD or EVENT??
			count++;
		}

		total_samples++;

//	#ifdef TESTING
//		mustSend++;
//		if (mustSend > 20) {
//			//signals to send the result to the computer
//			chBSemSignal(&sendToComputer_sem);
//			mustSend=0;
//		}
//	#endif //TESTING


		//stop when buffer is full
		if(total_samples >= (MICSAMPLESIZE)) {
			sample_number++;

			total_samples = 0; 	//to refill buffer later
			current_micR_rms = 0;

			break;
		}
	}

	percentage_above_loud = 100*count/MICSAMPLESIZE;

	arm_rms_f32(micRinput, MICSAMPLESIZE, &current_micR_rms); //calcul la  rsm value

	if (current_micR_rms > EVENT) {// && sample_number < 10) {

		rms_above_event++;

		if (current_micR_rms > micR_rms_value) {

			micR_rms_value = current_micR_rms;
			//calculate direction of sound -- will only follow it if later finds out it was only a call and not music playing
			static float32_t correlation[CORRELATIONSIZE] = {0}; //careful, after computing direction, reinitialise to 0!!!

			arm_correlate_f32(micRinput, MICSAMPLESIZE, micLinput, MICSAMPLESIZE, correlation); //<- causes problems idk how to fix
			/*stopped giving an error when i changed MICSAMPLESIZE to 24 (low value), still gave error w/ 160*/
//			chprintf((BaseSequentialStream *)&SDU1, "COrrelation1 : %f\n", correlation);
			int32_t shiftRL = 0;
			shiftRL = get_shift(correlation); //make a function?


			arm_correlate_f32(micLinput, MICSAMPLESIZE, micBinput, MICSAMPLESIZE, correlation);
//			chprintf((BaseSequentialStream *)&SDU1, "COrrelation2   : %f\n", correlation);
			int32_t shiftLB = 0;
			shiftLB = get_shift(correlation);


			arm_correlate_f32(micRinput, MICSAMPLESIZE, micBinput, MICSAMPLESIZE, correlation);
//			chprintf((BaseSequentialStream *)&SDU1, "COrrelation3   : %f\n", correlation);
			int32_t shiftRB = 0;
			shiftRB = get_shift(correlation);



			direction = get_direction(shiftRL, shiftLB, shiftRB);
		}
	}

	if (sample_number == 10) {

		sample_number = 0;

		micR_rms_value = 0;

		//if music:
		if (rms_above_event > 5 && percentage_above_loud > 0.5) { //!!! use DEFINE for the 10 (in case MICSAMPLESIZE is changed

			dance_flag = 1; //yes dancing

			rms_above_event = 0;
			count = 0;

		//if call or whistle
		} else if (rms_above_event > 0) {

//			chprintf((BaseSequentialStream *)&SDU1, "direction is: %d\n", direction);

			dance_flag = 0; //no dancing

			rms_above_event = 0;
			count = 0;
//			run_to_direction(direction);

			direction = 0;		//reset the direction

//			chThdSleepMilliseconds(2000);
		//if  nothing just reset all
		} else {
			//chprintf((BaseSequentialStream *)&SDU1, "cond 3\n");
			dance_flag = 0; //no dancing
			rms_above_event = 0;
			count = 0;
//			palTogglePad(GPIOB, GPIOB_LED_BODY);

		}
	}
}

uint8_t check_for_call(float *data, uint16_t num_samples, int32_t stream_avg) {
	uint8_t hereboy = 0;
	int32_t signalR_max = stream_avg + CLAP; //value to add? idk
	int32_t signalR_min = stream_avg - CLAP;
	//float signalR_min = stream_avg * 0.10;
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

int8_t get_direction(int32_t shift1, int32_t shift2, int32_t shift3) {
	int8_t direction = 0;
	//shift1: RL //shift2: LB //shift3: RB
	//int8_t direction1 = 0;
	//int8_t direction2 = 0;
//	chprintf((BaseSequentialStream *)&SDU1, "shift R - L : %d\n", shift1);
//
//	chprintf((BaseSequentialStream *)&SDU1, "shift L - B : %d\n", shift2);
//
//	chprintf((BaseSequentialStream *)&SDU1, "shift R - B : %d\n", shift3);

	if (shift1 > MAXDELTA1) {

		direction = 7;

	} else if (shift1 < - MAXDELTA1) {

		direction = 3;

	} else if (shift1 > 0) {

		if (shift2 >= 0) {

			direction = 8;

		} else {

			direction = 6;
		}
	} else if (shift1 < 0) { //changed sig
		if (shift3 >= 0) {

			direction = 2;

		} else {

			direction = 4;
		}
	} else {
		if (shift2 >= 0 || shift3 >= 0) {
			direction = 5;
		} else {
			direction = 1;
		}
	}

//	if(shift1==0){
//		direction = 5;
//	}else if(shift1==-MAXDELTA1){
//		direction = 1;
//	}else{
//		direction = 1010101;
//	}
	return direction;
}

int32_t get_shift(float *carray) {
	float32_t cmax = 0;
	int32_t shift = 0;
	for (int32_t i = 0 ; i < CORRELATIONSIZE ; i++) {
		if (carray[i] > cmax) {
			shift = i - MICSAMPLESIZE;// - 1;
			cmax = carray[i];
			carray[i] = 0;
		}
	}
	return shift;
}

void rotate_to_led(int8_t led) {
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

void run_to_direction(int8_t direction) {
	rotate_to_led(direction);
	move_straight();
}

void rotate_to_angle(int16_t angle) {
//	right_motor_set_speed(STOP);
//	left_motor_set_speed(STOP);
    //right_motor_set_pos(COUNTER_INIT);

	int32_t steps = 0;
    steps = angle_to_step(angle);
//	chprintf((BaseSequentialStream *)&SDU1, "steps is: %d\n", steps);

    if (steps != 0) { 		//if = 0, only needs to go straight

    	if (angle > LED5ANGLE) {

			//turn anticlockwise
    		steps = ONETURNROBOT - steps;// - HALFTURNROBOT; // or halfturnrobot - steps

			while (right_motor_get_pos() <= steps) {

				right_motor_set_speed(TURNSPEED);
				left_motor_set_speed(-TURNSPEED);

				int32_t position = right_motor_get_pos();

//				chprintf((BaseSequentialStream *)&SDU1, "position is: %d\n", position);
			} //while (right_motor_get_pos() < steps);// {}

		} else {
			//turn clockwise
			while (left_motor_get_pos() <= steps) {

				right_motor_set_speed(-TURNSPEED);
				left_motor_set_speed(TURNSPEED);

				//chprintf((BaseSequentialStream *)&SDU1, "turning cc: %d\n", angle);
			} //while (left_motor_get_pos() < steps);// {}
		}
    }

	right_motor_set_pos(STOP);
	left_motor_set_pos(STOP);

	int32_t position2 = left_motor_get_pos();

//	chprintf((BaseSequentialStream *)&SDU1, "position2 is: %d\n", position2);

//	right_motor_set_speed(STOP);
//	left_motor_set_speed(STOP);
}


int32_t angle_to_step(int16_t angle) {
    return (STEPPERDEGREE*angle); //constrainAngle(angle);
}

int distance_to_step(int distance) { //not sure this function is useful for me
    return (STEPPERCM*distance);
}

void move_straight(void) {
	int32_t cm = 0;
	// move straight
    do {
    	right_motor_set_speed(TURNSPEED);
    	left_motor_set_speed(TURNSPEED);
    	cm++;
    } while (cm < XCMSTEP);

	right_motor_set_speed(STOP);
	left_motor_set_speed(STOP);
}

