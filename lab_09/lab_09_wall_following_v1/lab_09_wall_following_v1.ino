int motorApin1 = 3, motorApin2 = 2; // left motor
int motorBpin1 = 7, motorBpin2 = 4; // right motor
int motorA = 5, motorB = 6;

int encoderLeft = 8, encoderRight = 9;

int triggerPinFront = 12, echoPinFront = 13;
int triggerPinLeft = 10, echoPinLeft = 11;

// Running average filter
const int runningAverageCount = 16;
float RunningAverageBuffer[runningAverageCount];
int NextRunningAverage;

// PID algorithm
const float SET_POINT = 10;
const float p = 5;

// Ultrasonic sensor
int error;
long duration;

void setup() {
  pinMode(motorApin1, OUTPUT), pinMode(motorApin2, OUTPUT);
  pinMode(motorBpin1, OUTPUT), pinMode(motorBpin2, OUTPUT);

  pinMode(motorA, OUTPUT), pinMode(motorB, OUTPUT);

  pinMode(triggerPinFront, OUTPUT), pinMode(echoPinFront, INPUT);
  pinMode(triggerPinLeft, OUTPUT), pinMode(echoPinLeft, INPUT);

  Serial.begin(9600);
}

void loop() {
  followWall();
  delay(30);
}

float runningAverage(int distance) {
  int threshold = 300;
  float runningAverageDistance = 0;

  if (distance > threshold)
  {
    distance = threshold;
  }

  RunningAverageBuffer[NextRunningAverage++] = distance;

  if (NextRunningAverage >= runningAverageCount) {
    NextRunningAverage = 0;
  }

  for (int i = 0; i < runningAverageCount; ++i) {
    runningAverageDistance += RunningAverageBuffer[i];
  }
  return runningAverageDistance /= runningAverageCount;

  delay(100);
}

int pid(int distance) {
  float current = runningAverage(distance);
  float output = current - SET_POINT;

  output = constrain(output, -100, 100);
  return output;
}


int triggerSensor(int triggerPin, int echoPin) {
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(5);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  duration = pulseIn(echoPin, HIGH);

  float distance = (duration / 2) / 29.1;  // Divide by 29.1 or multiply by 0.0343

  return distance;
}

void followWall() {
  long distance = triggerSensor(triggerPinLeft, echoPinLeft);
  long clamped_distance = constrain(distance, 5, 15);
  int output = pid(clamped_distance);

  float leftSpeed = 120 - (output * 2);
  float rightSpeed = 120 + (output * 2);

  Serial.print(clamped_distance);
  Serial.print(" ");
  Serial.print(leftSpeed);
  Serial.print(" ");
  Serial.println(rightSpeed);

  forward(leftSpeed, rightSpeed);
}

void forward(int outputRight, int outputLeft) {
  analogWrite(motorA, outputRight);
  digitalWrite(motorApin1, HIGH);
  digitalWrite(motorApin2, LOW);

  analogWrite(motorB, outputLeft);
  digitalWrite(motorBpin1, HIGH);
  digitalWrite(motorBpin2, LOW);

  delay(25);
  stop();
}

void stop() {
  analogWrite(motorB, 0);
  digitalWrite(motorBpin1, LOW);
  digitalWrite(motorBpin2, LOW);

  analogWrite(motorA, 0);
  digitalWrite(motorApin1, LOW);
  digitalWrite(motorApin2, LOW);

  delay(50);
}
