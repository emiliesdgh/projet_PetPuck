#ifndef PROCESS_IMAGE_H
#define PROCESS_IMAGE_H

extern mutex_t image_buffer_mutex;
void test_lecture_(void);

float get_distance_cm(void);
uint16_t get_line_position(void);
void process_image_start(void);
void ambient_light(uint8_t *buffer1, uint8_t *buffer2, uint8_t *buffer3);

void extract_pixels_R (uint8_t *red_buffer, uint8_t *image_buffer_ptr);
void extract_pixels_G (uint8_t *green_buffer, uint8_t *image_buffer_ptr);
void extract_pixels_B (uint8_t *blue_buffer, uint8_t *image_buffer_ptr);
void extract_pixels_RGB (uint8_t *red_buffer, uint8_t *green_buffer, uint8_t *blue_buffer, uint8_t **image_buffer_ptr);
void get_ptr_to_img_array(uint8_t *image_buffer_ptr);
void get_dcmi_capture (void);
void polite_puck_start(void);
//void create_static_thd_selector(void);

#endif /* PROCESS_IMAGE_H */
