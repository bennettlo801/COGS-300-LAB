// Motor pins & speed
int motorApin1 = 3, motorApin2 = 2; // left motor
int motorBpin1 = 7, motorBpin2 = 4; // right motor
int motorA = 5, motorB = 6;
const int SPEED = 70;

// IR Sensor
const int irSensorPinMiddle = 8;

void setup() {
  pinMode(motorApin1, OUTPUT), pinMode(motorApin2, OUTPUT);
  pinMode(motorBpin1, OUTPUT), pinMode(motorBpin2, OUTPUT);

  pinMode(motorA, OUTPUT), pinMode(motorB, OUTPUT);

  pinMode(irSensorPinMiddle, INPUT);
  Serial.begin(9600);
}

void loop() {
  int sensorValue = digitalRead(irSensorPinMiddle);  // Read the value from the IR sensor

  if (sensorValue == LOW) {
    Serial.println("Line");
    backward();
  } else {
    Serial.println("No line");
    forward();
  }

  delay(100);  // Small delay for stability
}

void forward() {
  analogWrite(motorB, SPEED);
  digitalWrite(motorBpin1, HIGH);
  digitalWrite(motorBpin2, LOW);

  analogWrite(motorA, SPEED);
  digitalWrite(motorApin1, HIGH);
  digitalWrite(motorApin2, LOW);
}

void backward() {
  analogWrite(motorB, SPEED);
  digitalWrite(motorBpin1, LOW);
  digitalWrite(motorBpin2, HIGH);

  analogWrite(motorA, SPEED);
  digitalWrite(motorApin1, LOW);
  digitalWrite(motorApin2, HIGH);
}
