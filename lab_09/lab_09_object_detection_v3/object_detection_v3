#include <Servo.h>
#include <math.h>    // for exp()
#include <string.h>  // for strcmp()

Servo myservo;

// Servo parameters
int servoPosition = 0;
int incrementServo = 5;
unsigned long lastUpdate = 0;
unsigned long updateInterval = 60;  // sweep speed

// Bayesian filter
const int num_positions = 5;
float  readings[num_positions]             = {0, 0, 0, 0, 0};
double beliefs_array[num_positions]        = {0.2, 0.2, 0.2, 0.2, 0.2};
float  multiplied_readings[num_positions]  = {0, 0, 0, 0, 0};

// Ultrasonic sensor pins
int triggerPin = 12, echoPin = 13;
long duration;
float cm;
const float MAX_DISTANCE = 150.0; // detection range

// Bayesian parameters
const float EPSILON = 0.01;
const float ALPHA   = 0.8;  // trust new evidence more

// Motor pins & speed
int motorApin1 = 3, motorApin2 = 2; // left motor direction
int motorBpin1 = 7, motorBpin2 = 4; // right motor direction
int motorA = 5, motorB = 6;         // PWM enable pins
const int SPEED = 80;               // drive speed

// Motor timing
unsigned long motorStartTime = 0;
unsigned long motorDuration  = 0;
bool motorActive = false;

// --- Function Prototypes ---
void  sweepServo();
void  updateBeliefs();
float bestAngle(float &confidence);
void  printBeliefs();
void  recordPositions();
float triggerSensor();
void  faceObject(float angle, float confidence);
void  moveMotor(const char* direction, unsigned long duration);
void  updateMotors();
void  stopMotors();
void  resetReadings();

void setup() {
  myservo.attach(9);
  myservo.write(servoPosition); // start at 0°

  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(motorApin1, OUTPUT);
  pinMode(motorApin2, OUTPUT);
  pinMode(motorBpin1, OUTPUT);
  pinMode(motorBpin2, OUTPUT);

  pinMode(motorA, OUTPUT);
  pinMode(motorB, OUTPUT);

  stopMotors(); // ensure motors are off

  Serial.begin(9600);
  Serial.println("Starting object detection robot...");
}

void loop() {
  // Only scan when motors are NOT moving
  if (!motorActive) {
    sweepServo();
  }
  // Always keep track of whether a move has finished
  updateMotors();
}

// Sweep servo and update beliefs at each end
void sweepServo() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastUpdate >= updateInterval) {
    lastUpdate = currentMillis;

    // Move servo by increment
    servoPosition += incrementServo;

    bool atEnd = false;

    // Constrain and detect endpoints
    if (servoPosition >= 180) {
      servoPosition = 180;
      incrementServo = -incrementServo; // reverse direction
      atEnd = true;
    } else if (servoPosition <= 0) {
      servoPosition = 0;
      incrementServo = -incrementServo; // reverse direction
      atEnd = true;
    }

    myservo.write(servoPosition);
    recordPositions(); // record at this position if multiple of 45°

    if (atEnd) {
      // We completed a pass to one side
      Serial.println("=== End of sweep, updating beliefs ===");
      updateBeliefs();

      float confidence = 0.0;
      float objectAngle = bestAngle(confidence);  // weighted average angle

      Serial.print("Most likely object angle: ");
      Serial.print(objectAngle, 1);
      Serial.print("°  | Confidence: ");
      Serial.println(confidence, 3);

      printBeliefs();

      // Decide movement based on belief
      faceObject(objectAngle, confidence);

      // Reset readings for next sweep
      resetReadings();
    }
  }
}

// Only drive straight when object is in front.
// Otherwise rotate left/right to get it to the centre.
void faceObject(float angle, float confidence) {

  if (confidence < 0.30) { // only move if fairly confident
    Serial.println("Low confidence — holding position.");
    stopMotors();
    return;
  }

  Serial.print("Tracking object at ");
  Serial.print(angle, 1);
  Serial.println("°");

  // Treat 90° as "straight ahead"
  // Simple zones:
  // [0, 60)   = RIGHT side
  // [60,120]  = FRONT
  // (120,180] = LEFT side

  if (angle < 60.0f) {
    // Object is to the RIGHT → rotate RIGHT
    Serial.println("Object on RIGHT -> turning RIGHT");
    moveMotor("RIGHT", 300);
  }
  else if (angle > 120.0f) {
    // Object is to the LEFT → rotate LEFT
    Serial.println("Object on LEFT -> turning LEFT");
    moveMotor("LEFT", 300);
  }
  else {
    // Object roughly in front → drive FORWARD
    Serial.println("Object in FRONT -> driving FORWARD");
    moveMotor("FORWARD", 500);
  }
}

// Non-blocking motor control
void moveMotor(const char* direction, unsigned long duration) {
  motorActive    = true;
  motorStartTime = millis();
  motorDuration  = duration;

  if (strcmp(direction, "FORWARD") == 0) {
    // both motors forward
    analogWrite(motorA, SPEED);
    digitalWrite(motorApin1, HIGH);
    digitalWrite(motorApin2, LOW);

    analogWrite(motorB, SPEED);
    digitalWrite(motorBpin1, HIGH);
    digitalWrite(motorBpin2, LOW);
  } 
  // NOTE: LEFT and RIGHT blocks have been SWAPPED here
  else if (strcmp(direction, "LEFT") == 0) {
    // this was previously RIGHT -- now LEFT for your wiring
    // spin LEFT: left wheel forward, right wheel backward (for your robot)
    analogWrite(motorA, SPEED);
    digitalWrite(motorApin1, LOW);   // left wheel forward
    digitalWrite(motorApin2, HIGH);

    analogWrite(motorB, SPEED);
    digitalWrite(motorBpin1, HIGH);    // right wheel backward
    digitalWrite(motorBpin2, LOW);
  } 
  else if (strcmp(direction, "RIGHT") == 0) {
    // this was previously LEFT -- now RIGHT for your wiring
    // spin RIGHT: left wheel backward, right wheel forward (for your robot)
    analogWrite(motorA, SPEED);
    digitalWrite(motorApin1, HIGH);    // left wheel backward
    digitalWrite(motorApin2, LOW);

    analogWrite(motorB, SPEED);
    digitalWrite(motorBpin1, LOW);   // right wheel forward
    digitalWrite(motorBpin2, HIGH);
  }
}

// Continuously check if movement duration has ended
void updateMotors() {
  if (motorActive && (millis() - motorStartTime >= motorDuration)) {
    stopMotors();
    motorActive = false;
    Serial.println("Movement finished, resuming sweep.");
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
  int index = servoPosition / 45;
  if (abs(servoPosition - index*45) < 3) {
    if (index >= 0 && index < num_positions) {
      readings[index] = triggerSensor();
      Serial.print("Angle ");
      Serial.print(servoPosition);
      Serial.print("° -> ");
      Serial.print(readings[index]);
      Serial.println(" cm");
    }
  }
}

// Ultrasonic sensor read with averaging
float triggerSensor() {
  const int samples = 5;
  float total = 0;
  int valid = 0;

  for (int i = 0; i < samples; i++) {
    digitalWrite(triggerPin, LOW);
    delayMicroseconds(5);
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(triggerPin, LOW);

    duration = pulseIn(echoPin, HIGH, 30000); // increase timeout
    float dist = (duration / 2.0) / 29.1;

    // Reject invalid/noisy samples
    if (duration == 0) continue;      // timeout/no echo
    if (dist < 5) continue;           // reject suspiciously small values
    if (dist > MAX_DISTANCE) dist = MAX_DISTANCE;

    total += dist;
    valid++;
    delay(5);
  }

  if (valid == 0) return MAX_DISTANCE;  // treat as "nothing detected"
  return total / valid;
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

// Return weighted-average best angle + confidence
float bestAngle(float &confidence) {
  float maxVal = 0.0, secondVal = 0.0;
  float angleSum = 0.0;
  float totalBelief = 0.0;

  for (int i = 0; i < num_positions; i++) {
    float b = beliefs_array[i];
    float angle = i * 45.0;

    // track max and second max for confidence
    if (b > maxVal) {
      secondVal = maxVal;
      maxVal = b;
    } else if (b > secondVal) {
      secondVal = b;
    }

    angleSum    += b * angle;
    totalBelief += b;
  }

  confidence = maxVal - secondVal;  // difference-based confidence

  if (totalBelief <= 0.0) {
    return 90.0; // default straight ahead
  }

  // Weighted mean angle
  return angleSum / totalBelief;
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

// Clear readings for next sweep
void resetReadings() {
  for (int i = 0; i < num_positions; i++) {
    readings[i] = MAX_DISTANCE;  // treat as "far / no object"
  }
}
