#include "pixel_sumation.h"
#include "wifi_conection.h"

// ----------- setup ----------- //
void setup() {
  Serial.begin(115200);
  wifi_init();
  pixel_sumation_init();
}

// ----------- loop ----------- //

unsigned long previousMillis = 0;
const long interval = 100;

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    PredictionResult result = pixel_sumation_iteration();
    wifi_iteration(result.prediccion, result.foto);
  }

  handleClient();
}