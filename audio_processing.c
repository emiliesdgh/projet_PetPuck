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
#include <control.h>

//semaphore
static BSEMAPHORE_DECL(sendToComputer_sem, TRUE);

static float micLinput[MICSAMPLESIZE];
static float micRinput[MICSAMPLESIZE];
static float micBinput[MICSAMPLESIZE];
static uint8_t allowed_to_move;

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
//static thread_t *Controlp;

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
	static float32_t micR_rms_value = 0;
	static uint16_t rms_above_event = 0;
	static uint8_t direction = 0;
	static int16_t count = 0;
	float32_t current_micR_rms = 0;
	static float32_t correlation[CORRELATIONSIZE] = {0};
	//careful, after computing direction, reinitialise to 0!!! (done in function)

	//loop to fill the buffers
	for(uint16_t i = 0 ; i < num_samples ; i+=4) {
		//construct an array of numbers from PCM data (can give negative values!)
		micRinput[total_samples] = (float)data[i + MIC_RIGHT];
		micLinput[total_samples] = (float)data[i + MIC_LEFT];
		micBinput[total_samples] = (float)data[i + MIC_BACK];

		if (micRinput[total_samples] > LOUD) { //LOUD or EVENT??
			count++;
		}

		total_samples++;

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

	arm_rms_f32(micRinput, MICSAMPLESIZE, &current_micR_rms);

	if (current_micR_rms > EVENT && sample_number < 10) {
		rms_above_event = rms_above_event + 1;

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

	if (sample_number == 9 && allowed_to_move) {
		set_allowed_to_move(0);
		sample_number = 0;
		//if music:
		if (rms_above_event > 9) { // && percentage_above_loud > 0.4) 		//!!! use DEFINE for the 10 (in case MICSAMPLESIZE is changed
//			sample_number = 0;
			chprintf((BaseSequentialStream *)&SDU1, "cond 1\n");
			//if call or whistle
			//set mode_of_the_robot = MOT
			set_robot_moves(DANCE);
//			set_direction_to_follow(STOP);
			direction = 0;
			//chThdSleepMilliseconds(1000);
			//chThdResume(&Controlp,R_OK);
		} else if (rms_above_event > 0) {
			chprintf((BaseSequentialStream *)&SDU1, "cond 2\n");
			chprintf((BaseSequentialStream *)&SDU1, "direction is: %d\n", direction);
			//set mode_of_the_robot = MOT
//			chMsgSend(Control)
			set_direction_to_follow(direction);
			set_position_reached(0);
			set_robot_moves(HEREBOY);
			//chThdSleepMilliseconds(1000);
			//chThdResume(&Controlp,R_OK);
//			chSysLockFromISR();
//			chThdResumeI(&trp, (msg_t)0x1337);  /* Resuming the thread with message.*/
//			chSysUnlockFromISR();

//			run_to_direction(direction);
			direction = 0;					//reset the direction
//			sample_number = 0;
		} else {
			direction = 0;
			set_robot_moves(MIC);

			chprintf((BaseSequentialStream *)&SDU1, "cond 3 %d \n", chVTGetSystemTime());

//			sample_number = 0;
//			stay_put();
			//set mode_of_the_robot = keep going in MIC
		}
		rms_above_event = 0;
		micR_rms_value = 0;
		count = 0;
	}

}


uint8_t get_direction(int32_t shift1, int32_t shift2, int32_t shift3) {
	uint8_t direction = 0;
	//shift1: RL //shift2: LB //shift3: RB

	if (shift1 > MAXDELTA1) { //R lags L
		direction = 7;
	} else if (shift1 < -MAXDELTA1) { //L lags R
		direction = 3;
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

void set_allowed_to_move(uint8_t allowed) {
	allowed_to_move = allowed;
}


