#ifndef AUDIO_PROCESSING_H
#define AUDIO_PROCESSING_H


#define FFT_SIZE 		1024
#define MICSAMPLESIZE	1600 //320 //24 //1600 //1024 //160
#define CORRELATIONSIZE (2*MICSAMPLESIZE-1)
#define NOMUSIC			30
#define WHISTLE			250
#define CLAP			800
#define LONGEVENT		140
#define EVENT			140//140
#define LOUD			150//250
//#define PI				3.14
//#define WHEELPERIMETER	12.9 //in cm
//#define CIRCLE			360
//#define ONETURNSTEP		1000 //20 steps/rev for the motor, 50:1 reduction gear: 20*50 steps for a complete wheel turn
//#define ONETURNROBOT	1290 //the two wheels are 53mm apart so one full rotation is 53*pi, which is 166.5mm.
//							 //Divided by the wheel perimeter, times the number of step in one revolution //not sure about the value
//							 //pi*5.3*(1/12.9)*1000
//#define STEPPERDEGREE	3.58 // 3.58  = (ONETURNROBOT/CIRCLE)
//#define STEPPERCM		77.5 // 77.5 = (ONETURNSTEP/WHEELPERIMETER)
//#define XCMSTEP			(STEPPERCM*1000)
//#define HALFTURNROBOT	646
//#define TURNSPEED		500
//#define LED1ANGLE		0
//#define LED2ANGLE		45
//#define LED3ANGLE		90
//#define LED4ANGLE		135
//#define LED5ANGLE		180
//#define LED6ANGLE		225
//#define LED7ANGLE		270
//#define LED8ANGLE		315
//#define RIGHTTURNLIMIT	(LED5ANGLE*STEPPERDEGREE)
//#define STOP			0 //stop speed (stop moving motors)


//max # of samples that can exist between 2 signals
#define MAXDELTA1	2//3//6//3 //2.8		//sampling frequency[Hz]*dist between micros[cm] / speed of sound [cm/s]
//6cm   we approximate w/ an integer? maybe better
#define MAXDELTA2	5//2 //2.099 	//16k 					*?						 /34300
//4.5cm

typedef enum { //this is to try and get PCM data
	//2 times FFT_SIZE because these arrays contain complex numbers (real + imaginary)
	MIC_R_INPUT = 0,
	MIC_L_INPUT = 0,
	MIC_F_INPUT = 0,
	MIC_B_INPUT = 0
//	LEFT_CMPLX_INPUT = 0,
//	RIGHT_CMPLX_INPUT,
//	FRONT_CMPLX_INPUT,
//	BACK_CMPLX_INPUT,
	//Arrays containing the computed magnitude of the complex numbers
//	LEFT_OUTPUT,
//	RIGHT_OUTPUT,
//	FRONT_OUTPUT,
//	BACK_OUTPUT
} BUFFER_NAME_t;

//
//static const float asin_lookup[2][9] = {
//		{-1, -0.87 /*-sqrt3/2*/, -0.71 /*-sqrt2/2*/, -0.5, 0, 0.5, 0.71, -0.87, 1},
//		{-PI/2, -PI/3, -PI/4, -PI/6, 0, PI/6, PI/4, PI/3, PI/2}
//};


void processAudioData(int16_t *data, uint16_t num_samples);

/*
*	put the invoking thread into sleep until it can process the audio datas
*/
void wait_send_to_computer(void);

int32_t get_micro_average(float *micro_ID, uint16_t sample_size);

uint8_t check_for_call(float *data, uint16_t num_samples, int32_t stream_avg);

void follow_direction(void);

void get_micro_RMS(float *micro_ID, uint16_t sample_size, uint32_t micro_rms);

uint8_t get_direction(int32_t shift1, int32_t shift2, int32_t shift3);

void turn_led(float angle);

int32_t get_shift(float *carray);

void set_allowed_to_move(uint8_t allowed);

/*** to put in a different file***/

//void run_to_direction(uint8_t direction);
//void rotate_to_angle(int angle);
////int distance_to_step(int distance);
//int angle_to_step(int angle);
//void move_straight(void);
//void stay_put(void);

//for control
//void set_robot_moves(uint8_t new_mode);
//uint8_t get_robot_moves (void);


/*
*	Returns the pointer to the BUFFER_NAME_t buffer asked
*/

float* get_audio_buffer_ptr(BUFFER_NAME_t name);

#endif /* AUDIO_PROCESSING_H */
