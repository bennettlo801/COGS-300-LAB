const int motorApin1 = 3, motorApin2 = 2; // left motor
const int motorBpin1 = 7, motorBpin2 = 4; // right motor
const int motorA = 5, motorB = 6;

const int encoderLeft = 8, encoderRight = 9;

const int triggerPinFront = 10, echoPinFront = 11;
const int triggerPinLeft = 12, echoPinLeft = 13;

// Running average filter
const int runningAverageCount = 16;
float RunningAverageBuffer[runningAverageCount];
int NextRunningAverage;

// PID algorithm
const float SET_POINT = 25;
const float p = 4;
const int MIN_SPEED = 100;

// Ultrasonic sensor
int error;
long duration;
const int MAX_DISTANCE = 200;

void setup() {
  pinMode(motorApin1, OUTPUT), pinMode(motorApin2, OUTPUT);
  pinMode(motorBpin1, OUTPUT), pinMode(motorBpin2, OUTPUT);

  pinMode(motorA, OUTPUT), pinMode(motorB, OUTPUT);

  pinMode(triggerPinFront, OUTPUT), pinMode(echoPinFront, INPUT);
  pinMode(triggerPinLeft, OUTPUT), pinMode(echoPinLeft, INPUT);

  Serial.begin(9600);
  delay(1000);
}

void loop() {
  followFront();
  // followWall();
  // readControls();
}

void printToSerial(int distance, int output) {
  Serial.print(distance);
  Serial.print("cm ");
  Serial.print(output);
  Serial.println(" speed");
}

float runningAverage(int distance) {
  float runningAverageDistance = 0;

  RunningAverageBuffer[NextRunningAverage++] = distance;

  if (NextRunningAverage >= runningAverageCount) {
    NextRunningAverage = 0;
  }

  for (int i = 0; i < runningAverageCount; ++i) {
    runningAverageDistance += RunningAverageBuffer[i];
  }

  return runningAverageDistance /= runningAverageCount;
}

int pid(int distance) {
  error = runningAverage(distance) - SET_POINT;
  int output = p * error;

  return output;
}

int triggerSensor(int triggerPin, int echoPin) {
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(5);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  duration = pulseIn(echoPin, HIGH, 38000);

  float distance = duration * 0.034 / 2;

  if (distance > MAX_DISTANCE) distance = MAX_DISTANCE;
  if (distance == 0) distance = MAX_DISTANCE;

  return distance;
}

void followFront() {
  long distance = triggerSensor(triggerPinFront, echoPinFront);
  int output = pid(distance);

  if (abs(output) > 0 && abs(output) < MIN_SPEED) {
    output = MIN_SPEED;
  }

  if (output >= 0) {
    forward(output);
  } else if (output < 0) {
    backward(output);
  }
}

void followWall() {
  float margin = 1;
  long distance = triggerSensor(triggerPinLeft, echoPinLeft);
  int output = pid(distance);

  // if (abs(output) > 0 && abs(output) < MIN_SPEED) {
  //   output = MIN_SPEED;
  // }

  // if (output > (0 + margin)) {
  //   right(output);
  // } else if (output < (0 - margin)) {
  //   left(output);
  // }
}

void readControls() {

  if (Serial.available() > 0) {
    char input = Serial.read();
    Serial.print("input received: ");
    Serial.println(input);

    switch (input) {
      case 'w': forward(MIN_SPEED); break;
      case 's': backward(MIN_SPEED); break;
      case 'a': left(MIN_SPEED); break;
      case 'd': right(MIN_SPEED); break;
      // default: stop(); break;
    }
  }
}

void forward(int output) {

  analogWrite(motorB, output);
  digitalWrite(motorBpin1, HIGH);
  digitalWrite(motorBpin2, LOW);

  analogWrite(motorA, output);
  digitalWrite(motorApin1, HIGH);
  digitalWrite(motorApin2, LOW);

  // Serial.println("attempting forward");
}

void backward(int output) {
  output = abs(output);

  analogWrite(motorB, output);
  digitalWrite(motorBpin1, LOW);
  digitalWrite(motorBpin2, HIGH);

  analogWrite(motorA, output);
  digitalWrite(motorApin1, LOW);
  digitalWrite(motorApin2, HIGH);

  // Serial.println("attempting backward");

}

void left(int output) {
  output = abs(output);

  int output_map = map(output, 0, 255, 0, 255);

  analogWrite(motorA, output);
  digitalWrite(motorApin1, HIGH);
  digitalWrite(motorApin2, LOW);

  analogWrite(motorB, MIN_SPEED);
  digitalWrite(motorBpin1, HIGH);
  digitalWrite(motorBpin2, LOW);

  // Serial.println("attempting left");
}

void right(int output) {
  output = abs(output);

  int output_map = map(output, 0, 255, 255, 0);

  analogWrite(motorB, output_map);
  digitalWrite(motorBpin1, HIGH);
  digitalWrite(motorBpin2, LOW);

  analogWrite(motorA, MIN_SPEED);
  digitalWrite(motorApin1, HIGH);
  digitalWrite(motorApin2, LOW);

  // Serial.println("attempting right");
}

void stop() {
  analogWrite(motorB, 0);
  digitalWrite(motorBpin1, LOW);
  digitalWrite(motorBpin2, LOW);

  analogWrite(motorA, 0);
  digitalWrite(motorApin1, LOW);
  digitalWrite(motorApin2, LOW);
}
