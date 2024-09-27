#define PIXEL_SUMATION_H

#include <stdint.h> 

#include <Arduino.h>



void pixel_sumation_init();
int pixel_sumation_iteration();
int predict_direction(uint8_t* image, int width, int height);
uint8_t* apply_threshold_grayscale(const uint8_t* image, int width, int height, int threshold);
int simple_pixel_sumation(const uint8_t* image, int width, int height);
int calculate_degrees(int left_white_count, int right_white_count);
int count_white_pixels(const uint8_t* image, int width, int height, int start_x, int end_x);
void printImage();
void printVectorInt(uint8_t * v);
void printVectorFloat(float * v);

