#include "pixel_sumation.h"
#include "wifi_conection.h"

// ----------- setup ----------- //
void setup() {
  pixel_sumation_init();
  wifi_init();
}

// ----------- loop ----------- //

void loop() {
  wifi_iteration(pixel_sumation_iteration());
}
