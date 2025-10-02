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

// For recording
struct Command {
  char type;
  long deltaTicksRight;
  long deltaTicksLeft;
};

Command currentCommand;
long startTicksRight = 0;
long startTicksLeft  = 0;
bool recordingCommand = false;

// Stop buffer for gaps
unsigned long stopStartTime = 0;
const unsigned long stopBuffer = 50; // ms

// Setup 
void setup() {
    pinMode(encoderRight, INPUT_PULLUP);
    pinMode(encoderLeft, INPUT_PULLUP);

    lastRight = digitalRead(encoderRight);
    lastLeft  = digitalRead(encoderLeft);

    pinMode(motor1pin1, OUTPUT);
    pinMode(motor1pin2, OUTPUT);
    pinMode(motor2pin1, OUTPUT);
    pinMode(motor2pin2, OUTPUT);

    pinMode(enA, OUTPUT);
    pinMode(enB, OUTPUT);

    Serial.begin(115200);
}

// Loop 
void loop() {
    // 1️⃣ Update encoder readings
    int v1 = digitalRead(encoderRight);
    int v2 = digitalRead(encoderLeft);

    if (v1 != lastRight) { ticksRight++; lastRight = v1; }
    if (v2 != lastLeft)  { ticksLeft++;  lastLeft  = v2; }

    // 2️⃣ Handle keyboard commands
    if (Serial.available() > 0) {
        char input = Serial.read();
        Serial.print("input received: "); Serial.println(input);

        if (input == 'w' || input == 'q' || input == 'e') {
            startRecordingCommand(input);
        } else {
            stopMotors();
        }
    }

    // 3️⃣ If recording a command, update delta ticks
    if (recordingCommand) {
        currentCommand.deltaTicksRight = ticksRight - startTicksRight;
        currentCommand.deltaTicksLeft  = ticksLeft  - startTicksLeft;

        // For demonstration, output current deltas
        Serial.print(currentCommand.type); Serial.print(",");
        Serial.print(currentCommand.deltaTicksRight); Serial.print(",");
        Serial.println(currentCommand.deltaTicksLeft);
    }

    // 4️⃣ Stop buffer handling (gaps)
    if (!recordingCommand && stopStartTime > 0 && millis() - stopStartTime >= stopBuffer) {
        stopStartTime = 0; // reset buffer
    }
}

void startRecordingCommand(char cmd) {
    currentCommand.type = cmd;
    startTicksRight = ticksRight;
    startTicksLeft  = ticksLeft;
    recordingCommand = true;

    switch (cmd) {
        case 'w': forward(); break;
        case 'q': left();    break;
        case 'e': right();   break;
    }
}

void stopMotors() {
    if (recordingCommand) {
        // Log the final ticks here or send to Processing
        Serial.print("REC,"); // marker
        Serial.print(currentCommand.type); Serial.print(",");
        Serial.print(currentCommand.deltaTicksRight); Serial.print(",");
        Serial.println(currentCommand.deltaTicksLeft);
    }

    setMotor(enA, motor1pin1, motor1pin2, 0, true);
    setMotor(enB, motor2pin1, motor2pin2, 0, true);

    recordingCommand = false;
    stopStartTime = millis();
}

// Motor functions 
void setMotor(int enPin, int pin1, int pin2, int speed, bool forward) {
    analogWrite(enPin, speed);
    digitalWrite(pin1, forward ? HIGH : LOW);
    digitalWrite(pin2, forward ? LOW : HIGH);
}

void forward() {
    setMotor(enA, motor1pin1, motor1pin2, SPEED_NORMAL, true);
    setMotor(enB, motor2pin1, motor2pin2, SPEED_NORMAL, true);
}

void left() {
    setMotor(enA, motor1pin1, motor1pin2, SPEED_NORMAL, true);
    //setMotor(enB, motor2pin1, motor2pin2, SPEED_NORMAL, false);
}

void right() {
    //setMotor(enA, motor1pin1, motor1pin2, SPEED_NORMAL, false);
    setMotor(enB, motor2pin1, motor2pin2, SPEED_NORMAL, true);
}
