#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_SGP30.h>

const char* ssid = getenv("SSID");         // Pobranie SSID z zmiennych środowiskowych
const char* password = getenv("PASSWORD"); // Pobranie hasła z zmiennych środowiskowych
const char* serverIP = getenv("SERVER_IP");  // Pobranie IP serwera z zmiennych środowiskowych
const int serverPort = 8080;

WiFiClient client; 
Adafruit_BME280 bme;
Adafruit_SGP30 sgp;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  WiFi.begin(ssid, password);
  
  if (!sgp.begin()) {
    Serial.println("Nie można znaleźć czujnika SGP30. Sprawdź połączenie!");
    while (1);
  }
  Serial.println("Czujnik SGP30 podłączony!");

  Serial.print("Łączenie z WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nPołączono z WiFi!");
  Serial.print("Adres IP ESP8266: ");
  Serial.println(WiFi.localIP());

  if (!bme.begin(0x76)) {
    if (!bme.begin(0x77)) {
      Serial.println("Nie znaleziono czujnika BME280!");
      while (1);
    }
  }
  Serial.println("Czujnik BME280 podłączony!");
}

void loop() {

  int pressure = bme.readPressure() / 100.0F;

  String jsonData = "{";
  jsonData += "\"temperature\": " + String(bme.readTemperature()) + ",";
  jsonData += "\"pressure\": " + String(pressure) + ",";
  jsonData += "\"readHumidity\": " + String(bme.readHumidity()) + ",";   
  jsonData += "\"TVOC\": " + String(sgp.TVOC) + ",";
  jsonData += "\"eCO2\": " + String(sgp.eCO2);
  jsonData += "}";

   if (!sgp.IAQmeasure()) {
    Serial.println("Błąd odczytu danych z SGP30!");
    return;
  }
  if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http;
    String url = "http://" + String(serverIP) + ":" + String(serverPort) + "/data";
    http.begin(client, url); 
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(jsonData); 
    if (httpResponseCode > 0) {

      Serial.print("Odpowiedź serwera: ");
      Serial.println(httpResponseCode);
      String response = http.getString();
      Serial.println("Treść odpowiedzi: " + response);

    } else {

      Serial.print("Błąd wysyłania danych: ");
      Serial.println(http.errorToString(httpResponseCode));

    }
    http.end(); // Zamknij połączenie
  
  } else {
  
    Serial.println("Brak połączenia z WiFi!");
  
  }

  Serial.print("Temperatura: ");
  Serial.print(bme.readTemperature());
  Serial.println(" °C");

  Serial.print("Ciśnienie: ");
  Serial.print(pressure);
  Serial.println(" hPa");

  Serial.print("Wilgotność: ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");

  Serial.print("TVOC: ");
  Serial.print(sgp.TVOC);
  Serial.println(" ppb");
  
  Serial.print("eCO2: ");
  Serial.print(sgp.eCO2);
  Serial.println(" ppm");

  delay(5000);
}
