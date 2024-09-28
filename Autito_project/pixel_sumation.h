#ifndef PIXEL_SUMATION_H
#define PIXEL_SUMATION_H

#include <stdint.h>
#include <Arduino.h>

struct PredictionResult {
    int prediccion;
    float* foto; // Asegúrate de que el tipo de datos de 'foto' sea correcto
};

// ----------- DECLARACIÓN DE FUNCIONES ----------- //

// Inicialización de la configuración de la cámara y modelo
void pixel_sumation_init();

// Captura una imagen, predice la dirección y retorna el ángulo
PredictionResult pixel_sumation_iteration();

// Predice la dirección con base en la imagen capturada y procesada
int predict_direction(uint8_t* image, int width, int height);

// Aplica un umbral a la imagen en escala de grises y retorna una imagen binaria
uint8_t* apply_threshold_grayscale(const uint8_t* image, int width, int height, int threshold);

// Realiza la suma simple de los píxeles blancos en la imagen binaria
int simple_pixel_sumation(const float* image, int width, int height);

// Calcula los grados de desviación en función de los píxeles blancos
int calculate_degrees(int left_white_count, int right_white_count);

// Cuenta los píxeles blancos en una región específica de la imagen binaria
int count_white_pixels(const float* image, int width, int height, int start_x, int end_x);

// Imprime la imagen procesada en la consola en formato ASCII
void printImage();

// Imprime el vector de enteros que representa la imagen original
void printVectorInt(uint8_t * v);

// Imprime el vector de flotantes que representa la imagen procesada
void printVectorFloat(float * v);

#endif // PIXEL_SUMATION_H
