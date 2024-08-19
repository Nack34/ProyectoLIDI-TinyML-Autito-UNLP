// ----------- INCLUDE ----------- //

#include "Arduino.h"
#include "embedia.h"
//#include "embedia_model.h"
#include "sequential_13_model.h"

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
  // Definir tamaño de la imagen original y la imagen resultante
  #define ORIG_SIZE 96 //--> size original img
  #define POOL_SIZE 48 //--> size pooling img
  //#define INPUT_SIZE (ORIG_SIZE * ORIG_SIZE)



float foto[INPUT_SIZE];
float fotoMaxPooleada[INPUT_SIZE];
float fotoAveragePooleada[INPUT_SIZE];
data3d_t input = { INPUT_CHANNELS, INPUT_WIDTH, INPUT_HEIGHT, foto};
data1d_t results;
char * labels[] = {"Piedra","Papel","Tijera","Nada "};

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
  model_init();

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
  }

  uint32_t i;

  maxPooling(fb->buf, fotoMaxPooleada, ORIG_SIZE, POOL_SIZE);  // IMPORTANT!! -> foto = foto with averagePooling 
  averagePooling(fb->buf, fotoAveragePooleada, ORIG_SIZE, POOL_SIZE);  // IMPORTANT!! -> foto = foto with averagePooling 

  float usarMax = 1.0f;
  foto = usarMax ? fotoMaxPooleada : fotoAveragePooleada;
  
  printImage();

  printVectors();

  Serial.print("Formato: ");  Serial.print(fb->format);
  Serial.print(", Len: ");    Serial.print(fb->len);
  Serial.print(", Width: ");  Serial.print(fb->width);
  Serial.print(", Height: "); Serial.println(fb->height);

  esp_camera_fb_return(fb);

  input.data = foto;
  // normalizacion
  for (uint16_t i = 0; i < INPUT_SIZE; i++) {
    input.data[i] = input.data[i] * 0.00392;
  }

  unsigned long tiempo = millis();
  uint16_t prediction = model_predict_class(input, &results);
  tiempo = millis() - tiempo; 
  
  Serial.print("Prediccion numero ->  "); Serial.println(labels[prediction]);
  Serial.print("Tiempo Inferencia en ms: "); Serial.println(tiempo);
  
  delay(500);
  Serial.println();
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


// ----------- printVectors ----------- //
// --->  Para ver los vectores
void printVectors(uint8_t * v){
  Serial.print("Imagen original: "); printVectorInt(fb->buf);  
  Serial.print("Imagen con maxPooling: "); printVectorFloat(fotoMaxPooleada);  
  Serial.print("Imagen con averagePooling: "); printVectorFloat(fotoAveragePooleada);  
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


// ----------- averagePooling ----------- //

void averagePooling(uint8_t* input, float* output, int origSize, int poolSize) {
  int stride = origSize / poolSize;
  for (int i = 0; i < poolSize; i++) {
    for (int j = 0; j < poolSize; j++) {
      float sumVal = 0.0;
      for (int m = 0; m < stride; m++) {
        for (int n = 0; n < stride; n++) {
          int x = i * stride + m;
          int y = j * stride + n;
          sumVal += input[x * origSize + y];
        }
      }
      output[i * poolSize + j] = sumVal / (stride * stride);
    }
  }
}

// ----------- maxPooling ----------- //

void maxPooling(uint8_t* input, float* output, int origSize, int poolSize) {
  int stride = origSize / poolSize;
  for (int i = 0; i < poolSize; i++) {
    for (int j = 0; j < poolSize; j++) {
      uint8_t maxVal = 0;
      for (int m = 0; m < stride; m++) {
        for (int n = 0; n < stride; n++) {
          int x = i * stride + m;
          int y = j * stride + n;
          if (input[x * origSize + y] > maxVal) {
            maxVal = input[x * origSize + y];
          }
        }
      }
      output[i * poolSize + j] = maxVal;
    }
  }
}
