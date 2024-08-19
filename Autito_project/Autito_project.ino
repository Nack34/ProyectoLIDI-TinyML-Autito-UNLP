// ----------- INCLUDE ----------- //

// --> CAM
#include "esp_camera.h" 

// --> PARA SOLUCION DE PROBLEMAS
#include "soc/soc.h" //disable brownout problems
#include "soc/rtc_cntl_reg.h"  //disable brownout problems

// ----------- DEFINE ----------- //


// --> Define de valores necesarios para TinyML

  //#define EMBEDIA_MODEL_CHANNELS 1
  //#define EMBEDIA_MODEL_HEIGHT 96
  //#define EMBEDIA_MODEL_WIDTH 96
  //#define NUMBER_OF_INPUTS   (INPUT_CHANNELS*INPUT_WIDTH*INPUT_HEIGHT)

#define RESOLUCION_CAMARA   FRAMESIZE_96X96          // 160x120
//  #define RESOLUCION_CAMARA    FRAMESIZE_QQVGA          // 160x120

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


// --> Const utilizados en la funcion averagePooling()
  #define INPUT_WIDTH 96
  #define INPUT_HEIGHT 96
  #define INPUT_SIZE 9216 // INPUT_WIDTH * INPUT_HEIGHT
float foto[INPUT_SIZE];


char * labels[] = {"Adelante","Izquierda","Derecha"};

// ----------- setup ----------- //


void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
 
  Serial.begin(115200);
  delay(1000);
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

// ----------- loop ----------- //

void loop() {
  camera_fb_t * fb = NULL;

  fb = esp_camera_fb_get();
  if (!fb) {
    ESP_LOGE(TAG, "Camera capture failed");
    return;
  }

  Serial.print("Formato: ");  Serial.print(fb->format);
  Serial.print(", Len: ");    Serial.print(fb->len);
  Serial.print(", Width: ");  Serial.print(fb->width);
  Serial.print(", Height: "); Serial.println(fb->height);

  //unsigned long tiempo = millis();
  uint16_t prediction = predict_direction(fb->buf, fb->width, fb->height, 3);
  //tiempo = millis() - tiempo;

  Serial.print("Prediccion ->  "); Serial.println(labels[prediction]);
  //Serial.print("Tiempo Inferencia en ms: "); Serial.println(tiempo);

  esp_camera_fb_return(fb);
  delay(500);
  Serial.println();
}








uint16_t predict_direction(uint8_t* image, int width, int height, int channels) {
    uint8_t* binary_image = apply_threshold(image, width, height, channels, 128); // Umbral de 128 como ejemplo

    // Suponiendo que 'foto' es un array de float de tamaño 9216
    for (int i = 0; i < 9216; i++) {
        foto[i] = static_cast<float>(image[i]); // Convertir cada elemento de uint8_t a float y asignarlo a 'foto'
    }
    printImage(); // Aquí se imprimirá la imagen original convertida a float

    for (int i = 0; i < 9216; i++) {
        foto[i] = static_cast<float>(binary_image[i]); // Convertir cada elemento de uint8_t a float y asignarlo a 'foto'
    }
    printImage(); // Aquí se imprimirá la imagen trasformada convertida a float

    uint16_t result = simple_pixel_sumation(binary_image, width, height, channels);
    free(binary_image); // Liberar memoria
    return result;
}



// Function to apply thresholding to an image
uint8_t* apply_threshold(const uint8_t* image, int width, int height, int channels, int threshold) {
    int pixel_count = width * height * channels;
    uint8_t* binary_image = (uint8_t*)malloc(pixel_count * sizeof(uint8_t)); // Crear un nuevo vector para la imagen transformada
    
    for (int i = 0; i < pixel_count; i += channels) {
        // Convertir RGB a escala de grises usando la fórmula de luminancia
        unsigned char gray = (0.299 * image[i] + 0.587 * image[i+1] + 0.114 * image[i+2]);

        // Aplicar el umbral
        unsigned char binary = (gray > threshold) ? 255 : 0;

        // Establecer todos los canales de color al valor binario
        binary_image[i] = binary;      // Canal Rojo
        binary_image[i+1] = binary;    // Canal Verde
        binary_image[i+2] = binary;    // Canal Azul
    }

    return binary_image;
}


uint16_t simple_pixel_sumation(const uint8_t* image, int width, int height, int channels) {
    int mid_x = width / 2;
    int left_white_count = count_white_pixels(image, width, height, channels, 0, mid_x);
    int right_white_count = count_white_pixels(image, width, height, channels, mid_x, width);

    Serial.print("Pixeles blancos en el lado izquierdo: "); Serial.println(left_white_count);
    Serial.print("Pixeles blancos en el lado derecho: "); Serial.println(right_white_count);

    const int SOME_THRESHOLD = 100; // Valor de umbral para decidir si la diferencia es significativa

    if (abs(left_white_count - right_white_count) < SOME_THRESHOLD) {
        return 0; // Ir hacia adelante
    } else if (left_white_count > right_white_count) {
        return 1; // Girar a la izquierda
    } else {
        return 2; // Girar a la derecha
    }
}

// Function to count white pixels in a specific region
int count_white_pixels(const uint8_t* image, int width, int height, int channels, int start_x, int end_x) {
    int count = 0;
    for (int y = 0; y < height; y++) {
        for (int x = start_x; x < end_x; x++) {
            int index = (y * width + x) * channels;
            if (image[index] == 255) { // Pixel blanco
                count++;
            }
        }
    }
    return count;
}
// ----------- printImage ----------- //


void printImage(){

  #define STEP_W 1
  #define STEP_H 2
  #define STEP_LN 5
  
  int pos, y, x,getPos,color;
  char str[INPUT_WIDTH+1];
  

  for (pos = 0,y=0; pos <= INPUT_WIDTH; pos += STEP_W, y++) {
    if (pos >=100 && (y%STEP_LN==0))
      Serial.print(pos/100);
    else
      Serial.print(' ');
  }
  Serial.println();
  
  for (pos = 0,y=0; pos <= INPUT_WIDTH; pos += STEP_W, y++) {
    if (pos >=10 && (y%STEP_LN==0))
      Serial.print( (pos % 100)/10);
    else
      Serial.print(' ');
  }
  Serial.println();

 
  for (pos = 0,y=0; pos <= INPUT_WIDTH; pos += STEP_W, y++) {
     if (y%STEP_LN==0)
        Serial.print(pos % 10);
     else
        Serial.print(' ');
  }
  Serial.println();
  
  for (y = 0; y < INPUT_HEIGHT; y += STEP_H) {
    pos = 0;
    //memset(str, 0, sizeof(str));
    for (x = 0; x < INPUT_WIDTH; x += STEP_W) {
      getPos = y * INPUT_WIDTH + x;
      color = foto[getPos];

      if (color > 224) {
        str[pos] = ' ';
      } else if (color > 192) {
        str[pos] = '-';
      } else if (color > 160) {
        str[pos] = '+';
      } else if (color > 128) {
        str[pos] = '=';
      } else if (color > 96) {
        str[pos] = 'O';
      } else if (color > 64) {
        str[pos] = 'H';
      } else if (color > 32) {
        str[pos] = '#';
      } else {
        str[pos] = 'M';
      }

      pos++;
    }
    str[INPUT_WIDTH / STEP_W] = 0;
    Serial.println(str);
  }

}

// ----------- printVectorInt ----------- //
// ---> Imprime los valores enteros que representan la imagen original capturada
// ---> Utiliza el formato de numpy array para facilitar su análisis posterior
void printVectorInt(uint8_t * v){
    int i;
    Serial.print("vector_orig = np.array{[");
    for(i=0;i<96*96;i++){
        Serial.print(v[i]);
        Serial.print(",");
    }
    Serial.println("]}");
}

// ----------- printVectorFloat ----------- //
// ---> Imprime los valores en float que representan la imagen procesada
// ---> Los valores representan los píxeles después de aplicar la operación de averagePooling

void printVectorFloat(float * v){
    int i;
    Serial.print("vector_recortado = np.array{[");
    for(i=0;i<INPUT_SIZE;i++){
        Serial.print(v[i]);
        Serial.print(",");
    }
    Serial.println("]}");
}

