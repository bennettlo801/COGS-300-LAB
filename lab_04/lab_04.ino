const int encoderPinA = 2;   // Must be interrupt-capable
const int encoderPinB = 3;   // Must be interrupt-capable

volatile long encoderTicks = 0; 
int lastEncoded = 0;

void setup() {
  pinMode(encoderPinA, INPUT_PULLUP);
  pinMode(encoderPinB, INPUT_PULLUP);
  Serial.begin(115200);

  // Attach interrupts to both channels
  attachInterrupt(digitalPinToInterrupt(encoderPinA), updateEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPinB), updateEncoder, CHANGE);
}

void loop() {
  static unsigned long lastPrint = 0;
  unsigned long now = millis();

  // Print ticks every 100ms
  if (now - lastPrint >= 100) {
    lastPrint = now;
    noInterrupts();
    long ticks = encoderTicks;
    interrupts();
    Serial.println(ticks);
  }
}

void updateEncoder() {
  int MSB = digitalRead(encoderPinA); // channel A
  int LSB = digitalRead(encoderPinB); // channel B

  int encoded = (MSB << 1) | LSB;    // combine the two bits
  int sum = (lastEncoded << 2) | encoded; 

  // Look up table for quadrature transitions
  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderTicks++;
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderTicks--;

  lastEncoded = encoded;
}
