int motor1pin1 = 2, motor1pin2 = 3;
int motor2pin1 = 4, motor2pin2 = 5;
int rightWheel = 9, leftWheel = 10;

int triggerPinFront = 12, echoPinFront = 11;
int triggerPinLeft = 13, echoPinLeft = 8;

// Running average filter
const int runningAverageCount = 16;
float RunningAverageBuffer[runningAverageCount];
int NextRunningAverage;

// PID algorithm
const float SET_POINT = 25;
const float p = 1;

// Ultrasonic sensor
int error;
long duration;

void setup() {
  pinMode(motor1pin1, OUTPUT), pinMode(motor1pin2, OUTPUT);
  pinMode(motor2pin1, OUTPUT), pinMode(motor2pin2, OUTPUT);

  pinMode(rightWheel, OUTPUT), pinMode(leftWheel, OUTPUT);

  pinMode(triggerPinFront, OUTPUT), pinMode(echoPinFront, INPUT);
  pinMode(triggerPinLeft, OUTPUT), pinMode(echoPinLeft, INPUT);

  Serial.begin(9600);
}

void loop() {
  followFront();
  // followWall();
  // readControls();

  delay(10);
}

void printToSerial(int distance, int output) {
  Serial.print(distance);
  Serial.print("cm ");
  Serial.print(output);
  Serial.println(" speed");
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
  error = runningAverage(distance) - SET_POINT;
  int output = p * error;

  printToSerial(distance, output);

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

void followFront() {
  long distance = triggerSensor(triggerPinFront, echoPinFront);
  int output = pid(distance);
  // map(value, fromLow, fromHigh, toLow, toHigh)
  // output = map(output, )

  if (output >= 0) {
    forward(output);
  } else if (output < 0) {
    backward(output);
  }
}

void followWall() {
  long distance = triggerSensor(triggerPinLeft, echoPinLeft);
  int output = pid(distance);

  if (output > (0 + 5)) {
    right(output);
  } else if (output < (0 - 5)) {
    left(output);
  }
}

void readControls() {
  int SPEED = 150;

  if (Serial.available() > 0) {
    char input = Serial.read();
    Serial.print("input received: ");
    Serial.println(input);

    switch (input) {
      case 'w': forward(SPEED); break;
      case 's': backward(SPEED); break;
      case 'a': left(SPEED); break;
      case 'd': right(SPEED); break;
      default: stop(); break;
    }
  }
}

void forward(int output) {
  output = abs(output);

  analogWrite(leftWheel, output);
  digitalWrite(motor2pin1, HIGH);
  digitalWrite(motor2pin2, LOW);

  analogWrite(rightWheel, output);
  digitalWrite(motor1pin1, HIGH);
  digitalWrite(motor1pin2, LOW);

  // Serial.println("attempting forward");
}

void backward(int output) {
  output = abs(output);

  analogWrite(leftWheel, output);
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, HIGH);

  analogWrite(rightWheel, output);
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, HIGH);

  // Serial.println("attempting backward");

}

void left(int output) {
  output = abs(output);

  analogWrite(rightWheel, output);
  digitalWrite(motor1pin1, HIGH);
  digitalWrite(motor1pin2, LOW);

  analogWrite(leftWheel, 0);
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, LOW);

  // Serial.println("attempting left");
}

void right(int output) {
  output = abs(output);

  analogWrite(leftWheel, output);
  digitalWrite(motor2pin1, HIGH);
  digitalWrite(motor2pin2, LOW);

  analogWrite(rightWheel, 0);
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, LOW);

  // Serial.println("attempting right");
}

void stop() {
  analogWrite(leftWheel, 0);
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, LOW);

  analogWrite(rightWheel, 0);
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, LOW);
}
