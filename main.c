//include the file .h for the main
#include <main.h>


static void serial_start(void)
{
	static SerialConfig ser_cfg = {
	    115200,
	    0,
	    0,
	    0,
	};

	sdStart(&SD3, &ser_cfg); // UART3.
}

int main(void)
{
    halInit();
    chSysInit();
    mpu_init();

    //start the serial communication
    serial_start();
    //start the USB communication
    usb_start();
    //start the camera
	po8030_start();
	dcmi_start();
    //start the audio
    dac_start();
    //start the RGB LEDs
	spi_comm_start();
	//start proximity thread
	initial_proximity();
	//start motors
	motors_init();
    //inits the I2C communication
    i2c_start();
	imu_start();
	//start the image processing
	po8030_advanced_config(FORMAT_RGB565, 0, 10, IMAGE_BUFFER_SIZE, 2, SUBSAMPLING_X1, SUBSAMPLING_X1);
	dcmi_set_capture_mode(CAPTURE_ONE_SHOT);
	dcmi_prepare();
	process_image_start();
	//start the GM/GN
	polite_puck_start();
	//start the audio processing
	mic_start(&processAudioData);
	Control_start();
	//start obstacle encounter
	playSoundFileStart();
	ObstacleEncounter_start();
	//start panic
	calibrate_gyro();
	calibrate_acc();
	PanicMode_start();

    while (1) {
		//waits 1 second
		chThdSleepMilliseconds(1000);
    }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
