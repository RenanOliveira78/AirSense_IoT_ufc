#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// WiFi credentials
const char* ssid = "galaxy14";       // Replace with your WiFi SSID
const char* password = "1237renan";    // Replace with your WiFi password

// ThingSpeak MQTT credentials
const char* mqttServer = "mqtt3.thingspeak.com";
const int mqttPort = 1883;
const char* mqttDeviceName = "AirS-01";
const char* mqttUser = "DRAVGw4JIBwjJS4VHAQBJzM";  // Replace with ThingSpeak MQTT Username
const char* mqttPassword = "3le000KUtB9Iv8t00rWKj/ed";  // Replace with ThingSpeak MQTT Password
const char* mqttTopic = "channels/2789610/publish"; // Replace with your Channel ID and Write API Key

WiFiClient espClient;
PubSubClient client(espClient);

// Pins
#define DHTPIN 4       // DHT11 data pin
#define DHTTYPE DHT11  // DHT11 sensor type
#define MQ7PIN 34      // MQ-7 sensor analog pin
#define MQ135PIN 35    // MQ-135 sensor analog pin

// Constants
const float MQ7_R0 = 6.22;    // values attained from calibration
const float MQ135_R0 = 138.87;
const float MQ7_m = -0.6527;   // Slope for MQ-7
const float MQ7_b = 1.30;      // Intercept for MQ-7
const float MQ135_m = -0.3376;   // Slope for MQ-135
const float MQ135_b = 0.7165;    // Intercept for MQ-135

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin(); // Commented out for testing without sensors

  pinMode(MQ7PIN, INPUT);  // Commented out for testing without sensors
  pinMode(MQ135PIN, INPUT);  // Commented out for testing without sensors

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

  // Connect to MQTT broker
  client.setServer(mqttServer, mqttPort);
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect(mqttUser, mqttUser, mqttPassword)) {
      Serial.println("Connected to MQTT");
    } else {
      Serial.print("Failed, state: ");
      Serial.println(client.state());
      delay(2000);
    }
  }

  Serial.println("Temp,Humidity,CO_ppm,AirQuality_ppm"); // CSV header
}

void loop() {
  /*
  // Simulated random values for testing
  float temp = random(200, 350) / 10.0;    // Simulated temperature between 20.0°C and 35.0°C
  float humidity = random(300, 800) / 10.0; // Simulated humidity between 30.0% and 80.0%
  float mq7_ppm = random(0.5, 10);          // Simulated CO concentration between 5 and 100 ppm
  float mq135_ppm = random(50, 800);       // Simulated air quality index between 50 and 300
  */
  // Uncomment the code below when using actual sensors
  
  // Read temperature and humidity from DHT11
  float temp = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Default values in case of read failure
  float mq7_ppm = -1.0;
  float mq135_ppm = -1.0;

  // Check DHT sensor
  if (isnan(temp) || isnan(humidity)) {
    temp = -1.0;      // Indicate failed reading
    humidity = -1.0;  // Indicate failed reading
  }

  // Read MQ-7 data
  int mq7_raw = analogRead(MQ7PIN);
  if (mq7_raw > 0) {
    float mq7_volt = mq7_raw * (3.3 / 4095.0);               // Convert ADC to voltage
    float mq7_RS = ((3.3 * 10.0) / mq7_volt) - 10.0;         // Calculate RS for MQ-7
    float mq7_ratio = mq7_RS / MQ7_R0;                       // Calculate ratio
    float mq7_ppm_log = (log10(mq7_ratio) - MQ7_b) / MQ7_m;  // Calculate log(PPM)
    mq7_ppm = pow(10, mq7_ppm_log);                          // Convert log value to PPM
  }

  // Read MQ-135 data
  int mq135_raw = analogRead(MQ135PIN);
  if (mq135_raw > 0) {
    float mq135_volt = mq135_raw * (3.3 / 4095.0);                     // Convert ADC to voltage
    float mq135_RS = ((3.3 * 10.0) / mq135_volt) - 10.0;              // Calculate RS for MQ-135
    float mq135_ratio = mq135_RS / MQ135_R0;                          // Calculate ratio
    float mq135_ppm_log = (log10(mq135_ratio) - MQ135_b) / MQ135_m;   // Calculate log(PPM)
    mq135_ppm = pow(10, mq135_ppm_log);                               // Convert log value to PPM
  }
  

  // Publish data to ThingSpeak
  String payload = String("field1=") + String(temp) +
                   "&field2=" + String(humidity) +
                   "&field3=" + String(mq7_ppm) +
                   "&field4=" + String(mq135_ppm);

  if (client.publish(mqttTopic, payload.c_str())) {
    Serial.println("Data sent to ThingSpeak: " + payload);
  } else {
    Serial.println("Failed to send data to ThingSpeak");
  }

  delay(5000); // Delay for 15 seconds before the next reading
}
