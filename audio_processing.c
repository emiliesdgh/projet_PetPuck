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


//semaphore
static BSEMAPHORE_DECL(sendToComputer_sem, TRUE);

static float micLinput[MICSAMPLESIZE];
static float micRinput[MICSAMPLESIZE];
static float micFinput[MICSAMPLESIZE];
static float micBinput[MICSAMPLESIZE];


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

void processAudioData(int16_t *data, uint16_t num_samples){

	/*
	*	We get 160 samples per mic every 10ms
	*	So we fill the samples buffers to reach
	*	1024 samples, then we compute the FFTs. //can change to MICSAMPLESIZE bc idc
	*/

	static uint16_t nb_samples = 0;
	static uint16_t mustSend = 0;
	static uint16_t threesec = 0;
	static int32_t sumavgmicR = 0;

	//loop to fill the buffers
	for(uint16_t i = 0 ; i < num_samples ; i+=4) {
		//construct an array of numbers from PCM data (can give negative values!)
		micRinput[nb_samples] = (float)data[i + MIC_RIGHT];
		micLinput[nb_samples] = (float)data[i + MIC_LEFT];
		micBinput[nb_samples] = (float)data[i + MIC_BACK];
		micFinput[nb_samples] = (float)data[i + MIC_FRONT];

		nb_samples++;

	#ifdef TESTING
		mustSend++;
		if (mustSend > 10) {
			//signals to send the result to the computer
			chBSemSignal(&sendToComputer_sem);
			mustSend=0;
		}
	#endif //TESTING

		//stop when buffer is full
		if(nb_samples >= (MICSAMPLESIZE)) {
			int32_t Rstreamavg = get_micro_average(micRinput, MICSAMPLESIZE);
			sumavgmicR += Rstreamavg;
			threesec++;
			//check for call function here?? problem:
			//if music starts playing he'll think he's getting called before realising it's music?
			//added (sumavgmicR/threesec) < NOMUSIC condition but im not sure it's properly functional
			uint8_t call = 0;
			call = check_for_call(micRinput, MICSAMPLESIZE, Rstreamavg);
			nb_samples = 0; 	//to refill buffer later
			if (call && ((sumavgmicR/threesec) < NOMUSIC)) {
				palSetPad(GPIOB, GPIOB_LED_BODY); //follow_direction();
			} else { palClearPad(GPIOB, GPIOB_LED_BODY); }
			break;
		}
	}

	if (threesec == 299 ) {  //once we have 3 sec worth of data, we check if the noise avg is above a certain level
							 //in the case that it is, we know that music is playing (or ppl are being too loud?)
		int32_t avgmicR = 0;
		avgmicR = sumavgmicR/300; //avg of 3 sec worth of data
		sumavgmicR = 0;
		threesec = 0;
		nb_samples = 0;
		if (avgmicR > NOMUSIC) { //instead of computing other avgs if music is playing, just start dancing
			danseMode(100);
		}
//			palClearPad(GPIOD, GPIOD_LED_FRONT);
//		} else if (avgmicR <= NOMUSIC) {
//			palSetPad(GPIOD, GPIOD_LED_FRONT);
//		}
	}
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
//			nb_samples = 0;
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
//			nb_samples = 0;
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
	int32_t signalR_max = stream_avg + 100;
	int32_t signalR_min = stream_avg - 100;
	//float signalR_min = stream_avg * 0.10;
		for (uint32_t i = 0; i < num_samples; i++) {
			hereboy = !((data[i] < signalR_max) && (data[i]>signalR_min));
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
	else{
		return NULL;
	}
}


