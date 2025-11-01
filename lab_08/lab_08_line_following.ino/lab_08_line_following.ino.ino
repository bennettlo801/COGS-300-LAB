// Motor pins & speed
int motorApin1 = 3, motorApin2 = 2; // left motor
int motorBpin1 = 7, motorBpin2 = 4; // right motor
int motorA = 5, motorB = 6;
const int SPEED = 50;

// IR Sensor
const int irSensorPinMiddle = A0;
const int irSensorPinLeft = A1;
const int irSensorPinRight = A2;

void setup()
{
  pinMode(motorApin1, OUTPUT), pinMode(motorApin2, OUTPUT);
  pinMode(motorBpin1, OUTPUT), pinMode(motorBpin2, OUTPUT);

  pinMode(motorA, OUTPUT), pinMode(motorB, OUTPUT);

  pinMode(irSensorPinMiddle, INPUT);
  pinMode(irSensorPinLeft, INPUT);
  pinMode(irSensorPinRight, INPUT);

  Serial.begin(9600);
}

void loop()
{
  int sensorValueMiddle = digitalRead(irSensorPinMiddle);
  int sensorValueLeft = digitalRead(irSensorPinLeft);
  int sensorValueRight = digitalRead(irSensorPinRight);

  if (sensorValueLeft == LOW && sensorValueRight == HIGH)
  {
    // Serial.println("Left sensor on line");
    left();
  }
  else if (sensorValueRight == LOW && sensorValueLeft == HIGH)
  {
    // Serial.println("Right sensor on line");
    right();
  }
  else
  {
    followMiddle(sensorValueMiddle);
  }
}

void followMiddle(int &sensorValueMiddle)
{
  if (sensorValueMiddle == LOW)
  {
    // Serial.println("Line");
    stop();
    forwardLeft();
  }
  else
  {
    // Serial.println("No line");
    forwardRight();
  }
}

void forwardLeft()
{
  analogWrite(motorB, SPEED);
  digitalWrite(motorBpin1, HIGH);
  digitalWrite(motorBpin2, LOW);

  analogWrite(motorA, SPEED * 0.75);
  digitalWrite(motorApin1, HIGH);
  digitalWrite(motorApin2, LOW);
}

void forwardRight()
{
  analogWrite(motorB, SPEED * 0.75);
  digitalWrite(motorBpin1, HIGH);
  digitalWrite(motorBpin2, LOW);

  analogWrite(motorA, SPEED);
  digitalWrite(motorApin1, HIGH);
  digitalWrite(motorApin2, LOW);
}

void left()
{
  analogWrite(motorB, SPEED);
  digitalWrite(motorBpin1, HIGH);
  digitalWrite(motorBpin2, LOW);

  analogWrite(motorA, 0);
  digitalWrite(motorApin1, HIGH);
  digitalWrite(motorApin2, LOW);
}

void right()
{
  analogWrite(motorB, 0);
  digitalWrite(motorBpin1, HIGH);
  digitalWrite(motorBpin2, LOW);

  analogWrite(motorA, SPEED);
  digitalWrite(motorApin1, HIGH);
  digitalWrite(motorApin2, LOW);
}

void stop()
{
  analogWrite(motorA, 0);
  digitalWrite(motorApin1, LOW);
  digitalWrite(motorApin2, LOW);

  analogWrite(motorB, 0);
  digitalWrite(motorBpin1, LOW);
  digitalWrite(motorBpin2, LOW);
}