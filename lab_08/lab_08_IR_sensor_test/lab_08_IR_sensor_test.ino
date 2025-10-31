// IR Sensor
const int irSensorPin = 8;

void setup() {
  pinMode(motorApin1, OUTPUT), pinMode(motorApin2, OUTPUT);
  pinMode(motorBpin1, OUTPUT), pinMode(motorBpin2, OUTPUT);

  pinMode(motorA, OUTPUT), pinMode(motorB, OUTPUT);

  pinMode(triggerPinFront, OUTPUT), pinMode(echoPinFront, INPUT);
  pinMode(triggerPinLeft, OUTPUT), pinMode(echoPinLeft, INPUT);

  pinMode(irSensorPin, INPUT);
  Serial.begin(9600);
}

void loop() {
  int sensorValue = digitalRead(irSensorPin);  // Read the value from the IR sensor

  if (sensorValue == LOW) {
    Serial.println("Obstacle detected");
  } else {
    Serial.println("No obstacle");
  }

  delay(100);  // Small delay for stability
}
