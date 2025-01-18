#define MQ7PIN 34    // GPIO pin for MQ-7 sensor
#define MQ135PIN 35  // GPIO pin for MQ-135 sensor

void setup() {
  Serial.begin(115200);  // Start serial communication
  pinMode(MQ7PIN, INPUT);
  pinMode(MQ135PIN, INPUT);
  Serial.println("Starting Calibration for MQ-7 and MQ-135...");
}

void loop() {
  float sensorValueMQ7 = 0.0;   // Variable to hold the MQ-7 readings
  float sensorValueMQ135 = 0.0; // Variable to hold the MQ-135 readings
  float sensor_volt_MQ7, sensor_volt_MQ135, RS_air_MQ7, RS_air_MQ135, R0_MQ7, R0_MQ135;
  int totalReadings = 1000;  // Number of readings for averaging

  Serial.print("Calibrating MQ-7 and MQ-135: [");  // Start the loading bar

  // Take multiple readings for both sensors and average them
  for (int i = 0; i < totalReadings; i++) {
    sensorValueMQ7 += analogRead(MQ7PIN);
    sensorValueMQ135 += analogRead(MQ135PIN);

    // Update the loading bar progress
    if (i % (totalReadings / 20) == 0) {  // Update every 5% (1/20th of total readings)
      Serial.print("=");
    }

    delay(25);  // Small delay for stable readings
  }

  Serial.println("]");  // End the loading bar

  // Calculate averages
  sensorValueMQ7 /= totalReadings;
  sensorValueMQ135 /= totalReadings;

  // Convert the raw values to voltages
  sensor_volt_MQ7 = sensorValueMQ7 * (3.3 / 4095.0);  // ESP32 ADC is 12-bit
  sensor_volt_MQ135 = sensorValueMQ135 * (3.3 / 4095.0);

  // Calculate RS_air (Sensor resistance in fresh air) for both sensors
  RS_air_MQ7 = ((3.3 * 10.0) / sensor_volt_MQ7) - 10.0;  // RL = 10kΩ
  RS_air_MQ135 = ((3.3 * 10.0) / sensor_volt_MQ135) - 10.0;

  // Calculate R0 (Baseline resistance in fresh air)
  R0_MQ7 = RS_air_MQ7 / 27.0;  // 27 is the RS/R0 ratio for MQ-7
  R0_MQ135 = RS_air_MQ135 / 3.7;  // 3.7 is the RS/R0 ratio for MQ-135

  // Print results for MQ-7
  Serial.println("\n--- MQ-7 Calibration ---");
  Serial.print("Average Raw Value (MQ-7): ");
  Serial.println(sensorValueMQ7);
  Serial.print("Sensor Voltage (MQ-7) [V]: ");
  Serial.println(sensor_volt_MQ7);
  Serial.print("RS_air (MQ-7) [kΩ]: ");
  Serial.println(RS_air_MQ7);
  Serial.print("R0 (MQ-7) [kΩ]: ");
  Serial.println(R0_MQ7);

  // Print results for MQ-135
  Serial.println("\n--- MQ-135 Calibration ---");
  Serial.print("Average Raw Value (MQ-135): ");
  Serial.println(sensorValueMQ135);
  Serial.print("Sensor Voltage (MQ-135) [V]: ");
  Serial.println(sensor_volt_MQ135);
  Serial.print("RS_air (MQ-135) [kΩ]: ");
  Serial.println(RS_air_MQ135);
  Serial.print("R0 (MQ-135) [kΩ]: ");
  Serial.println(R0_MQ135);

  Serial.println("\nCalibration complete! Note the R0 values above for future use.");
  delay(10000);  // Wait 10 seconds before restarting
}
