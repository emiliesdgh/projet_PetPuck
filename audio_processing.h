#ifndef AUDIO_PROCESSING_H
#define AUDIO_PROCESSING_H


#define FFT_SIZE 	1024
#define MICSAMPLESIZE	160
#define CORRELATIONSAMPLESIZE 	(2*MICSAMPLESIZE-1)
#define NOMUSIC		15

/*typedef enum {
	//2 times FFT_SIZE because these arrays contain complex numbers (real + imaginary)
	LEFT_CMPLX_INPUT = 0,
	RIGHT_CMPLX_INPUT,
	FRONT_CMPLX_INPUT,
	BACK_CMPLX_INPUT,
	//Arrays containing the computed magnitude of the complex numbers
	LEFT_OUTPUT,
	RIGHT_OUTPUT,
	FRONT_OUTPUT,
	BACK_OUTPUT
} BUFFER_NAME_t; */

void processAudioData2(int16_t *data, uint16_t num_samples);

void processAudioData(int16_t *data, uint16_t num_samples);

/*
*	put the invoking thread into sleep until it can process the audio datas
*/
void wait_send_to_computer(void);

int32_t get_micro_average(float *micro_ID, uint16_t sample_size);

uint8_t check_for_call(float *data, uint16_t num_samples, int32_t stream_avg);

/*
*	Returns the pointer to the BUFFER_NAME_t buffer asked
*/

// float* get_audio_buffer_ptr(BUFFER_NAME_t name);

#endif /* AUDIO_PROCESSING_H */
