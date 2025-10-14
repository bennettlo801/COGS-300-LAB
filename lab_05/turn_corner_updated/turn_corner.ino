int motorApin1 = 3, motorApin2 = 2; // left motor
int motorBpin1 = 7, motorBpin2 = 4; // right motor
int motorA = 5, motorB = 6;

int encoderLeft = 8, encoderRight = 9;

int triggerPinFront = 10, echoPinFront = 11;
int triggerPinLeft = 10, echoPinLeft = 11;

// Running average filter
const int runningAverageCount = 16;
float RunningAverageBuffer[runningAverageCount];
int NextRunningAverage;

// PID algorithm
const float SET_POINT = 25;
const float p = 15;

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
  // forward(255);
  // followFront();
  followWall();
  // readControls();
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
  int output_map = map(output, -25, 275, -255, 255);

  printToSerial(distance, output);

  if (output >= 0) {
    forward(output);
  } else if (output < 0) {
    backward(output);
  }
}

void followWall() {
  // --- Tunable constants ---
  const int WALL_LOST_THRESHOLD = 45;   // cm, when wall "disappears"
  const int WALL_FOUND_THRESHOLD = 30;  // cm, when wall is detected again
  const int BASE_SPEED = 130;           // forward speed
  const int MAX_CORRECTION = 70;        // max steering correction
  const int TURN_SPEED = 120;           // pivot turn speed
  const int SENSOR_REFRESH_MS = 50;     // faster update rate
  const float margin = 2.0;

  static long prevDist = SET_POINT;     // for detecting sudden jumps

  long distance = triggerSensor(triggerPinLeft, echoPinLeft);
  int output_raw = pid(distance);
  int correction = constrain(map(output_raw, -200, 200, -MAX_CORRECTION, MAX_CORRECTION), -MAX_CORRECTION, MAX_CORRECTION);

  // === Detect wall loss quickly ===
  bool wallLost = (distance > WALL_LOST_THRESHOLD && (distance - prevDist) > 10);

  // --- Normal wall following ---
  if (!wallLost) {
    int leftSpeed  = constrain(BASE_SPEED - correction, 0, 255);
    int rightSpeed = constrain(BASE_SPEED + correction, 0, 255);

    analogWrite(motorA, leftSpeed);
    digitalWrite(motorApin1, HIGH);
    digitalWrite(motorApin2, LOW);

    analogWrite(motorB, rightSpeed);
    digitalWrite(motorBpin1, HIGH);
    digitalWrite(motorBpin2, LOW);

    Serial.print("Following wall, dist=");
    Serial.println(distance);
  }

  // --- Wall lost: immediate left turn ---
  else {
    stop();
    delay(100);
    Serial.println("Wall lost! Executing corner turn...");

    // Pivot left until wall reappears
    unsigned long startTime = millis();
    while (true) {
      left(TURN_SPEED);
      long newDist = triggerSensor(triggerPinLeft, echoPinLeft);
      Serial.print("Turning... dist=");
      Serial.println(newDist);

      if (newDist < WALL_FOUND_THRESHOLD) {
        Serial.println("Wall reacquired — resuming follow");
        stop();
        delay(150);
        break;
      }

      // Safety: stop after ~2 seconds to prevent endless spin
      if (millis() - startTime > 2000) {
        Serial.println("Timeout while turning — stopping");
        stop();
        break;
      }

      delay(SENSOR_REFRESH_MS);
    }
  }

  prevDist = distance;
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
  output = constrain(output, 0, 255);

  // Left wheel stopped
  analogWrite(motorA, 0);
  digitalWrite(motorApin1, LOW);
  digitalWrite(motorApin2, LOW);

  // Right wheel forward
  analogWrite(motorB, output);
  digitalWrite(motorBpin1, HIGH);
  digitalWrite(motorBpin2, LOW);

  Serial.println("turning left (left wheel frozen)");
}

void right(int output) {
  output = abs(output);
  output = constrain(output, 0, 255);

  // Right wheel stopped
  analogWrite(motorB, 0);
  digitalWrite(motorBpin1, LOW);
  digitalWrite(motorBpin2, LOW);

  // Left wheel forward
  analogWrite(motorA, output);
  digitalWrite(motorApin1, HIGH);
  digitalWrite(motorApin2, LOW);

  Serial.println("turning right (right wheel frozen)");
}


void stop() {
  analogWrite(motorB, 0);
  digitalWrite(motorBpin1, LOW);
  digitalWrite(motorBpin2, LOW);

  analogWrite(motorA, 0);
  digitalWrite(motorApin1, LOW);
  digitalWrite(motorApin2, LOW);
}
