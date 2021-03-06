#include <audio_processing.h>

static float micLinput[MICSAMPLESIZE];
static float micRinput[MICSAMPLESIZE];
static float micBinput[MICSAMPLESIZE];
static uint8_t allowed_to_move = 1;
static uint16_t sample_number;
static uint8_t puck_playing_sound = 0;

/*
*	Callback called when the demodulation of the four microphones is done.
*	We get 160 samples per mic every 10ms (16kHz)
*
*	params :
*	int16_t *data			Buffer containing 4 times 160 samples. the samples are sorted by micro
*							so we have [micRight1, micLeft1, micBack1, micFront1, micRight2, etc...]
*	uint16_t num_samples	Tells how many data we get in total (should always be 640)
*	*/

void processAudioData(int16_t *data, uint16_t num_samples) {
	if(!get_position_reached() || !allowed_to_move || get_led_flag_uhOh()==1 || puck_playing_sound) {
			return;
	}

	static uint16_t total_samples = 0;
	static float32_t micR_rms_value = 0;
	static uint16_t rms_above_event = 0;
	static uint8_t direction = 0;
	static int16_t count = 0;
	static float32_t correlation[CORRELATIONSIZE] = {0};

	float32_t current_micR_rms = 0;
	int32_t percentage_above_loud = 0;

	//loop to fill the buffers
	for(uint16_t i = 0 ; i < num_samples ; i+=4) {
		//construct an array of numbers from PCM data (can give negative values!)
		micRinput[total_samples] = (float)data[i + MIC_RIGHT];
		micLinput[total_samples] = (float)data[i + MIC_LEFT];
		micBinput[total_samples] = (float)data[i + MIC_BACK];

		if (micRinput[total_samples] > LOUD) {
			count++;
		}

		percentage_above_loud = 100*count/MICSAMPLESIZE;

		total_samples++;

		//stop when buffer is full
		if(total_samples >= (MICSAMPLESIZE)) {
			sample_number++;
			if (sample_number > ONESEC) { 				//restart @ 0 after 1 sec
				sample_number = 0;
			}
			total_samples = 0; 							//to refill buffer later
			current_micR_rms = 0;
			break;
		}
	}

	arm_rms_f32(micRinput, MICSAMPLESIZE, &current_micR_rms);

	if (current_micR_rms > EVENT && get_sample_number() < 10) {
		rms_above_event = rms_above_event + 1;

		if (current_micR_rms > micR_rms_value) {
			micR_rms_value = current_micR_rms;

			//calculate direction of sound -- will only follow it if later finds out it was only a call and not music playing
			arm_correlate_f32(micRinput, MICSAMPLESIZE, micLinput, MICSAMPLESIZE, correlation);
			int32_t shiftRL = 0;
			shiftRL = get_shift(correlation);

			arm_correlate_f32(micLinput, MICSAMPLESIZE, micBinput, MICSAMPLESIZE, correlation);
			int32_t shiftLB = 0;
			shiftLB = get_shift(correlation);

			arm_correlate_f32(micRinput, MICSAMPLESIZE, micBinput, MICSAMPLESIZE, correlation);
			int32_t shiftRB = 0;
			shiftRB = get_shift(correlation);

			direction = get_direction(shiftRL, shiftLB, shiftRB);
		}
	}

	if (sample_number == ONESEC && allowed_to_move) {
		allowed_to_move = 0;

		//if music:
		if (rms_above_event > MUSICRMS && percentage_above_loud > MUSICPERCENTAGE) {
			set_robot_moves(DANCE);
		//if called:
		} else if (rms_above_event > 0) {
			set_direction_to_follow(direction);
			set_position_reached(0);
			set_robot_moves(HEREBOY);
		//no audible event
		} else {
			set_robot_moves(MIC);
		}

		direction = 0;
		rms_above_event = 0;
		micR_rms_value = 0;
		count = 0;
	}
}


uint8_t get_direction(int32_t shift1, int32_t shift2, int32_t shift3) {
	uint8_t direction = 0;
	//shift1: RL //shift2: LB //shift3: RB

	if (shift1 > MAXDELTA1) { 				//R lags L
		direction = 7;
	} else if (shift1 < -MAXDELTA1) { 		//L lags R
		direction = 3;
	} else if (shift1 > 0) { 				//gets to L first
		if (shift2 > 0) { 					//L lags B
			direction = 6;
		} else {
			direction = 8;
		}
	} else if (shift1 < 0) { 				//gets to R first
		if (shift3 > 0) {	 				//R lags B
			direction = 4;
		} else {
			direction = 2;
		}
	} else {
		if (shift2 < 0 || shift3 < 0) { 	//B lags R or L
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
			shift = i - MICSAMPLESIZE;
			cmax = carray[i];
		}
		carray[i] = 0;
	}
	return shift;
}

void set_allowed_to_move(uint8_t allowed) {
	allowed_to_move = allowed;
}

uint8_t get_allowed_to_move(void) {
	return allowed_to_move;
}

void set_sample_number(uint8_t snumber) {
	sample_number = snumber;
}

uint8_t get_sample_number(void) {
	return sample_number;
}

void set_puck_playing_sound(uint8_t playing) {
	puck_playing_sound = playing;
}

uint8_t get_puck_playing_sound(void) {
	return puck_playing_sound;
}
