#include "pixel_sumation.h"
#include "wifi_conection.h"

// ----------- setup ----------- //
void setup() {
  wifi_init();
  pixel_sumation_init();
}

// ----------- loop ----------- //

void loop() {
  PredictionResult result = pixel_sumation_iteration();

  wifi_iteration(result.prediccion, result.foto);
  delay(500);
  wifi_iteration(result.prediccion, result.foto);
}
