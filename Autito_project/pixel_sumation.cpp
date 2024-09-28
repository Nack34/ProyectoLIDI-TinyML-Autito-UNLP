#include "pixel_sumation.h"

// ----------- INCLUDES ----------- //
#include "esp_camera.h" 
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

// ----------- DEFINES ----------- //
#define RESOLUCION_CAMARA   FRAMESIZE_96X96
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

#define INPUT_WIDTH 96
#define INPUT_HEIGHT 96
#define INPUT_SIZE (INPUT_WIDTH * INPUT_HEIGHT)
float foto[INPUT_SIZE];
char* labels[] = {"Adelante", "Izquierda", "Derecha"};

// ----------- SETUP ----------- //
void pixel_sumation_init() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
  Serial.begin(115200);

  Serial.println("Bienvenido pececillo");
  Serial.setDebugOutput(false);
  Serial.println();
  Serial.println("Cargado :D -> Configuración camara...");

  // Configuración de la camara
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_GRAYSCALE;
  
  if(psramFound()){
    config.frame_size = RESOLUCION_CAMARA;
    config.fb_count = 2;
  } else {
    Serial.println("UTILIZA FRAMESIZE_SVGA");
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  
  // Model init
  //model_init();

  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  Serial.println("Listolin...");

}

// ----------- LOOP ----------- //
PredictionResult pixel_sumation_iteration() {
  PredictionResult result;
  result.prediccion = 0;
  result.foto = NULL;


  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Error al capturar la imagen.");
    return result;
  }

  int prediccion = predict_direction(fb->buf, fb->width, fb->height);
  Serial.printf("Predicción: %d\n", prediccion);
  esp_camera_fb_return(fb);
  
  result.prediccion = prediccion;
  result.foto = foto;

  return result;
}

// ----------- PREDICT ----------- //
int predict_direction(uint8_t* image, int width, int height) {
  uint8_t* binary_image = apply_threshold_grayscale(image, width, height, 128);

  for (int i = 0; i < INPUT_SIZE; i++) {
    foto[i] = static_cast<float>(binary_image[i]);
  }

  //printImage();
  free(binary_image);

  return simple_pixel_sumation(foto, width, height);
}

// ----------- THRESHOLD ----------- //
uint8_t* apply_threshold_grayscale(const uint8_t* image, int width, int height, int threshold) {
  int pixel_count = width * height;
  uint8_t* binary_image = (uint8_t*)malloc(pixel_count);

  for (int i = 0; i < pixel_count; i++) {
    binary_image[i] = (image[i] > threshold) ? 255 : 0;
  }
  return binary_image;
}

// ----------- SUMACIÓN DE PÍXELES ----------- //
int simple_pixel_sumation(const float* image, int width, int height) {
  int mid_x = width / 2;
  int left_white = count_white_pixels(image, width, height, 0, mid_x);
  int right_white = count_white_pixels(image, width, height, mid_x, width);
  return calculate_degrees(left_white, right_white);
}

// ----------- CÁLCULO DE ÁNGULO ----------- //
#define ANGULOS_DE_GIRO 50
int calculate_degrees(int left_white_count, int right_white_count) {
  int desviacion = right_white_count - left_white_count;
  int total_pixels = left_white_count + right_white_count;
  if (total_pixels == 0) return 0;
  return ANGULOS_DE_GIRO * desviacion / total_pixels;
}

// ----------- CONTEO DE PÍXELES BLANCOS ----------- //
int count_white_pixels(const float* image, int width, int height, int start_x, int end_x) {
  int count = 0;
  for (int y = 0; y < height; y++) {
    for (int x = start_x; x < end_x; x++) {
      if (image[y * width + x] == 255) {
        count++;
      }
    }
  }
  return count;
}

// ----------- PRINT IMAGE ----------- //
void printImage() {
  char str[INPUT_WIDTH + 1];
  for (int y = 0; y < INPUT_HEIGHT; y += 2) {
    for (int x = 0; x < INPUT_WIDTH; x += 1) {
      int color = foto[y * INPUT_WIDTH + x];
      str[x] = (color > 224) ? ' ' : (color > 128) ? '=' : '#';
    }
    str[INPUT_WIDTH] = 0;
    Serial.println(str);
  }
}
