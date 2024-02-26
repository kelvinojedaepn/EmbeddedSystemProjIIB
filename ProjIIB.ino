#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "DHT.h"
#include "MatrizLed.h"

const char* ssid = "XTRIM_HERMANOS OQUI";
const char* password = "0201179652";

WiFiServer server(80);

const int redPin = 12;    // Pin para el canal rojo del LED RGB
const int greenPin = 13;  // Pin para el canal verde del LED RGB
const int bluePin = 2;    // Pin para el canal azul del LED RGB

#define DHTTYPE DHT11  // DHT 11
#define dht_dpin 0
DHT dht(dht_dpin, DHTTYPE);

MatrizLed pantalla;

int DIN = 16;  // D0
int CS = 5;    // D1
int CLK = 4;   // D2

int ledPinInfrared = 15;  // Pin para el LED infrarrojo
int inputPinInfrared = 14;          // Pin de entrada para el sensor infrarrojo

unsigned long previousMillis = 0;
const long interval = 2000;  // Intervalo de actualización en milisegundos (3 segundos)

boolean printToMatrix = false;

void setup() {
  // Inicializar el sensor DHT11
  dht.begin();
  // Inicializar la matriz de LED
  pantalla.begin(DIN, CLK, CS, 1);  //(dataPin, clockPin, csPin, numDevices)

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(ledPinInfrared, OUTPUT);
  pinMode(inputPinInfrared, INPUT);

  // Inicialmente apagamos el LED RGB e infrarrojo
  digitalWrite(redPin, HIGH);
  digitalWrite(greenPin, HIGH);
  digitalWrite(bluePin, HIGH);
  digitalWrite(ledPinInfrared, LOW);

  // Iniciar conexión Wi-Fi
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Conexión WiFi establecida");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  // Lecturas de temperatura y humedad
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    float t = dht.readTemperature();
    float h = dht.readHumidity();

    // Imprimir en el puerto serial
    Serial.println("<h2>Lecturas de DHT11</h2>");
    Serial.print("Temperatura: ");
    Serial.print(t);
    Serial.println("°C");
    Serial.print("Humedad: ");
    Serial.print(h);
    Serial.println("%");

    // Manejo de clientes web
    WiFiClient client = server.available();
    if (client) {
      Serial.println("Nuevo cliente");
      while (!client.available()) {
        delay(1);
      }

      // Lectura de la solicitud HTTP
      String request = client.readStringUntil('\r');
      Serial.println(request);
      client.flush();

      // Control del LED RGB
      if (request.indexOf("/RED=TOGGLE") != -1) {
        digitalWrite(redPin, !digitalRead(redPin));
      } else if (request.indexOf("/GREEN=TOGGLE") != -1) {
        digitalWrite(greenPin, !digitalRead(greenPin));
      } else if (request.indexOf("/BLUE=TOGGLE") != -1) {
        digitalWrite(bluePin, !digitalRead(bluePin));
      } else if (request.indexOf("/PRINT=TRUE") != -1) {
        printToMatrix = true;
      }

      // Página web de control del LED RGB y la matriz LED
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println("");
      client.println("<!DOCTYPE HTML>");
      client.println("<html>");
      client.println("<head><title>Control de LED RGB y DHT11</title></head>");
      client.println("<style>");
      client.println("body {");
      client.println("  font-family: Helvetica, Arial, sans-serif;"); // Tipo de letra Helvetica
      client.println("  display: flex;");
      client.println("  justify-content: center;");
      client.println("  align-items: center;");
      client.println("  height: 100vh;"); // Establece la altura al 100% de la altura de la ventana del navegador
      client.println("  margin: 0;"); // Elimina el margen predeterminado
      client.println("  background-image: url('https://wallpapers.com/images/hd/1600x900-background-kbocjfpftninq69q.jpg');");
      client.println("  background-size: cover;"); // Para cubrir todo el viewport
      client.println("  background-position: center;"); // Centrar la imagen
      client.println("  background-attachment: fixed;"); // Fondo fijo mientras se desplaza la página
      client.println("  background-color: rgba(0, 0, 0, 0.5);"); // Transparencia del fondo
      client.println("  font-weight: bold;"); // Transparencia del fondo
      client.println("}");

      client.println("*{");
      client.println(" font-weight: bold;");
      client.println("}");



      client.println("button {");
      client.println("  border-radius: 10px;"); // Bordes redondeados
      client.println("  padding: 15px 20px;"); // Espaciado interno
      client.println("  border: none;"); // Sin borde
      client.println("}");

      client.println(".active-red {");
      client.println("  background-color: #FF0000;"); // Fondo rojo cuando está activo
      client.println("}");

      client.println(".active-green {");
      client.println("  background-color: #00FF00;"); // Fondo verde cuando está activo
      client.println("}");

      client.println(".active-blue {");
      client.println("  background-color: #0000FF;"); // Fondo azul cuando está activo
      client.println("  color: white;"); // Fondo azul cuando está activo
      client.println("}");

      
      client.println(".inactive {");
      client.println("  background-color: rgba(0.2, 0.2, 0.2, 0.33);"); // Transparencia del fondo
      client.println("  color: white;");
      client.println("  margin-right: 3px;");
      client.println("  border: 2px solid #000000;"); // Borde rojo cuando está inactivo
      client.println("  color: white;"); // Borde rojo cuando está inactivo
      client.println("}");

      client.println("button.print:hover {");
      client.println("  background-color: black;");
      client.println("  color: white;");
      client.println("  border: none;");
      client.println("  border-radius: 5px;");
      client.println("  padding: 10px 20px;");
      client.println("  text-align: center;");
      client.println("  text-decoration: none;");
      client.println("  display: inline-block;");
      client.println("  font-size: 16px;");
      client.println("  margin: 4px 2px;");
      client.println("  cursor: pointer;");
      client.println("}");
      
      client.println("button.print {");
      client.println("  background-color: rgba(0.15, 0.15, 0.15, 0.33);"); // Transparencia del fondo
      client.println("  color: white;");
      client.println("  font-weight: white;");
      client.println("  border: 2px solid #000000;"); // Borde rojo cuando está inactivo
      client.println("}");

      client.println(".container {");
      client.println("  text-align: center;"); // Alinea el texto al centro
      client.println("}");

      client.println(".p-status {");
      client.println("  color: white"); // Sombra más grande al pasar el mouse
      client.println("}");

      client.println(".card:hover {");
      client.println("  box-shadow: 0 8px 16px 0 rgba(0, 0, 0, 0.5);"); // Sombra más grande al pasar el mouse
      client.println("}");

      client.println(".card {");
      client.println("  border-radius: 10px;"); // Bordes redondeados
      client.println("  box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2);"); // Sombra
      client.println("  transition: 0.3s;"); // Transición suave al pasar el mouse
      client.println("  width: 300px;"); // Ancho de la tarjeta
      client.println("  padding: 20px;"); // Espaciado interno
      client.println("  background-color: rgba(0, 0, 0, 0.65);"); // Transparencia del fondo
      client.println("}");
      
      client.println(".card-1:hover {");
      client.println("  box-shadow: 0 8px 16px 0 rgba(0, 0, 0, 0.5);"); // Sombra más grande al pasar el mouse
      client.println("}");

      client.println(".card-1 {");
      client.println("  border-radius: 10px;"); // Bordes redondeados
      client.println("  box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2);"); // Sombra
      client.println("  transition: 0.3s;"); // Transición suave al pasar el mouse
      client.println("  width: 300px;"); // Ancho de la tarjeta
      client.println("  padding: 20px;"); // Espaciado interno
      client.println("  background-color: rgba(0, 0, 0, 0.65);"); // Transparencia del fondo
      client.println("  margin-top: 10px;"); // Transparencia del fondo
      client.println("}");

      client.println(".card-t:hover {");
      client.println("  box-shadow: 0 8px 16px 0 rgba(0, 0, 0, 0.5);"); // Sombra más grande al pasar el mouse
      client.println("}");

      client.println(".card-t {");
      client.println("  border-radius: 10px;"); // Bordes redondeados
      client.println("  box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2);"); // Sombra
      client.println("  transition: 0.3s;"); // Transición suave al pasar el mouse
      client.println("  width: 300px;"); // Ancho de la tarjeta
      client.println("  padding: 20px;"); // Espaciado interno
      client.println("  background-color: rgba(0, 0, 0, 0.65);"); // Transparencia del fondo
      client.println("  margin-top: 10px;"); // Margen superior
      client.println("}");
      
      client.println(".card-e:hover {");
      client.println("  box-shadow: 0 8px 16px 0 rgba(0, 0, 0, 0.5);"); // Sombra más grande al pasar el mouse
      client.println("}");

      client.println(".card-e {");
      client.println("  border-radius: 10px;"); // Bordes redondeados
      client.println("  box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2);"); // Sombra
      client.println("  transition: 0.3s;"); // Transición suave al pasar el mouse
      client.println("  width: 300px;"); // Ancho de la tarjeta
      client.println("  padding: 20px;"); // Espaciado interno
      client.println("  background-color: rgba(0.65, 0.65, 0.65, 0.65);"); // Transparencia del fondo
      client.println("  margin-top: 10px;"); // Margen superior
      client.println("  margin-bottom: 10px;"); // Margen superior
      client.println("}");
      

      
      client.println("</style>");
      client.println("<body>");

      client.println("<div class=\"container\">");
      client.println("<h1 style=\"color: white; margin-top=10px;\">Arduino Server IoT</h1>");
      client.println("<div class=\"card\" style=\"background-image: url('https://img.freepik.com/free-photo/aesthetic-background-with-pink-neon-led-light-effect_53876-103788.jpg');\">");
      client.println("<p class=\"p-status\"><a href=\"/RED=TOGGLE\"><button class=\"" + String(digitalRead(redPin) ?  "inactive": "active-red") + "\">Rojo</button></a> Estado: " + String(digitalRead(redPin) ? "OFF" : "ON") + "</p>");
      client.println("<p class=\"p-status\"><a href=\"/GREEN=TOGGLE\"><button class=\"" + String(digitalRead(greenPin) ?  "inactive":"active-green" ) + "\">Verde</button></a> Estado: " + String(digitalRead(greenPin) ? "OFF" : "ON") + "</p>");
      client.println("<p class=\"p-status\"><a href=\"/BLUE=TOGGLE\"><button class=\"" + String(digitalRead(bluePin) ? "inactive": "active-blue") + "\">Azul</button></a> Estado: " + String(digitalRead(bluePin) ? "OFF" : "ON") + "</p>");
      client.println("<p class=\"p-status\"><a href=\"/PRINT=TRUE\"><button class=\"print\">Imprimir en la Matriz LED</button></a></p>");
      client.println("</div>"); 

      // Mostrar lecturas de temperatura y humedad en la página web
      
      client.print("<div class=\"card-1\" style=\"background-image: url('");
      client.print(t <= 14.00 ? "https://mundoruralweb.com.ar/wp-content/uploads/2020/01/campo-atardecer-990x419.jpg" : "https://upload.wikimedia.org/wikipedia/commons/5/50/Tamshiyacu_Tahuayo_Regional_Conservation_Area_Iquitos_Amazon_Rainforest_Peru.jpg");
      client.println("');\">");
      client.println("<h2>Lecturas de DHT11</h2>");
      client.print("Temperatura: ");
      client.print(t);
      client.println(" C<br>");
      client.print("Humedad: ");
      client.print(h);
      client.println("%<br>");
      client.println("</div>"); 

      // Mostrar estado de la temperatura en la página web
      client.print("<div class=\"card-t\" style=\"background-image: url('");
      client.print(t <= 14.00 ? "https://rcpnoticias.com.mx/wp-content/uploads/2020/09/temperatura-mas-fria-tierra.png" : "https://humanidades.com/wp-content/uploads/2017/07/clima-calido-2-e1571417682380.jpg");
      client.println("');\">");
      client.println("<h2 style=\"color: white;\">Estado de la temperatura</h2>");
      if (!isnan(t)) {
        if (t <= 14.00) {
          client.println("<p style=\"color: white;\">Temperatura fría</p>");
        } else if (t >= 25.00) {
          client.println("<p style=\"color: white;\">Temperatura caliente</p>");
        } else {
          client.println("<p style=\"color: white;\">Temperatura normal</p>");
        }
      } else {
        client.println("<p style=\"color: white;\">Error al leer la temperatura</p>");
      }
      client.println("</div>");

      // Mostrar estado de la humedad en la página web
      client.print("<div class=\"card-t\" style=\"background-image: url('");
      client.print(h <= 50.00 ? "https://upload.wikimedia.org/wikipedia/commons/thumb/8/82/Sonoran_Desert_33.081359_n112.431507.JPG/800px-Sonoran_Desert_33.081359_n112.431507.JPG" : "https://i.blogs.es/bae948/borneo_rainforest/450_1000.jpg");
      client.println("');\">");
      client.println("<h2 style=\"color: white;\">Estado de la humedad</h2>");
      if (!isnan(h)) {
        if (h < 50) {
          client.println("<p style=\"color: white;\">Humedad baja</p>");
        } else if (h >= 75) {
          client.println("<p style=\"color: white;\">Humedad alta</p>");
        } else {
          client.println("<p style=\"color: white;\">Humedad normal</p>");
        }
      } else {
        client.println("<p style=\"color: white;\">Error al leer la humedad</p>");
      }
      client.println("</div>");


      // Sensor infrarrojo
      int val = digitalRead(inputPinInfrared);  // Lee el valor de entrada del sensor
      Serial.print("\nVAL ");
      Serial.println(val);

      client.print("<div class=\"card-e\" style=\"background-image: url('");
      client.print(val == 0 ? "https://d500.epimg.net/cincodias/imagenes/2020/05/04/lifestyle/1588577532_319277_1588577593_noticia_normal.jpg" : "https://www.shutterstock.com/image-photo/beagle-dogs-intelligent-lively-fun-260nw-2311861335.jpg");
      client.println("'); margin-bottom=10px;\">");
      client.println("<h2 style=\"color: black;\">Sensor infrarrojo</h2>");
      if (val == LOW) {
        // Si el sensor detecta algo, apaga el LED
        digitalWrite(ledPinInfrared, HIGH);
        
      } else {
        // Si no se detecta nada, enciende el LED
        digitalWrite(ledPinInfrared, LOW);
        
      }
      client.println("</div>");
      client.println("</div>");

      client.println("</body></html>");
      delay(1);
      Serial.println("Cliente desconectado");
      Serial.println("");
    }
  }




  // Imprimir en la matriz LED si se activa la bandera
  if (printToMatrix) {
    float t = dht.readTemperature();
    if (t <= 14.00) {
      pantalla.borrar();
      pantalla.escribirFraseScroll("Frio", 150);
      Serial.println("Imprimiendo Frio");
      delay(2000);
    } else if (t >= 25.00) {
      pantalla.borrar();
      pantalla.escribirFraseScroll("Calido", 150);
      Serial.println("Imprimiendo Caliente");
      delay(2000);
    } else {
      pantalla.borrar();
      pantalla.escribirFraseScroll("Normal", 150);
      Serial.println("Imprimiendo Normal");
      delay(2000);
    }

    printToMatrix = false;  // Reiniciar la bandera después de imprimir en la matriz
  }
}
