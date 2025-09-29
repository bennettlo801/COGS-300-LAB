const int encoderPinA = 6, encoderPinB = 7;

void setup() {
  pinMode(encoderPinA, INPUT_PULLUP);   // use pullups; pressed/LOW = 0
  pinMode(encoderPinB, INPUT_PULLUP);
  Serial.begin(115200);
}

void loop() {
  int v1 = digitalRead(encoderPinA);    // 0 or 1
  int v2 = digitalRead(encoderPinB);    // 0 or 1
  Serial.print(v1); Serial.print(',');
  Serial.println(v2);          // newline-terminated
  delay(100);                    // ~200 Hz (adjust as needed)
}