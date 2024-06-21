#define BLYNK_TEMPLATE_ID "TMPL67koCQCyf"
#define BLYNK_TEMPLATE_NAME "DHT11"
#define BLYNK_AUTH_TOKEN "k2fKjLx4fwH46sseSM96F4cDMOR7UM9r"

#include <DHT.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp8266.h>

#define DHTPIN D1
#define DHTTYPE DHT11

int relay1 = D7;
DHT dht(DHTPIN, DHTTYPE);

// Temperature thresholds
float temp_cold = 28.0;
float temp_normal = 33.0;
float temp_hot = 34.0;

// Humidity thresholds
float hum_dry = 35.0;
float hum_normal = 60.0;
float hum_humid = 80.0;

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "KAFECINTA";  
char pass[] = "KAFECINTAAA";  

void setup() {  
  pinMode(relay1, OUTPUT);
  Serial.begin(115200);
  dht.begin();
  Blynk.begin(auth, ssid, pass);
}

void loop() {
  Blynk.run();
  
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  float temp_cold_degree = max(0.0f, min(1.0f, (temp_normal - temperature) / (temp_normal - temp_cold)));
  float temp_hot_degree = max(0.0f, min(1.0f, (temperature - temp_normal) / (temp_hot - temp_normal)));
  float temp_normal_degree = 1.0f - temp_cold_degree - temp_hot_degree;

  float hum_dry_degree = max(0.0f, min(1.0f, (hum_normal - humidity) / (hum_normal - hum_dry)));
  float hum_humid_degree = max(0.0f, min(1.0f, (humidity - hum_normal) / (hum_humid - hum_normal)));
  float hum_normal_degree = 1.0f - hum_dry_degree - hum_humid_degree;

  float relay_on_degree = min(temp_hot_degree, hum_dry_degree);
  float relay_off_degree = max(temp_cold_degree, hum_humid_degree);

  float duration;
  if (relay_on_degree > relay_off_degree) {
    duration = relay_on_degree * 30; 
  } else {
    duration = 0;
  }

  // Print the results
  Serial.print(F("temp_cold_degree: ")); Serial.print(temp_cold_degree);
  Serial.print(F(" temp_hot_degree: ")); Serial.print(temp_hot_degree);
  Serial.print(F(" temp_normal_degree: ")); Serial.println(temp_normal_degree);

  Serial.print(F("hum_dry_degree: ")); Serial.print(hum_dry_degree);
  Serial.print(F(" hum_humid_degree: ")); Serial.print(hum_humid_degree);
  Serial.print(F(" hum_normal_degree: ")); Serial.println(hum_normal_degree);

  Serial.print(F("relay_on_degree: ")); Serial.print(relay_on_degree);
  Serial.print(F(" relay_off_degree: ")); Serial.println(relay_off_degree);

  Serial.print(F("Temperature: ")); Serial.print(temperature);
  Serial.print(F(" °C, Humidity: ")); Serial.print(humidity);
  Serial.print(F(" %, Watering duration: ")); Serial.print(duration);
  Serial.println(F(" seconds"));

  Blynk.virtualWrite(V1, temperature);
  Blynk.virtualWrite(V0, humidity);

  if (temp_cold_degree > temp_normal_degree && temp_cold_degree > temp_hot_degree) {
    Blynk.virtualWrite(V2, "Cold");
  } else if (temp_normal_degree > temp_cold_degree && temp_normal_degree > temp_hot_degree) {
    Blynk.virtualWrite(V2, "Normal");
  } else {
    Blynk.virtualWrite(V2, "Hot");
  }

  if (hum_dry_degree > hum_normal_degree && hum_dry_degree > hum_humid_degree) {
    Blynk.virtualWrite(V4, "Dry");
  } else if (hum_normal_degree > hum_dry_degree && hum_normal_degree > hum_humid_degree) {
    Blynk.virtualWrite(V4, "Normal");
  } else {
    Blynk.virtualWrite(V4, "Humid");
  }

  digitalWrite(relay1, HIGH);
  delay(duration * 1000); 
  digitalWrite(relay1, LOW);

  delay(2000);
}
