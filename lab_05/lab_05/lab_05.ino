int motor1pin1 = 2;
int motor1pin2 = 3;

int motor2pin1 = 4;
int motor2pin2 = 5;

int rightWheel = 9;
int leftWheel = 10;

int triggerPinFront = 12;
int echoPinFront = 11;

int triggerPinLeft = 13;
int echoPinLeft = 8;

// PID algorithm
int error;
const float SET_POINT = 25; 
const float p = 1.0;

int SPEED;

long duration, cm, inches;

void setup() {
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(motor2pin1, OUTPUT);
  pinMode(motor2pin2, OUTPUT);

  pinMode(rightWheel, OUTPUT);
  pinMode(leftWheel, OUTPUT);

  pinMode(triggerPinFront, OUTPUT);
  pinMode(echoPinFront, INPUT);

  pinMode(triggerPinLeft, OUTPUT);
  pinMode(echoPinLeft, INPUT);

  Serial.begin(9600);  // Start serial communication
}

void loop() {
  triggerSensor();
  // triggerSensor(triggerPinLeft, echoPinLeft);
  pid();
  followFront();
  // followWall();
  printToSerial();
}

void pid() {
  error = cm - SET_POINT;
  SPEED = p * error;
  SPEED = constrain(SPEED, -255, 255);
}

void printToSerial() {
  Serial.print(cm);
  Serial.print("cm ");
  Serial.print(SPEED);
  Serial.println(" speed");
}

void triggerSensor() {
  digitalWrite(triggerPinFront, LOW);
  delayMicroseconds(5);
  digitalWrite(triggerPinFront, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPinFront, LOW);

  duration = pulseIn(echoPinFront, HIGH);

  cm = (duration / 2) / 29.1;    // Divide by 29.1 or multiply by 0.0343
}

void followFront() {
  if (SPEED >= 0) {
    forward();
  } else if (SPEED < 0) {
    backward();
  }
}

// void followWall() {
//   if (SPEED > 0) {
//     right();
//   } else if (SPEED < 0) {
//     left();
//   }
// }

void readControls() {
  if (Serial.available() > 0) {
    char input = Serial.read();
    Serial.print("input received: ");
    Serial.println(input);

    switch (input) {
      case 'w': forward(); break;
      case 's': backward(); break;
      case 'a': left(); break;
      case 'd': right(); break;
      default: stop(); break;
    }
  }
}

void setMotor(int enPin, int pin1, int pin2, int speed, bool forward) {
  analogWrite(enPin, speed);
  digitalWrite(pin1, forward ? HIGH : LOW);
  digitalWrite(pin2, forward ? LOW : HIGH);
}

void forward() {
  setMotor(rightWheel, motor1pin1, motor1pin2, SPEED, true);
  setMotor(leftWheel, motor2pin1, motor2pin2, SPEED, true);
}


void backward() {
  setMotor(rightWheel, motor1pin1, motor1pin2, SPEED, false);
  setMotor(leftWheel, motor2pin1, motor2pin2, SPEED, false);
}

void left() {
  setMotor(rightWheel, motor1pin1, motor1pin2, SPEED, true);
}

void right() {
  setMotor(leftWheel, motor2pin1, motor2pin2, SPEED, true);
}

void stop() {
  setMotor(rightWheel, motor1pin1, motor1pin2, 0, true);
  setMotor(leftWheel, motor2pin1, motor2pin2, 0, true);
}
