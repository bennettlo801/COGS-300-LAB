#include <cmath>

// Encoder setup 
const int encoderRight = 6;
const int encoderLeft  = 7;

// encoder specs
const int ticks_per_rev = 40;
const float wheel_radius = 65.0 / 2; // mm

const float distance_per_rev  = 2.0 * M_PI * wheel_radius;
const float distance_per_tick = distance_per_rev / ticks_per_rev;

long ticksRight = 0;
long ticksLeft  = 0;

int lastRight = LOW;
int lastLeft  = LOW;

// Motor setup 
int motor1pin1 = 2;
int motor1pin2 = 3;

int motor2pin1 = 4;
int motor2pin2 = 5;

int enA = 9;    // right wheel
int enB = 10;   // left wheel

// wheel speeds
const int SPEED_NORMAL = 150;
const int SPEED_FAST   = 225;
const int SPEED_SLOW   = 125;

// Setup 
void setup() {
    // Encoder pins
    pinMode(encoderRight, INPUT_PULLUP);
    pinMode(encoderLeft, INPUT_PULLUP);

    lastRight = digitalRead(encoderRight);
    lastLeft  = digitalRead(encoderLeft);

    // Motor pins
    pinMode(motor1pin1, OUTPUT);
    pinMode(motor1pin2, OUTPUT);
    pinMode(motor2pin1, OUTPUT);
    pinMode(motor2pin2, OUTPUT);

    pinMode(enA, OUTPUT);
    pinMode(enB, OUTPUT);

    // Serial communication
    Serial.begin(115200);
}

// Loop 
void loop() {
    // 1️⃣ Handle keyboard commands
    if (Serial.available() > 0) {
        char input = Serial.read();
        Serial.print("input received: ");
        Serial.println(input);

        switch (input) {
            case 'w': forward();      break;
            case 's': backward();     break;
            case 'a': left();         break;
            case 'd': right();        break;
            case 'q': rotateLeft();   break;
            case 'e': rotateRight();  break;
            default:  stop();         break;
        }
    }

    // 2️⃣ Update encoder readings
    int v1 = digitalRead(encoderRight);
    int v2 = digitalRead(encoderLeft);

    if (v1 != lastRight) {
        ticksRight++;
        lastRight = v1;
    }
    if (v2 != lastLeft) {
        ticksLeft++;
        lastLeft = v2;
    }

    // 3️⃣ Compute distances
    float distanceRight = ticksRight * distance_per_tick;
    float distanceLeft  = ticksLeft  * distance_per_tick;

    // 4️⃣ Output telemetry
    Serial.print(distanceRight); Serial.print(",");
    Serial.println(distanceLeft);

    delay(10); // small delay to reduce serial spam
}

//  Motor functions 
void setMotor(int enPin, int pin1, int pin2, int speed, bool forward) {
    analogWrite(enPin, speed);
    digitalWrite(pin1, forward ? HIGH : LOW);
    digitalWrite(pin2, forward ? LOW : HIGH);
}

void forward() {
    setMotor(enA, motor1pin1, motor1pin2, SPEED_NORMAL, true);
    setMotor(enB, motor2pin1, motor2pin2, SPEED_NORMAL, true);
}

void backward() {
    setMotor(enA, motor1pin1, motor1pin2, SPEED_NORMAL, false);
    setMotor(enB, motor2pin1, motor2pin2, SPEED_NORMAL, false);
}

void left() {
    setMotor(enA, motor1pin1, motor1pin2, SPEED_FAST, true);
    setMotor(enB, motor2pin1, motor2pin2, SPEED_SLOW, true);
}

void right() {
    setMotor(enA, motor1pin1, motor1pin2, SPEED_SLOW, true);
    setMotor(enB, motor2pin1, motor2pin2, SPEED_FAST, true);
}

void rotateLeft() {
    setMotor(enA, motor1pin1, motor1pin2, SPEED_NORMAL, true);
    //setMotor(enB, motor2pin1, motor2pin2, SPEED_NORMAL, false);
}

void rotateRight() {
    //setMotor(enA, motor1pin1, motor1pin2, SPEED_NORMAL, false);
    setMotor(enB, motor2pin1, motor2pin2, SPEED_NORMAL, true);
}

void stop() {
    setMotor(enA, motor1pin1, motor1pin2, 0, true);
    setMotor(enB, motor2pin1, motor2pin2, 0, true);
}
