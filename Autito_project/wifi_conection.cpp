#include "wifi_conection.h"

#include <WiFi.h>
#include <WebServer.h>

WebServer server(80);
int grados = 0;
float* imagen;  // Puntero a la imagen
const int IMAGE_WIDTH = 96;  // Ancho de la imagen
const int IMAGE_HEIGHT = 96; // Alto de la imagen

// Configura la conexión WiFi
void wifi_init() {
  Serial.begin(115200);
  const char* ssid = "Fibertel casa 2.4ghz";
  const char* password = "17ab500xy7";
  
  Serial.println("Desconectamos antes de conectar el WiFi");
  WiFi.disconnect();
  Serial.print("Conectando a ");
  Serial.println(ssid);
  
  // Conectamos el ESP a la red WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  // Intentamos conectarnos a la red
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Si logramos conectarnos, mostramos la IP
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Página principal con JavaScript para mostrar grados e imagen
  server.on("/", []() {
    String content = "";
    content += "<html>";
    content += "<div id=\"contador\">Cargando grados...</div>";
    content += "<canvas id=\"imageCanvas\" width=\"" + String(IMAGE_WIDTH) + "\" height=\"" + String(IMAGE_HEIGHT) + "\"></canvas>";
    content += "<script>";
    content += "function updateData() {";
    content += "fetch('/contador').then(response => response.text()).then(data => {";
    content += "document.getElementById('contador').innerHTML = 'Grados: ' + data; });";
    content += "fetch('/imagen').then(response => response.json()).then(data => {";
    content += "let canvas = document.getElementById('imageCanvas');";
    content += "let ctx = canvas.getContext('2d');";
    content += "let imgData = ctx.createImageData(" + String(IMAGE_WIDTH) + ", " + String(IMAGE_HEIGHT) + ");";
    content += "for(let i = 0; i < data.length; i++) {";
    content += "imgData.data[i * 4] = data[i];"; // Rojo
    content += "imgData.data[i * 4 + 1] = data[i];"; // Verde
    content += "imgData.data[i * 4 + 2] = data[i];"; // Azul
    content += "imgData.data[i * 4 + 3] = 255;"; // Alpha
    content += "}";
    content += "ctx.putImageData(imgData, 0, 0);";
    content += "});";
    content += "}";
    content += "setInterval(updateData, 1000);"; // Actualiza cada 1 segundo
    content += "</script>";
    content += "</html>";
    server.send(200, "text/html", content);
  });

  // Ruta para devolver los grados
  server.on("/contador", []() {
    server.send(200, "text/plain", String(grados));
  });

  // Ruta para devolver la imagen en formato JSON
  server.on("/imagen", []() {
    String jsonImage = "[";
    for (int i = 0; i < IMAGE_WIDTH * IMAGE_HEIGHT; i++) {
      jsonImage += String(static_cast<int>(imagen[i])) + ((i < IMAGE_WIDTH * IMAGE_HEIGHT - 1) ? "," : "");
    }
    jsonImage += "]";
    server.send(200, "application/json", jsonImage);
  });

  server.begin();
}

// Función para manejar las actualizaciones periódicas
void wifi_iteration(int degrees, float* v) {
  grados = degrees;  // Actualizamos los grados
  imagen = v;        // Actualizamos el puntero a la imagen
  server.handleClient();
  delay(100);
}
