#include <puck_movement.h>

static uint8_t led_flag_uhOh = 0;
static int8_t reset_direction = 0;
imu_msg_t imu_values;

#define ACC_OFFSET 0.0239
#define NB_ACC_SAMPLES 5

//semaphore
static BSEMAPHORE_DECL(image_ready_sem, TRUE);

enum {
    RED,
    GREEN,
	BLUE,
};

//*********PROXIMITY + DISTANCE TO STOP THREAD***********
messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);

//function to start proximity and obstacle encounter threads
void initial_proximity(void) {
    // TOF sensor
    VL53L0X_start();
    // inits the Inter Process Communication bus.
    messagebus_init( & bus, & bus_lock, & bus_condvar);
    proximity_start();
    calibrate_ir();
    chThdSleepMilliseconds(500);
}

//makes motor move w/ selected speed
void motor_set_danse_speed(float speed_r, float speed_l) {
	 int speed_r_step_s,speed_l_step_s;
	 //transform the speed from cm/s into step/s
	 speed_r_step_s = -speed_r * ONETURNSTEP / WHEELPERIMETER;
	 speed_l_step_s = speed_l * ONETURNSTEP / WHEELPERIMETER;

	 left_motor_set_speed(speed_l_step_s);
	 right_motor_set_speed(speed_r_step_s);
}

void dancing_puck(void) {

	reset_direction = 1;

	for(int16_t dance_counter=1; dance_counter<5; dance_counter++){
		//move one way
		if(dance_counter==1 || dance_counter==3){
			motor_set_danse_speed(5,5);
			palTogglePad(GPIOB, GPIOB_LED_BODY);
		//move the other way
		}else if(dance_counter ==2 || dance_counter ==4){
			motor_set_danse_speed(-5,-5);
			palTogglePad(GPIOB, GPIOB_LED_BODY);
		}
		chThdSleepMilliseconds(300);
	}
}


//*********OBSTACLE ENCOUNTER THREAD***********
//initialization of the obstacle encounter thread
static THD_WORKING_AREA(waObstacleEncounter, 2048);
static THD_FUNCTION(ObstacleEncounter, arg){

	chRegSetThreadName(__FUNCTION__);
	(void)arg;

    systime_t time;
	messagebus_topic_t *imu_topic = messagebus_find_topic_blocking(&bus, "/imu");
    uint16_t distance_mm = 0;

	while(1) {

		while(get_selector()==0) {
			chThdYield();
		}

		time = chVTGetSystemTime();
        //wait for new measures to be published
        messagebus_topic_wait(imu_topic, &imu_values, sizeof(imu_values));
		distance_mm  = VL53L0X_get_dist_mm();
		motors_speed(distance_mm);

		if(led_flag_uhOh == 1) {
			set_puck_playing_sound(1);
			reset_direction = 1;
	    	uint32_t color = get_colors();
			playNote(NOTE_G4, 120);

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
			playNote(NOTE_E4, 120);
			set_puck_playing_sound(0);
		} else {

		}
		//100Hz freq
		chThdSleepUntilWindowed(time, time + MS2ST(10)); //- > mettre dans chaque thread et le 10 c'est la periode
	}
}

/*function to start the obstacle encounter thread */
void ObstacleEncounter_start(void) {
	chThdCreateStatic(waObstacleEncounter, sizeof(waObstacleEncounter), NORMALPRIO+1, ObstacleEncounter, NULL);
}

/*function that checks the distance between the robot and a possible obstacle*/
void motors_speed(uint16_t distance){
	if(distance > DISTANCE_MIN){
		clear_leds();
		led_flag_uhOh = 0;
	} else {
		set_direction_to_follow(0);
		led_flag_uhOh += 1;
	}
}

int get_led_flag_uhOh(void) {
	return led_flag_uhOh;
}

int8_t get_reset_direction(void) {
	return reset_direction;
}

void clear_reset_direction(void) {
	 reset_direction = 0;
}

uint32_t get_colors(void) {

	get_dcmi_capture();

	uint8_t *img_buff_ptr = 0;
   	uint32_t redred 	= 0;
   	uint32_t greengreen = 0;
   	uint32_t blueblue = 0;

	// Gets the pointer to the array filled with the last image in RGB565
	img_buff_ptr = dcmi_get_last_image_ptr();

	// Extracts and adds all pixels values of one line, by color (format RGB565)
	for(uint16_t i = 0 ; i < (2 * IMAGE_BUFFER_SIZE) ; i+=2) {	// pixels acquired on 2 bytes
		redred += (img_buff_ptr[i] & 0xF8) >> 2;
		greengreen += ((img_buff_ptr[i] & 0x07) << 3) + ((img_buff_ptr[i+1] & 0xE0) >> 5);
		blueblue += (img_buff_ptr[i+1] & 0x1F) << 1;
	}

	if (redred >= greengreen && redred >= blueblue){
		return RED;
	} else if(greengreen >= blueblue){
		return GREEN;
	} else {
		return BLUE;
	}
}

//********************************************
static THD_WORKING_AREA(waPanicMode, 1024);
static THD_FUNCTION(PanicMode, arg){

	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	systime_t time;
	messagebus_topic_t *imu_topic = messagebus_find_topic_blocking(&bus, "/imu");
	int8_t led_flag_panic = 0;

	while(1){

		while(get_selector()==0) {
			chThdYield();
		}

		time = chVTGetSystemTime();
        //wait for new measures to be published
        messagebus_topic_wait(imu_topic, &imu_values, sizeof(imu_values));

        led_flag_panic = get_inclination(&imu_values);

        if(led_flag_panic == 1){

        	set_puck_playing_sound(1);
        	reset_direction = 1;
        	motor_set_danse_speed(0,0);
        	dac_play(NOTE_CS3);
        	PanicMode_LED();

        	dac_stop();
            set_puck_playing_sound(0);
        }
        //100Hz
        chThdSleepUntilWindowed(time, time + MS2ST(10));
	}
}

void PanicMode_start(void) {
	chThdCreateStatic(waPanicMode, sizeof(waPanicMode), NORMALPRIO+2 , PanicMode, NULL) ;
}


int8_t get_inclination(imu_msg_t *imu_values) {
	//create a pointer to the array for shorter name
	float *accel = imu_values->acceleration;

	int8_t no_panic = 0;
	int8_t panic = 1;

	if(fabs(accel[X_AXIS]) > THRESHOLD_ACC_X || fabs(accel[Y_AXIS]) > THRESHOLD_ACC_Y){
		return panic;

	}else{
		return no_panic;
	}
}

