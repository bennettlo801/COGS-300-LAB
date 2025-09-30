// Records output in encoderRight and encoderLeft to seconds in CSV file. 

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

  // CSV header matches Arduino output (time in seconds now)
  out.println("t_s,distanceRight_mm,distanceLeft_mm");
}

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

      // Record with timestamp in seconds (with decimals)
      out.println((millis() / 1000.0) + "," + distanceRight + "," + distanceLeft);
      out.flush();
    } catch (Exception e) {
      println("Parse error: " + line);
    }
  }
}

// Press any key to safely close the file and exit
void keyPressed() {
  out.flush();
  out.close();
  exit();
}

void draw() {}
