#include <Servo.h>

Servo myservo;

// Servo parameters
int servoPosition = 0;
int incrementServo = 5;
unsigned long lastUpdate = 0;
unsigned long updateInterval = 40;

// Bayesian filter
const int num_positions = 5;
float readings[num_positions] = {0, 0, 0, 0, 0};
double beliefs_array[num_positions] = {0.2, 0.2, 0.2, 0.2, 0.2};
float multiplied_readings[num_positions] = {0, 0, 0, 0, 0};

// Ultrasonic sensor pins
int triggerPin = 12, echoPin = 13;
long duration;
float cm;
const float MAX_DISTANCE = 100.0; // more sensitive within shorter range

// Bayesian parameters
const float EPSILON = 0.01;
const float ALPHA = 0.8; // trust new evidence more

// Motor pins & speed
int motorApin1 = 3, motorApin2 = 2; // left motor
int motorBpin1 = 7, motorBpin2 = 4; // right motor
int motorA = 5, motorB = 6;
const int SPEED = 100;

// Motor timing
unsigned long motorStartTime = 0;
unsigned long motorDuration = 0;
bool motorActive = false;
String motorDirection = "";

// --- Function Prototypes ---
void sweepServo();
void updateBeliefs();
int bestBelief(float &confidence);
void printBeliefs();
void recordPositions();
float triggerSensor();
void faceObject(int angle, float confidence);
void moveMotor(String direction, unsigned long duration);
void updateMotors();
void stopMotors();

void setup() {
  myservo.attach(9);

  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(motorApin1, OUTPUT);
  pinMode(motorApin2, OUTPUT);
  pinMode(motorBpin1, OUTPUT);
  pinMode(motorBpin2, OUTPUT);

  pinMode(motorA, OUTPUT);
  pinMode(motorB, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  sweepServo();
  updateMotors();
}

// Sweep servo and update beliefs at each end
void sweepServo() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastUpdate >= updateInterval) {
    lastUpdate = currentMillis;

    servoPosition += incrementServo;
    myservo.write(servoPosition);
    recordPositions();

    if (servoPosition >= 180 || servoPosition <= 0) {
      incrementServo = -incrementServo;
      updateBeliefs(); // Bayesian update after sweep

      float confidence = 0.0;
      int objectAngle = bestBelief(confidence) * 45;
      Serial.print("Most likely object angle: ");
      Serial.print(objectAngle);
      Serial.print("°  | Confidence: ");
      Serial.println(confidence, 2);

      faceObject(objectAngle, confidence);
      printBeliefs();
    }
  }
}

// Turn robot toward object based on servo angle
void faceObject(int angle, float confidence) {
  if (confidence < 0.30) { // only move if fairly confident
    Serial.println("Low confidence — holding position.");
    stopMotors();
    return;
  }

  switch(angle) {
    case 0:   moveMotor("RIGHT", 200); break;
    case 45:  moveMotor("RIGHT", 150); break;
    case 90:  moveMotor("FORWARD", 300); break;
    case 135: moveMotor("LEFT", 150); break;
    case 180: moveMotor("LEFT", 200); break;
  }
}

// Non-blocking motor control
void moveMotor(String direction, unsigned long duration) {
  motorActive = true;
  motorDirection = direction;
  motorStartTime = millis();
  motorDuration = duration;

  if (direction == "FORWARD") {
    analogWrite(motorA, SPEED);
    digitalWrite(motorApin1, HIGH);
    digitalWrite(motorApin2, LOW);

    analogWrite(motorB, SPEED);
    digitalWrite(motorBpin1, HIGH);
    digitalWrite(motorBpin2, LOW);
  } 
  else if (direction == "LEFT") {
    analogWrite(motorA, SPEED);
    digitalWrite(motorApin1, LOW);
    digitalWrite(motorApin2, HIGH);

    analogWrite(motorB, SPEED);
    digitalWrite(motorBpin1, HIGH);
    digitalWrite(motorBpin2, LOW);
  } 
  else if (direction == "RIGHT") {
    analogWrite(motorA, SPEED);
    digitalWrite(motorApin1, HIGH);
    digitalWrite(motorApin2, LOW);

    analogWrite(motorB, SPEED);
    digitalWrite(motorBpin1, LOW);
    digitalWrite(motorBpin2, HIGH);
  }
}

// Continuously check if movement duration has ended
void updateMotors() {
  if (motorActive && millis() - motorStartTime >= motorDuration) {
    stopMotors();
    motorActive = false;
  }
}

// Stop both motors
void stopMotors() {
  analogWrite(motorA, 0);
  digitalWrite(motorApin1, LOW);
  digitalWrite(motorApin2, LOW);

  analogWrite(motorB, 0);
  digitalWrite(motorBpin1, LOW);
  digitalWrite(motorBpin2, LOW);
}

// Record distance at current servo angle (every 45 degrees)
void recordPositions() {
  if (servoPosition % 45 == 0) {
    int index = servoPosition / 45;
    readings[index] = triggerSensor();
  }
}

// Ultrasonic sensor read with averaging
float triggerSensor() {
  const int samples = 5;
  float total = 0;

  for (int i = 0; i < samples; i++) {
    digitalWrite(triggerPin, LOW);
    delayMicroseconds(5);
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(triggerPin, LOW);

    duration = pulseIn(echoPin, HIGH, 20000); // 20ms timeout
    float dist = (duration / 2) / 29.1;
    if (dist >= MAX_DISTANCE) dist = MAX_DISTANCE;
    else if (dist <= 5) dist = 5; // prevents extreme 0s
    total += dist;
    delay(10);
  }
  return total / samples;
}

// Bayesian update using sharper likelihood function
void updateBeliefs() {
  float sum = 0.0;

  for (int i = 0; i < num_positions; i++) {
    float normalized = readings[i] / MAX_DISTANCE;
    if (normalized > 1.0) normalized = 1.0;

    // Stronger exponential weighting for close objects
    float likelihood = exp(-normalized * 6.0);

    multiplied_readings[i] = likelihood * beliefs_array[i];
    multiplied_readings[i] = max(multiplied_readings[i], EPSILON);
    sum += multiplied_readings[i];
  }

  for (int i = 0; i < num_positions; i++) {
    float posterior = multiplied_readings[i] / sum;
    beliefs_array[i] = ALPHA * posterior + (1 - ALPHA) * (1.0 / num_positions);
  }
}

// Return index of most likely object angle + confidence
int bestBelief(float &confidence) {
  float maxVal = 0.0, secondVal = 0.0;
  int maxPos = 0;

  for (int i = 0; i < num_positions; i++) {
    if (beliefs_array[i] > maxVal) {
      secondVal = maxVal;
      maxVal = beliefs_array[i];
      maxPos = i;
    } else if (beliefs_array[i] > secondVal) {
      secondVal = beliefs_array[i];
    }
  }

  confidence = maxVal - secondVal; // difference-based confidence
  return maxPos;
}

// Print beliefs
void printBeliefs() {
  Serial.println("Beliefs:");
  for (int i = 0; i < num_positions; i++) {
    Serial.print(i * 45);
    Serial.print("°: ");
    Serial.println(beliefs_array[i], 3);
  }
  Serial.println("------");
}
