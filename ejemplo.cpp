#include <iostream>
#include <chrono>
#include <thread>

// Función que se ejecuta una sola vez
void setup() {
    std::cout << "Setup: se ejecuta una sola vez al inicio" << std::endl;
}

// Función que se repite indefinidamente
void loop() {
    std::cout << "Loop: ejecutando una vez por segundo..." << std::endl;
}

int main() {
    setup();
    while (true) {
        loop();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}
