import processing.serial.*;

// -------------------- SERIAL & RECORDING --------------------
Serial port;
PrintWriter out;

// -------------------- BUTTONS --------------------
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

// -------------------- SETUP --------------------
void setup() {
  size(400, 400);
  background(220);
  
  // --- Serial connection ---
  println(Serial.list());
  String portName = Serial.list()[0]; // change index if needed
  port = new Serial(this, portName, 115200);
  port.bufferUntil('\n');

  // --- CSV recording ---
  String fn = nf(year(),4)+nf(month(),2)+nf(day(),2)+"_"+nf(hour(),2)+nf(minute(),2)+nf(second(),2);
  out = createWriter("arduino_log_"+fn+".csv");
  out.println("t_s,distanceRight_mm,distanceLeft_mm");
  
  // --- Create control buttons ---
  buttons = new ControlButton[4];
  buttons[0] = new ControlButton(width/2 - 30, 120, 60, 60, 'w'); // forward
  buttons[1] = new ControlButton(width/2 - 30, 190, 60, 60, 's'); // stop
  buttons[2] = new ControlButton(width/2 - 100, 190, 60, 60, 'a'); // left
  buttons[3] = new ControlButton(width/2 + 40, 190, 60, 60, 'd'); // right
}

// -------------------- DRAW --------------------
void draw() {
  background(220);
  fill(0);
  textAlign(CENTER);
  textSize(16);
  text("Keyboard Robot Controller", width/2, 20);
  
  for (ControlButton b : buttons) {
    b.display();
  }
}

// -------------------- SERIAL EVENT (RECORDING) --------------------
void serialEvent(Serial p) {
  String raw = p.readStringUntil('\n');
  if (raw == null) return;

  String line = trim(raw);
  if (line.length() == 0) return;

  String[] parts = split(line, ',');
  if (parts.length == 2) {
    try {
      float distanceRight = float(parts[0]);
      float distanceLeft  = float(parts[1]);
      out.println((millis() / 1000.0) + "," + distanceRight + "," + distanceLeft);
      out.flush();
    } catch (Exception e) {
      println("Parse error: " + line);
    }
  }
}

// -------------------- KEYBOARD CONTROLS --------------------
void keyPressed() {
  // Activate button & send key to Arduino
  for (ControlButton b : buttons) {
    if (key == b.keyChar) {
      b.active = true;
      port.write(key);
      println("Sent: " + key);
    }
  }
  
  // Allow safe exit if 'ESC' is pressed
  if (key == ESC) {
    key = 0; // prevent default ESC behaviour
    safeExit();
  }
}

void keyReleased() {
  // Deactivate button & send stop
  for (ControlButton b : buttons) {
    if (key == b.keyChar) {
      b.active = false;
      // port.write('x'); // stop command
      // println("Sent: stop");
    }
  }
}

// -------------------- SAFE EXIT --------------------
void safeExit() {
  if (out != null) {
    out.flush();
    out.close();
    println("CSV file saved.");
  }
  exit();
}
