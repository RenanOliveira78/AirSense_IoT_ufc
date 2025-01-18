#include <DHT.h>

// Pins
#define DHTPIN 4       // DHT11 data pin
#define DHTTYPE DHT11  // DHT11 sensor type
#define MQ7PIN 34      // MQ-7 sensor analog pin
#define MQ135PIN 35    // MQ-135 sensor analog pin

// Constants
const float MQ7_R0 = 6.22;    // values attained from calibration
const float MQ135_R0 = 238.87;
const float MQ7_m = -0.6527;   // Slope for MQ-7
const float MQ7_b = 1.30;      // Intercept for MQ-7
const float MQ135_m = -0.3376;   // Slope for MQ-135
const float MQ135_b = 0.7165;    // Intercept for MQ-135

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(MQ7PIN, INPUT);
  pinMode(MQ135PIN, INPUT);
}

void loop() {
  // Read temperature and humidity from DHT11
  float temp = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temp) || isnan(humidity)) {
    Serial.println("Error: Failed to read from DHT sensor.");
  } else {
    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.print("°C, Humidity: ");
    Serial.print(humidity);
    Serial.println("%");
  }

  // Read MQ-7 data
  int mq7_raw = analogRead(MQ7PIN);
  if (mq7_raw <= 0) {
    Serial.println("Error: Failed to read from MQ-7 sensor.");
  } else {
    float mq7_volt = mq7_raw * (3.3 / 4095.0);               // Convert ADC to voltage
    float mq7_RS = ((3.3 * 10.0) / mq7_volt) - 10.0;        // Calculate RS for MQ-7
    float mq7_ratio = mq7_RS / MQ7_R0;                      // Calculate ratio
    float mq7_ppm_log = (log10(mq7_ratio) - MQ7_b) / MQ7_m; // Calculate log(PPM)
    float mq7_ppm = pow(10, mq7_ppm_log);                   // Convert log value to PPM

    Serial.println("\n--- MQ-7 (Carbon Monoxide) ---");
    //Serial.print("Raw Value: ");
    //Serial.println(mq7_raw);
    //Serial.print("Voltage: ");
    //Serial.println(mq7_volt);
    Serial.print("PPM: ");
    Serial.println(mq7_ppm);
  }

  // Read MQ-135 data
  int mq135_raw = analogRead(MQ135PIN);
  if (mq135_raw <= 0) {
    Serial.println("Error: Failed to read from MQ-135 sensor.");
  } else {
    float mq135_volt = mq135_raw * (3.3 / 4095.0);                  // Convert ADC to voltage
    float mq135_RS = ((3.3 * 10.0) / mq135_volt) - 10.0;           // Calculate RS for MQ-135
    float mq135_ratio = mq135_RS / MQ135_R0;                       // Calculate ratio
    float mq135_ppm_log = (log10(mq135_ratio) - MQ135_b) / MQ135_m; // Calculate log(PPM)
    float mq135_ppm = pow(10, mq135_ppm_log);                       // Convert log value to PPM

    Serial.println("\n--- MQ-135 (Air Quality) ---");
    //Serial.print("Raw Value: ");
    //Serial.println(mq135_raw);
    //Serial.print("Voltage: ");
    //Serial.println(mq135_volt);
    Serial.print("PPM: ");
    Serial.println(mq135_ppm);
  }

  Serial.println("\n-----------------------------");
  delay(4000); // Delay for 2 seconds before the next reading
}
