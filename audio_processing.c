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


//semaphore
static BSEMAPHORE_DECL(sendToComputer_sem, TRUE);

static float micLinput[MICSAMPLESIZE];
static float micRinput[MICSAMPLESIZE];
static float micFinput[MICSAMPLESIZE];
static float micBinput[MICSAMPLESIZE];
static int8_t dance_flag = 0;


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

	clear_leds();

	/*
	*	We get 160 samples per mic every 10ms
	*	So we fill the samples buffers to reach
	*	1024 samples, then we compute the FFTs. //can change to MICSAMPLESIZE bc idc
	*/

	static uint16_t total_samples = 0;
	#ifdef TESTING
		static uint16_t mustSend = 0;
	#endif //TESTING
	static uint16_t sample_number = 0;
//	static int32_t sumavgmicR = 0;
//	int32_t Rstreamavg = 0;


	static float32_t micR_rms_value = 0;			//demander pour static declaration inside function
	float32_t current_micR_rms = 0;

	static uint8_t rms_above_event = 0;

	static int32_t direction = 0;

	//static int32_t micR_rms_value_sum = 0;
	//static int32_t micR_rms_avg = 0;
	//static int32_t rms_avg_buffer[16000/MICSAMPLESIZE]; //16000: data pts for 1 sec
	//static int8_t buffer_pt = 0;
	int16_t count = 0;

	int16_t percentage_above_loud = 0; //tells % of data points above a certain predefined level

	//loop to fill the buffers
	for(uint16_t i = 0 ; i < num_samples ; i+=4) {
		//construct an array of numbers from PCM data (can give negative values!)
		micRinput[total_samples] = (float)data[i + MIC_RIGHT];
		micLinput[total_samples] = (float)data[i + MIC_LEFT];
		micBinput[total_samples] = (float)data[i + MIC_BACK];
		//micFinput[total_samples] = (float)data[i + MIC_FRONT]; //dont think it's necessary bc we dont want 3D location (only 2d)


		if (micRinput[total_samples] > LOUD) { //LOUD or EVENT??
			count++;
		}

		total_samples++;

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
//			Rstreamavg = get_micro_average(micRinput, MICSAMPLESIZE);
//			sumavgmicR += Rstreamavg;
			total_samples = 0; 	//to refill buffer later
			current_micR_rms = 0;
			//micR_rms_value = 0;
			break;
		}
	}

	percentage_above_loud = 100*count/MICSAMPLESIZE;

	/*****************************NOTE:*************************/
/*	 might be good to use a circular buffer to put the rms values of the
	 * samples in it that way we can check 100ms by 100ms
	 * idk how to implement a circular buffer
	 * this like might be useful:
	 * https://stackoverflow.com/questions/827691/how-do-you-implement-a-circular-buffer-in-c	*/
	/************************************************************/

	//uint8_t call = 0;

	arm_rms_f32(micRinput, MICSAMPLESIZE, &current_micR_rms);

	//if (micR_rms_value > EVENT) {
	//calculate the current RMS value, and if it is larger than the last one, get the "new direction"

//
//	if (sample_number < 9) {				//count how many rms values are above the event threshold in 1s
//		if (current_mic_rms > EVENT) {
//			rms_above_event++;
//			if (rms_above_event > 6) {
//				palSetPad(GPIOB, GPIOB_LED_BODY);//dance mode!!
//			}
//
//		}
//	} else if (sample_number == 9) {
//		rms_above_event = 0;
//		palClearPad(GPIOB, GPIOB_LED_BODY);
//	}


	if (current_micR_rms > EVENT) {// && sample_number < 10) {
//		chprintf((BaseSequentialStream *)&SDU1, "current_micR_rms is: %f\n", current_micR_rms);

		rms_above_event++;

		if (current_micR_rms > micR_rms_value) {
			micR_rms_value = current_micR_rms;
			//calculate direction of sound -- will only follow it if later finds out it was only a call and not music playing
			static float32_t correlation[CORRELATIONSIZE] = {0}; //careful, after computing direction, reinitialise to 0!!!

			arm_correlate_f32(micRinput, MICSAMPLESIZE, micLinput, MICSAMPLESIZE, correlation); //<- causes problems idk how to fix
			/*stopped giving an error when i changed MICSAMPLESIZE to 24 (low value), still gave error w/ 160*/

			int32_t shiftRL = 0;
			float maxRL = 0;
			for (int32_t i = 0 ; i < CORRELATIONSIZE ; i++) {
				if (correlation[i] > maxRL) { //R-L --> L lags R by shiftRL??
					shiftRL = i - MICSAMPLESIZE - 1;//(int)CORRELATIONSIZE/2;//- MICSAMPLESIZE/2;
					maxRL = correlation[i];
					correlation[i] = 0;
				}
			}

			//chprintf((BaseSequentialStream *)&SDU1, "shiftRL is: %d\n", shiftRL);

			arm_correlate_f32(micLinput, MICSAMPLESIZE, micBinput, MICSAMPLESIZE, correlation);
			int32_t shiftLB = 0;
			float maxLB = 0;
			for (int32_t i = 0 ; i < CORRELATIONSIZE ; i++) {
				if (correlation[i] > maxLB) { //L-B
					shiftLB = i - MICSAMPLESIZE - 1;//(int)CORRELATIONSIZE/2; //- MICSAMPLESIZE/2;
					maxLB = correlation[i];
					correlation[i] = 0;
				}
			}

			//chprintf((BaseSequentialStream *)&SDU1, "shiftLB is: %d\n", shiftLB);

			arm_correlate_f32(micRinput, MICSAMPLESIZE, micBinput, MICSAMPLESIZE, correlation);
			int32_t shiftRB = 0;
			float maxRB = 0;
			for (int32_t i = 0 ; i < CORRELATIONSIZE ; i++) {
				if (correlation[i] > maxRB) { //L-B
					shiftRB = i - MICSAMPLESIZE - 1;//(int)CORRELATIONSIZE/2; //- MICSAMPLESIZE/2;
					maxRB = correlation[i];
					correlation[i] = 0;
				}
			}

			//chprintf((BaseSequentialStream *)&SDU1, "shiftRB is: %d\n", shiftRB);

			direction = get_direction(shiftRL, shiftLB, shiftRB);
			//chprintf((BaseSequentialStream *)&SDU1, "direction is: %d\n", direction);
			//chThdSleepMilliseconds(2000);

		}
	}
//		else if (current_micR_rms <= EVENT) {
//		dance_flag = 0;
//	}

	if (sample_number == 10) {
		sample_number = 0;
		micR_rms_value = 0;
		//if music:
		if (rms_above_event > 5 && percentage_above_loud > 0.5) { //!!! use DEFINE for the 10 (in case MICSAMPLESIZE is changed
	//		chprintf((BaseSequentialStream *)&SDU1, "rms# is: %d\n", rms_above_event);
	//		chprintf((BaseSequentialStream *)&SDU1, "micR_rms is: %f\n", current_micR_rms);

	//		//dance mode
	//		for (uint8_t i = 0; i < 8; i++) {
	//			dancing_puck();
	//		}
			dance_flag = 1; //yes dancing
			//palSetPad(GPIOB, GPIOB_LED_BODY);
			rms_above_event = 0;
			count = 0;
			//chprintf((BaseSequentialStream *)&SDU1, "cond 1\n");
			//if call or whistle
		} else if (rms_above_event > 0) {
			//chprintf((BaseSequentialStream *)&SDU1, "cond 2\n");
			//follow_direction();
			//palClearPad(GPIOB, GPIOB_LED_BODY);
			int32_t direction1 = direction - 1;
			//LedSet_ALL(direction1, 1);
			//chprintf((BaseSequentialStream *)&SDU1, "direction is: %d\n", direction1);
			dance_flag = 0; //no dancing
			rms_above_event = 0;
			count = 0;
			//chThdSleepMilliseconds(2000);
		} else {
			//chprintf((BaseSequentialStream *)&SDU1, "cond 3\n");
			dance_flag = 0; //no dancing
			rms_above_event = 0;
			count = 0;
		}

		chprintf((BaseSequentialStream *)&SDU1, "flag is%d\n", dance_flag);
		//dancing_puck(dance_flag);
	}

//}

///************************* here we differentiate between call & music *********************************/
//
//	if (5 < percentage_above_loud && percentage_above_loud < 20) {//more than 5% and less than 20% of the values are "event loud"
//	//if (micR_rms_avg < LONGEVENT) { //either there was a clap/snap or it's background noise
//	//idk if the RMS value would actually be below LONGEVENT so commenting for now
//		call = check_for_call(micRinput, MICSAMPLESIZE, 0); //this checks if there was a clap/finger snap (bc values go up to +/- 1000
//		if (call) {
//			palSetPad(GPIOB, GPIOB_LED_BODY); //follow_direction();
//			call = 0;
//		} else if (!call) {
//			palClearPad(GPIOB, GPIOB_LED_BODY); //do nothing
//		}
//	//}
//	}
//
//		else if (100*count/MICSAMPLESIZE < 20 || micR_rms_avg >= LONGEVENT) { // either there is a whistle or music playing
//		//we put rms values in a buffer to check??
//			if (buffer_pt < 10) { //fill like a circular buffer
//				rms_avg_buffer[buffer_pt] = (int)micR_rms_value;
//				buffer_pt++;
//				float32_t micRinput_copy[MICSAMPLESIZE];
//				arm_copy_f32(micRinput, micRinput_copy, MICSAMPLESIZE);
//				if (buffer_pt == 10) {
//					buffer_pt = 0;
//				}
//			}
//		}


//	if (sample_number == 6 && micR_rms_value < 0.3 * micR_rms_avg && micR_rms_avg >= WHISTLE) { //this is the 6th sample --> @ 600 ms
//		//we can conclude there was a whistle
//		palSetPad(GPIOD, GPIOD_LED7);
//		sample_number = 0;
//		call = 1;
//	} else {
//		micR_rms_value_sum += (int)micR_rms_value; //type conversion to not sum floating point data
//		micR_rms_avg = micR_rms_value_sum/sample_number;
//		palClearPad(GPIOD, GPIOD_LED7);
//	}
//
//
//	if (sample_number == 10 && micR_rms_avg >= NOMUSIC) {
//		palSetPad(GPIOB, GPIOB_LED_BODY);
//		sample_number = 0;
//		call = 0;
//	} else if (sample_number == 10 && micR_rms_avg < NOMUSIC) {
//		palClearPad(GPIOB, GPIOB_LED_BODY);
//	}


	//check for call function here?? problem:
	//if music starts playing he'll think he's getting called before realising it's music?
	//added (sumavgmicR/threesec) < NOMUSIC condition but im not sure it's properly functional
	//uint8_t call = 0;
	// call = check_for_call(micRinput, MICSAMPLESIZE, Rstreamavg);

//	if (call) { // && ((sumavgmicR/threesec) < NOMUSIC)) {
//		palSetPad(GPIOB, GPIOB_LED_BODY); //follow_direction();
//		call = 0;
//	} else if (!call) { palClearPad(GPIOB, GPIOB_LED_BODY); }
//
//
//	if (sample_number == 8 ) {  //once we have X sec worth of data, we check if the noise avg is above a certain level
//							 //in the case that it is, we know that music is playing (or ppl are being too loud?)
////		int32_t avgmicR = 0;
////		avgmicR = sumavgmicR/300; //avg of 3 sec worth of data
////		sumavgmicR = 0;
//		sample_number = 0;
//		total_samples = 0;
//		if (/*avgmicR*/ micR_rms_value > NOMUSIC) { //instead of computing other avgs/rms value?? if music is playing, just start dancing
//			dancing_puck();
//		}
////			palClearPad(GPIOD, GPIOD_LED_FRONT);
////		} else if (avgmicR <= NOMUSIC) {
////			palSetPad(GPIOD, GPIOD_LED_FRONT);
////		}
//	}
			/*
			 * here we do the data processing: calculate the avg (of the noise),
			 * set an event threshold
			 * the problem: what is this avg actually giving?
			 * should we do the avg of the PCM data or of the volume?
			 * can't get the volume of the different microphones!
			 */

//		//instead of computing other avgs if music is playing, just start dancing
//		if (abs(avgmicR) > NOMUSIC && mustSend == 299) {
////			chprintf((BaseSequentialStream *)&SDU1, "avgR is: %d\n", avgmicR);
////			chprintf((BaseSequentialStream *)&SDU1, "avgR > NOMUSIC");
//			palSetPad(GPIOD, GPIOD_LED_FRONT); //dance_moves();
//		    //palClearPad(GPIOB, GPIOB_LED_BODY);
//			total_samples = 0;
//			mustSend = 0;
//			sumavgmicR = 0;
////				if(mustSend > 8) {
////					//signals to send the result to the computer
////					chBSemSignal(&sendToComputer_sem);
////					mustSend = 0;
////				}
//		}
//		else if (abs(avgmicR) <= NOMUSIC && mustSend == 299) {
//			danseMode(100);
//			avgmicL +=  get_micro_average(micLinput, MICSAMPLESIZE);
////			chprintf((BaseSequentialStream *)&SDU1, "avgL is%d\n", avgmicL);
////		    palSetPad(GPIOB, GPIOB_LED_BODY);
//		    palClearPad(GPIOD, GPIOD_LED_FRONT);
//			total_samples = 0;
//			mustSend = 0;
//			sumavgmicR = 0;
//			/* i don't think it's necessary to have a threshold for more than 2 microphones: if 2
//			 * already see that there is an event, no need to check for the other 2
//			 * depends if we should - the avg for each microphone?? idk
//			 */
//			//avgmicB = get_micro_average(micBinput, MICSAMPLESIZE);
//			//avgmicF = get_micro_average(micFinput, MICSAMPLESIZE);
//		}
////			float hereboyR, hereboyL, hereboyB, hereboyF = 0;
////			hereboyR = (float)avgmicR * 1.20;
////			hereboyL = (float)avgmicL * 1.20;
////			//hereboyB = (float)avgmicB * 1.20;
////			//hereboyF = (float)avgmicF * 1.20;
////			float32_t correlationRL[CORRELATIONSAMPLESIZE];
////			arm_correlate_f32(micRinput, MICSAMPLESIZE, micLinput, MICSAMPLESIZE, correlationRL);
////			int16_t shiftRL, maxRL = 0;
////			for (uint32_t i = 0 ; i < CORRELATIONSAMPLESIZE ; i++) {
////				if (correlationRL[i] > maxRL) {
////					shiftRL = i - MICSAMPLESIZE/2;
////				}
////			}

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

int32_t get_direction(int32_t shift1, int32_t shift2, int32_t shift3) {
	int32_t direction = 0;
	//shift1: RL //shift2: LB //shift3: RB
	//int8_t direction1 = 0;
	//int8_t direction2 = 0;

	if (shift1 >= MAXDELTA1) {
		direction = 3;
	} else if (shift1 <= -MAXDELTA1) {
		direction = 7;
	} else if (shift1 < 0) {
		//direction1 = 8;
		if (shift2 > 0) {
			direction = 8;
		} else {
			direction = 6;
		}
	} else if (shift1 < 0) {
		//direction1 = 2;
		if (shift3 > 0) {
			direction = 2;
		} else {
			direction = 4;
		}
	} else if (shift1 == 0) {
		if (shift2 > 0 || shift3 > 0) {
			direction = 5;
		} else {
			direction = 1;
		}
	}

//	if (shift2 >= MAXDELTA2) {	//B lags L
//		direction2 = 5;
//	} else if (shift2 <= -MAXDELTA2) {
//		direction2 = 1;
//	} else if (shiftLB < 0) {
//		direction2 = 6;
//	} else if (shiftLB > 0) {
//		direction2 = 4;
//	}
//
//	if (direction1 == 2) {
//		if (direction2 == 0) {
//			direction = 4;
//		} else {
//		direction = 4;
//		}
//	}
//
//
//	for (uint8_t i = 0; i < 9; i++) {
//		if (angle2 > asin_lookup[0][i]) {
//			angle2 = asin_lookup[1][i];
//			if(i > 5)  				// if the second angle is bigger than +30deg
//				angle = PI - angle1;   	// the direction = 90-angle1
//			else angle = angle1;
//			}
//		}
//	}

//
//		// We want an angle strictly between [0,2*PI]
//		if (angle < 0)
//			angle = 2*PI + angle;
	return direction;
}

