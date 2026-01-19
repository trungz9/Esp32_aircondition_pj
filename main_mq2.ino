#include <WiFi.h>
#include <WebServer.h>

/* ========== WIFI ========== */
const char* ssid = "P 610";
const char* password = "17108898";

/* ========== PIN ========== */
#define FAN_PIN 4
#define LED_PIN 2

#define MQ2_AO_PIN 34
#define MQ2_DO_PIN 27

/* ========== PWM ========== */
#define PWM_FREQ 25000
#define PWM_RESOLUTION 8   // 0–255

WebServer server(80);

/* ========== BIẾN TRẠNG THÁI ========== */
bool autoMode = false;
int manualSpeed = 0;

/* ========== HTML ========== */
const char webpage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>ESP32 Fan Control</title>
</head>
<body style="font-family:Arial;text-align:center;">

<h2>Dieu khien quat & LED</h2>

<label>
  <input type="checkbox" id="mode" onchange="setMode(this.checked)">
  AUTO MODE (MQ-2)
</label>

<br><br>

<input type="range" min="0" max="100" value="0"
       id="slider"
       oninput="setSpeed(this.value)">
<p>Cong suat: <span id="val">0</span>%</p>

<hr>

<h3>Cam bien khoi MQ-2</h3>
<p>ADC: <span id="mq2_raw">0</span></p>
<p>Muc khoi: <span id="mq2_percent">0</span>%</p>

<script>
function setSpeed(v) {
  document.getElementById("val").innerHTML = v;
  fetch("/set?speed=" + v);
}

function setMode(v) {
  fetch("/mode?auto=" + v);
  document.getElementById("slider").disabled = v;
}

function updateMQ2() {
  fetch("/mq2")
    .then(r => r.json())
    .then(d => {
      document.getElementById("mq2_raw").innerHTML = d.raw;
      document.getElementById("mq2_percent").innerHTML = d.percent;
    });
}

setInterval(updateMQ2, 1000);
</script>

</body>
</html>
)rawliteral";

/* ========== HANDLERS ========== */

void handleRoot() {
  server.send(200, "text/html", webpage);
}

void handleSetSpeed() {
  if (!autoMode && server.hasArg("speed")) {
    manualSpeed = constrain(server.arg("speed").toInt(), 0, 100);
  }
  server.send(200, "text/plain", "OK");
}

void handleMode() {
  if (server.hasArg("auto")) {
    autoMode = (server.arg("auto") == "true");
  }
  server.send(200, "text/plain", "OK");
}

void handleMQ2() {
  int raw = analogRead(MQ2_AO_PIN);
  int percent = map(raw, 0, 4095, 0, 100);

  String json = "{";
  json += "\"raw\":" + String(raw) + ",";
  json += "\"percent\":" + String(percent);
  json += "}";

  server.send(200, "application/json", json);
}

/* ========== SETUP ========== */

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("ESP32 START");

  // PWM pin-based (core mới)
  ledcAttach(FAN_PIN, PWM_FREQ, PWM_RESOLUTION);
  ledcAttach(LED_PIN, PWM_FREQ, PWM_RESOLUTION);

  ledcWrite(FAN_PIN, 0);
  ledcWrite(LED_PIN, 0);

  pinMode(MQ2_DO_PIN, INPUT);

  analogReadResolution(12);
  analogSetPinAttenuation(MQ2_AO_PIN, ADC_11db);

  WiFi.begin(ssid, password);
  Serial.print("WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/set", handleSetSpeed);
  server.on("/mode", handleMode);
  server.on("/mq2", handleMQ2);

  server.begin();
}

/* ========== LOOP ========== */

void loop() {
  server.handleClient();

  int duty = 0;

  if (autoMode) {
    int mq2 = analogRead(MQ2_AO_PIN);
    duty = map(mq2, 1000, 3500, 0, 255);
    duty = constrain(duty, 0, 255);
  } else {
    duty = map(manualSpeed, 0, 100, 0, 255);
  }

  ledcWrite(FAN_PIN, duty);
  ledcWrite(LED_PIN, duty);

  delay(10);
}
