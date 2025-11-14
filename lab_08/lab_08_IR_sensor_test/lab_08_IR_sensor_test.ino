// IR Sensor
const int irSensorPinMiddle = A0;

void setup() {
  pinMode(irSensorPinMiddle, INPUT);
  Serial.begin(9600);
}

void loop() {
  int sensorValue = digitalRead(irSensorPinMiddle);  // Read the value from the IR sensor

  if (sensorValue == LOW) {
    Serial.println("Obstacle detected");
  } else {
    Serial.println("No obstacle");
  }

  delay(100);  // Small delay for stability
}
