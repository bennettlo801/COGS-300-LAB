int motor1pin1 = 2;
int motor1pin2 = 3;

int motor2pin1 = 4;
int motor2pin2 = 5;

int enA = 9;    //right wheel
int enB = 10;   //left wheel

// wheel speeds
const int SPEED_NORMAL = 150;
const int SPEED_FAST = 225;
const int SPEED_SLOW = 125;

void setup()
{
    pinMode(motor1pin1, OUTPUT);
    pinMode(motor1pin2, OUTPUT);
    pinMode(motor2pin1, OUTPUT);
    pinMode(motor2pin2, OUTPUT);

    pinMode(enA, OUTPUT); // ENA
    pinMode(enB, OUTPUT); // ENB

    Serial.begin(9600); // Start serial communication
}

void loop() {
    if (Serial.available() > 0) {
        char input = Serial.read();
        Serial.print("input received: ");
        Serial.println(input);

        switch (input) {
            case 'w': forward(); break;
            case 's': backward(); break;
            case 'a': left(); break;
            case 'd': right(); break;
            case 'q': rotateLeft(); break;
            case 'e': rotateRight(); break;
            default:  stop(); break;
        }
    }
}

void setMotor(int enPin, int pin1, int pin2, int speed, bool forward) {
    analogWrite(enPin, speed);
    digitalWrite(pin1, forward ? HIGH : LOW);
    digitalWrite(pin2, forward ? LOW : HIGH);
}

void forward() {
    setMotor(enA, motor1pin1, motor1pin2, SPEED_NORMAL, true);
    setMotor(enB, motor2pin1, motor2pin2, SPEED_NORMAL, true);
}


void backward()
{
    setMotor(enA, motor1pin1, motor1pin2, SPEED_NORMAL, false);
    setMotor(enB, motor2pin1, motor2pin2, SPEED_NORMAL, false);
}

void left() {
    setMotor(enA, motor1pin1, motor1pin2, SPEED_FAST, true);
    setMotor(enB, motor2pin1, motor2pin2, SPEED_SLOW, true);
}


void right()
{
    setMotor(enA, motor1pin1, motor1pin2, SPEED_SLOW, true);
    setMotor(enB, motor2pin1, motor2pin2, SPEED_FAST, true);
}

void rotateLeft()
{
    setMotor(enA, motor1pin1, motor1pin2, SPEED_NORMAL, true);
    setMotor(enB, motor2pin1, motor2pin2, SPEED_NORMAL, false);
}

void rotateRight()
{
    setMotor(enA, motor1pin1, motor1pin2, SPEED_NORMAL, false);
    setMotor(enB, motor2pin1, motor2pin2, SPEED_NORMAL, true);
}

void stop()
{
    setMotor(enA, motor1pin1, motor1pin2, 0, true);
    setMotor(enB, motor2pin1, motor2pin2, 0, true);
}
