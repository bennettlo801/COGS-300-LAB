// =====================================================
// Pin Definitions
// =====================================================
int motorApin1 = 3, motorApin2 = 2; // left motor
int motorBpin1 = 7, motorBpin2 = 4; // right motor
int motorA = 5, motorB = 6;

int encoderLeft = 8, encoderRight = 9;

int triggerPinFront = 12, echoPinFront = 13;
int triggerPinLeft = 10, echoPinLeft = 11;


// =====================================================
// Running Average Buffer
// =====================================================
const int runningAverageCount = 16;
float RunningAverageBuffer[runningAverageCount];
int NextRunningAverage = 0;


// =====================================================
// PID Settings
// =====================================================
const float SET_POINT = 10;
const float p = 5;


// =====================================================
// Motor Pulse Timing State Machine
// =====================================================
enum MotorState { IDLE, RUNNING, STOPPED };
MotorState motorState = IDLE;

unsigned long motorTimestamp = 0;

const unsigned long motorRunDuration  = 25;  // (was delay(25))
const unsigned long motorStopDuration = 50;  // (was delay(50))


// =====================================================
// Setup
// =====================================================
void setup() {
  pinMode(motorApin1, OUTPUT);
  pinMode(motorApin2, OUTPUT);
  pinMode(motorBpin1, OUTPUT);
  pinMode(motorBpin2, OUTPUT);
  pinMode(motorA, OUTPUT);
  pinMode(motorB, OUTPUT);

  pinMode(triggerPinFront, OUTPUT);
  pinMode(echoPinFront, INPUT);
  pinMode(triggerPinLeft, OUTPUT);
  pinMode(echoPinLeft, INPUT);

  Serial.begin(9600);
}


// =====================================================
// Main Loop (Non-blocking)
// =====================================================
void loop() {

  unsigned long now = millis();

  // Update motor state machine
  updateMotorState(now);

  // Only update steering/sensors when motor is ready for a new pulse
  if (motorState == IDLE) {
    followWall();
  }
}


// =====================================================
// Running Average Filter
// =====================================================
float runningAverage(int distance) {

  const int threshold = 300;
  if (distance > threshold) distance = threshold;

  RunningAverageBuffer[NextRunningAverage++] = distance;
  if (NextRunningAverage >= runningAverageCount)
    NextRunningAverage = 0;

  float sum = 0;
  for (int i = 0; i < runningAverageCount; i++)
    sum += RunningAverageBuffer[i];

  return sum / runningAverageCount;
}


// =====================================================
// PID
// =====================================================
int pid(int distance) {
  float current = runningAverage(distance);
  float output = current - SET_POINT;

  return constrain(output, -100, 100);
}


// =====================================================
// Ultrasonic Sensor Trigger
// =====================================================
int triggerSensor(int triggerPin, int echoPin) {
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(5);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  long duration = pulseIn(echoPin, HIGH);

  float distance = (duration / 2) / 29.1;

  return distance;
}


// =====================================================
// Wall-following logic
// =====================================================
void followWall() {

  long distance = triggerSensor(triggerPinLeft, echoPinLeft);
  long clamped_distance = constrain(distance, 5, 15);

  int output = pid(clamped_distance);

  float leftSpeed  = 100 - (output * 2);
  float rightSpeed = 100 + (output * 2);

  Serial.print(clamped_distance);
  Serial.print("  ");
  Serial.print(leftSpeed);
  Serial.print("  ");
  Serial.println(rightSpeed);

  forward(rightSpeed, leftSpeed);
}


// =====================================================
// Motor State Machine Logic
// =====================================================
void updateMotorState(unsigned long now) {

  switch (motorState) {

    case RUNNING:
      if (now - motorTimestamp >= motorRunDuration) {
        stopMotors();
        motorTimestamp = now;
        motorState = STOPPED;
      }
      break;

    case STOPPED:
      if (now - motorTimestamp >= motorStopDuration) {
        motorState = IDLE;  // ready for next pulse
      }
      break;

    case IDLE:
      // followWall() will initiate the next forward() pulse
      break;
  }
}


// =====================================================
// Motor Start Pulse
// =====================================================
void forward(int outputRight, int outputLeft) {

  // Right motor
  analogWrite(motorA, outputRight);
  digitalWrite(motorApin1, HIGH);
  digitalWrite(motorApin2, LOW);

  // Left motor
  analogWrite(motorB, outputLeft);
  digitalWrite(motorBpin1, HIGH);
  digitalWrite(motorBpin2, LOW);

  motorTimestamp = millis();
  motorState = RUNNING;  // start the 25 ms run window
}


// =====================================================
// Motor Stop Pulse
// =====================================================
void stopMotors() {

  analogWrite(motorA, 0);
  digitalWrite(motorApin1, LOW);
  digitalWrite(motorApin2, LOW);

  analogWrite(motorB, 0);
  digitalWrite(motorBpin1, LOW);
  digitalWrite(motorBpin2, LOW);
}
