#ifndef AUDIO_PROCESSING_H
#define AUDIO_PROCESSING_H

#define MICSAMPLESIZE	1600
#define CORRELATIONSIZE (2*MICSAMPLESIZE-1)
#define EVENT			200
#define LOUD			250
#define ONESEC			9  //value of sample# @ 1 sec
#define MUSICRMS		4  //rms above EVENT signaling music
#define MUSICPERCENTAGE 35

//max # of samples that can exist between left and right mics
#define MAXDELTA1	2	   //sampling frequency[Hz]*dist between micros[cm] / speed of sound [cm/s]
						   //16k 				   *6						/34300

//max # of samples that can exist between right/left and back mics
//#define MAXDELTA2	2

#include "ch.h"
#include "hal.h"
#include <audio/microphone.h>
#include <arm_math.h>
#include <puck_led.h>
#include <control.h>
#include <puck_movement.h>

void processAudioData(int16_t *data, uint16_t num_samples);
void get_micro_RMS(float *micro_ID, uint16_t sample_size, uint32_t micro_rms);
uint8_t get_direction(int32_t shift1, int32_t shift2, int32_t shift3);
int32_t get_shift(float *carray);
void set_playing_sound(uint8_t allowed);
uint8_t get_playing_sound(void);
void set_allowed_to_move(uint8_t allowed);
uint8_t get_allowed_to_move(void);
void set_allowed_to_run(uint8_t allowed);
uint8_t get_allowed_to_run(void);
void set_sample_number(uint8_t snumber);
uint8_t get_sample_number(void);
void set_puck_playing_sound(uint8_t playing);
uint8_t get_puck_playing_sound(void);

#endif /* AUDIO_PROCESSING_H */
