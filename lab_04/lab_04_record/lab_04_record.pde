import processing.serial.*;

Serial port;
PrintWriter out;

void setup() {
  println(Serial.list());
  String portName = Serial.list()[0];     // <-- change to your port index
  port = new Serial(this, portName, 115200);
  port.bufferUntil('\n');

  // Create a timestamped CSV file
  String fn = nf(year(),4)+nf(month(),2)+nf(day(),2)+"_"+nf(hour(),2)+nf(minute(),2)+nf(second(),2);
  out = createWriter("arduino_log_"+fn+".csv");

  // CSV header matches Arduino output
  out.println("t_ms,distanceRight_mm,distanceLeft_mm");
}

void serialEvent(Serial p) {
  String line = trim(p.readStringUntil('\n'));
  if (line == null || line.length() == 0) return;

  String[] parts = split(line, ',');
  if (parts.length == 2) {
    // Parse as float
    float distanceRight = float(parts[0]);
    float distanceLeft  = float(parts[1]);

    // Record with timestamp in milliseconds
    out.println(millis() + "," + distanceRight + "," + distanceLeft);
    out.flush();   // force write to disk immediately
  }
}

// Press any key to safely close the file and exit
void keyPressed() {
  out.flush();
  out.close();
  exit();
}

void draw() {}
