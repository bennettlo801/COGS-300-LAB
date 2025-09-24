import processing.serial.*;

Serial myPort;

class ControlButton {
  float x, y, w, h;
  char keyChar;
  boolean active = false;
  
  ControlButton(float x, float y, float w, float h, char keyChar) {
    this.x = x;
    this.y = y;
    this.w = w;
    this.h = h;
    this.keyChar = keyChar;
  }
  
  void display() {
    stroke(0);
    strokeWeight(2);
    if (active) {
      fill(0, 200, 0); // green when active
    } else {
      fill(180);
    }
    rect(x, y, w, h, 10);
    fill(0);
    textAlign(CENTER, CENTER);
    textSize(20);
    text(keyChar, x + w/2, y + h/2);
  }
}

ControlButton[] buttons;

void setup() {
  size(400, 400);
  
  // Serial connection
  println(Serial.list());
  myPort = new Serial(this, Serial.list()[0], 9600);
  
  // Create buttons (arranged roughly like WASD + QE)
  buttons = new ControlButton[6];
  buttons[0] = new ControlButton(width/2 - 30, 120, 60, 60, 'W');  // forward
  buttons[1] = new ControlButton(width/2 - 30, 190, 60, 60, 'S');  // backward
  buttons[2] = new ControlButton(width/2 - 100, 190, 60, 60, 'A'); // left
  buttons[3] = new ControlButton(width/2 + 40, 190, 60, 60, 'D');  // right
  buttons[4] = new ControlButton(width/2 - 100, 120, 60, 60, 'Q'); // rotate left
  buttons[5] = new ControlButton(width/2 + 40, 120, 60, 60, 'E');  // rotate right
}

void draw() {
  background(220);
  fill(0);
  textAlign(CENTER);
  text("Keyboard Robot Controller", width/2, 20);
  
  for (ControlButton b : buttons) {
    b.display();
  }
}

void keyPressed() {
  for (ControlButton b : buttons) {
    if (key == b.keyChar) {
      b.active = true;
      myPort.write(key);
      println("Sent: " + key);
    }
  }
}

void keyReleased() {
  for (ControlButton b : buttons) {
    if (key == b.keyChar) {
      b.active = false;
      myPort.write('x'); // stop when released
      println("Sent: stop");
    }
  }
}
