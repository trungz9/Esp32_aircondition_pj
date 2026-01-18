#include <WiFi.h>
#include <WebServer.h>

/* ========== WIFI ========== */
const char* ssid = "P 610";      // đổi theo WiFi của bạn
const char* password = "17108898";

/* ========== PIN ========== */
#define FAN_PIN 4     // GPIO04 → MOSFET quạt
#define LED_PIN 2     // GPIO02 → LED

/* ========== PWM ========== */
#define PWM_FREQ 25000     // 25kHz (êm cho quạt)
#define PWM_RESOLUTION 8   // 0–255

WebServer server(80);

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

  <input type="range" min="0" max="100" value="0"
         oninput="setSpeed(this.value)">
  <p>Công suất: <span id="val">0</span>%</p>

<script>
function setSpeed(v) {
  document.getElementById("val").innerHTML = v;
  fetch("/set?speed=" + v);
}
</script>
</body>
</html>
)rawliteral";

/* ========== HANDLERS ========== */
void handleRoot() {
  server.send(200, "text/html", webpage);
}

void handleSetSpeed() {
  if (server.hasArg("speed")) {
    int speed = server.arg("speed").toInt();   // 0–100
    speed = constrain(speed, 0, 100);

    int duty = map(speed, 0, 100, 0, 255);

    // PIN-BASED PWM (QUAN TRỌNG)
    ledcWrite(FAN_PIN, duty);
    ledcWrite(LED_PIN, duty);

    Serial.print("Speed = ");
    Serial.print(speed);
    Serial.print("%  Duty = ");
    Serial.println(duty);
  }

  server.send(200, "text/plain", "OK");
}

/* ========== SETUP ========== */
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\nKhoi dong ESP32...");

  // Gắn PWM cho từng PIN (core mới)
  ledcAttach(FAN_PIN, PWM_FREQ, PWM_RESOLUTION);
  ledcAttach(LED_PIN, PWM_FREQ, PWM_RESOLUTION);

  // Tắt ban đầu
  ledcWrite(FAN_PIN, 0);
  ledcWrite(LED_PIN, 0);

  /* WiFi */
  WiFi.begin(ssid, password);
  Serial.print("Dang ket noi WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi da ket noi");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  /* Web server */
  server.on("/", handleRoot);
  server.on("/set", handleSetSpeed);
  server.begin();

  Serial.println("Web server san sang");
}

/* ========== LOOP ========== */
void loop() {
  server.handleClient();
}

