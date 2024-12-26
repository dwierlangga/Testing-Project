#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <HTTPClient.h>

// Konfigurasi WiFi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Konfigurasi MQTT
const char* mqtt_server = "broker.emqx.io"; // Broker MQTT
const int mqtt_port = 1883;
const char* mqtt_topic_temp = "DWI/LOCAL/SENSOR/PANEL_1"; // Topik untuk DHT22

// Konfigurasi HTTP POST
const char* host = "28ec-2001-448a-2082-8960-f50a-1b88-1ee0-ce57.ngrok-free.app"; // URL Server ngrok

// Pin DHT22 dan LED
#define DHTPIN 4 // Pin data DHT22
#define DHTTYPE DHT22
#define LED_PIN 2 // Pin untuk LED yang menandakan fan menyala

DHT dht(DHTPIN, DHTTYPE);

// Pin Potentiometer
const int potPin = 36; // GPIO36 (A0)

// MQTT Client
WiFiClient espClient;
PubSubClient mqttClient(espClient);

// HTTPS Client
WiFiClientSecure httpsClient;

// Suhu Normal Ruangan
float normalTemperature = 27.0; // Normal temperature in Celsius

void setupWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void reconnect() {
  while (!mqttClient.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (mqttClient.connect("node1")) {
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setupWiFi();
  mqttClient.setServer(mqtt_server, mqtt_port);

  dht.begin(); // Inisialisasi DHT22
  pinMode(LED_PIN, OUTPUT); // Set pin LED sebagai output

  httpsClient.setInsecure(); // Abaikan sertifikat SSL untuk HTTPS
}

void loop() {
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();

  // Membaca data dari DHT22
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
  } else {
    // Kirim data suhu dan kelembapan ke MQTT
    String payloadTemp = "{\"temperature\":" + String(temperature) + ",\"humidity\":" + String(humidity) + "}";
    mqttClient.publish(mqtt_topic_temp, payloadTemp.c_str());
    Serial.println("Temperature Data sent: " + payloadTemp);

    float temperatureThreshold = normalTemperature * 1.02; // 2% lebih tinggi dari suhu normal
    if (temperature >= temperatureThreshold) {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("Fan ON (LED ON)");
    } else {
      digitalWrite(LED_PIN, LOW);
      Serial.println("Fan OFF (LED OFF)");
    }
  }

  // Membaca data dari Potentiometer
  int potValue = analogRead(potPin);
  float voltage = potValue * (230.0 - 210.0) / 4095 + 210.0;
  float current = potValue * (1.0 - 0.1) / 4095 + 0.1;
  float power = voltage * current;

  // Kirim data ke server menggunakan HTTP POST
  HTTPClient http;
  String postData = "&status1=" + String(temperature) +
                    "&status2=" + String(humidity) +
                    "&status3=" + String(voltage) +
                    "&status4=" + String(current) +
                    "&status5=" + String(power);

  String url = "https://" + String(host) + "/testproject/postdemo.php";
  Serial.println("Connecting to: " + url);
  http.begin(httpsClient, url);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int httpCode = http.POST(postData);
  String payload = http.getString();

  Serial.println("HTTP POST Response:");
  Serial.println("Code: " + String(httpCode));
  Serial.println("Payload: " + payload);

  http.end();

  delay(1000);
}
