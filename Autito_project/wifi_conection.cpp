#include "wifi_conection.h"


//LINK AL VIDEO: https://www.youtube.com/watch?v=cxzhuWPPVfI
//LINK AL CODIGO: https://www.programadornovato.com/wifi-esp32/

#include <WiFi.h>
#include <WebServer.h>

WebServer server(80);
unsigned long grados = 0;

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

  // Si logramos conectarnos mostramos la IP
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Página principal con JavaScript para actualizar el contador automáticamente
  server.on("/", []() {
    String content = "";
    content += "<html>";
    //content += "<div><a href=\"encender\">Encender</a></div>";
    //content += "<div><a href=\"apagar\">Apagar</a></div>";
    content += "<div id=\"contador\">Cargando contador de milisegundos...</div>";
    content += "<script>";
    content += "setInterval(function() {";
    content += "fetch('/contador').then(response => response.text()).then(data => {";
    content += "document.getElementById('contador').innerHTML = 'Grados: ' + data;";
    content += "});";
    content += "}, 1000);"; // Actualiza cada 1 segundo (1000 ms)
    content += "</script>";
    content += "</html>";
    server.send(200, "text/html", content);
  });

  // Ruta para devolver el valor del contador de milisegundos
  server.on("/contador", []() {
    server.send(200, "text/plain", String(grados));
  });

  server.begin();
}

void wifi_iteration(int degrees) {
  grados = degrees; // Actualizamos los degrees
  server.handleClient();
  delay(100);
}
