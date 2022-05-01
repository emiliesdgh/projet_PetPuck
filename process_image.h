#ifndef PROCESS_IMAGE_H
#define PROCESS_IMAGE_H

#define DAWN					50
#define DUSK					70
#define IMAGE_BUFFER_SIZE		640


float get_distance_cm(void);
uint16_t get_line_position(void);
void process_image_start(void);
void ambient_light(uint8_t *buffer);


#endif /* PROCESS_IMAGE_H */
