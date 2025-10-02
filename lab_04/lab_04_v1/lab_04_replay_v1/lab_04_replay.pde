import processing.serial.Serial;
import java.io.*;

// --- Serial port ---
Serial myPort;

// --- CSV data ---
ArrayList<Row> rows = new ArrayList<Row>();
String csvFile = "arduino_log_20250930_185525.csv"; // <-- replace with your CSV filename

// --- Last command sent ---
char lastCommand = 's'; // initially stopped

// --- Smoothing settings ---
int smoothingWindow = 4;  // moving average over last 4 rows
float deadZone = 2.0;     // ignore movement smaller than 2 mm

void setup() {
  println(Serial.list());
  String portName = Serial.list()[0]; // adjust index for your Arduino
  myPort = new Serial(this, portName, 115200);

  loadCSV(csvFile);
  println("Loaded " + rows.size() + " rows.");

  playCSV();
  println("Playback complete.");
  myPort.write('s'); // stop robot at the end
  exit();
}

// --- Load CSV file ---
void loadCSV(String filename) {
  String[] lines = loadStrings(filename);
  for (String line : lines) {
    line = trim(line);
    if (line.length() == 0) continue;
    if (line.startsWith("t")) continue; // skip header
    String[] parts = split(line, ',');
    if (parts.length < 3) continue;
    float t = float(parts[0]);
    float r = float(parts[1]);
    float l = float(parts[2]);
    rows.add(new Row(t, r, l));
  }
}

// --- Playback with smoothing ---
void playCSV() {
  for (int i = 1; i < rows.size(); i++) {
    // --- Compute smoothed deltas ---
    float avgDR = 0, avgDL = 0;
    int count = 0;
    for (int j = max(1, i - smoothingWindow + 1); j <= i; j++) {
      Row cur = rows.get(j);
      Row prev = rows.get(j - 1);
      avgDR += cur.right - prev.right;
      avgDL += cur.left - prev.left;
      count++;
    }
    avgDR /= count;
    avgDL /= count;

    // --- Apply dead zone ---
    if (abs(avgDR) < deadZone) avgDR = 0;
    if (abs(avgDL) < deadZone) avgDL = 0;

    // --- Determine movement command ---
    char cmd = inferCommand(avgDR, avgDL);

    // --- Send command if changed ---
    if (cmd != lastCommand) {
      myPort.write(cmd);
      println("Sent command: " + cmd + " at t=" + rows.get(i).time);
      lastCommand = cmd;
    }

    // --- Wait until next row ---
    float dt = (rows.get(i).time - rows.get(i - 1).time) * 1000; // ms
    delay((int)dt);
  }
}

// --- Command inference ---
char inferCommand(float dR, float dL) {
  if (dR == 0 && dL == 0) return 's';                // stop
  if (abs(dR - dL) < deadZone) return 'w';          // forward
  if (abs(dR) < deadZone && dL > 0) return 'd';     // turn right
  if (dR > 0 && abs(dL) < deadZone) return 'a';     // turn left
  return 'w';                                       // fallback forward
}

// --- Row class ---
class Row {
  float time;
  float right;
  float left;

  Row(float t, float r, float l) {
    time = t;
    right = r;
    left = l;
  }
}

void draw() {
  // no graphics needed
}