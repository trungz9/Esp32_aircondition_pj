#include <WiFi.h>
#include <WebServer.h>

/* ========== CẤU HÌNH WIFI (PHÁT WIFI) ========== */
const char* ssid = "ESP32_DieuKhienQuat"; // Tên WiFi ESP32 phát ra
const char* password = "12345678";        // Mật khẩu (ít nhất 8 ký tự)

/* ========== PIN ========== */
#define FAN_PIN 4 
#define LED_PIN 2 
#define SMOKE_PIN 34 

#define PWM_FREQ 25000
#define PWM_RESOLUTION 8

WebServer server(80);

/* ========== BIẾN TOÀN CỤC ========== */
int currentSmokePercent = 0;
int manualSpeed = 0;
bool isAutoMode = false;

/* ========== GIAO DIỆN WEB (Giữ nguyên logic cũ) ========== */
const char webpage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta charset="UTF-8">
  <title>ESP32 Local Control</title>
  <style>
    body { font-family: sans-serif; text-align: center; background: #e0f7fa; padding: 20px; }
    .card { background: white; padding: 20px; border-radius: 15px; box-shadow: 0 4px 10px rgba(0,0,0,0.1); margin-bottom: 20px; }
    .status-alert { color: #d32f2f; font-weight: bold; font-size: 1.2em; }
    .switch { position: relative; display: inline-block; width: 60px; height: 34px; }
    .switch input { opacity: 0; width: 0; height: 0; }
    .slider-switch { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; transition: .4s; border-radius: 34px; }
    .slider-switch:before { position: absolute; content: ""; height: 26px; width: 26px; left: 4px; bottom: 4px; background-color: white; transition: .4s; border-radius: 50%; }
    input:checked + .slider-switch { background-color: #009688; }
    input:checked + .slider-switch:before { transform: translateX(26px); }
  </style>
</head>
<body onload="setInterval(updateData, 1000)">
  <div class="card">
    <h2>CHẾ ĐỘ TỰ ĐỘNG</h2>
    <label class="switch">
      <input type="checkbox" id="autoCheck" onchange="toggleAuto(this.checked)">
      <span class="slider-switch"></span>
    </label>
    <p id="modeText">Chế độ: THỦ CÔNG</p>
  </div>
  <div class="card" id="manualControl">
    <h2>ĐIỀU KHIỂN QUẠT</h2>
    <input type="range" min="0" max="100" value="0" id="speedRange" oninput="setSpeed(this.value)" style="width:80%">
    <p>Công suất: <span id="speedVal">0</span>%</p>
  </div>
  <div class="card">
    <h2>CẢM BIẾN KHÓI</h2>
    <h1 style="font-size: 50px; color:#00796b;"><span id="smokeDisplay">0</span>%</h1>
    <p id="alertText">Hệ thống sẵn sàng</p>
  </div>
<script>
function toggleAuto(isAuto) {
  document.getElementById("modeText").innerHTML = isAuto ? "Chế độ: TỰ ĐỘNG" : "Chế độ: THỦ CÔNG";
  document.getElementById("manualControl").style.opacity = isAuto ? "0.3" : "1.0";
  document.getElementById("speedRange").disabled = isAuto;
  fetch("/mode?auto=" + (isAuto ? 1 : 0));
}
function setSpeed(v) {
  document.getElementById("speedVal").innerHTML = v;
  fetch("/set?speed=" + v);
}
function updateData() {
  fetch("/status").then(response => response.json()).then(data => {
    document.getElementById("smokeDisplay").innerHTML = data.smoke;
    if(data.isAuto) document.getElementById("speedVal").innerHTML = data.fan;
    if(data.smoke > 35) {
      document.getElementById("alertText").innerHTML = "CẢNH BÁO: PHÁT HIỆN KHÓI!";
      document.getElementById("alertText").className = "status-alert";
    } else {
      document.getElementById("alertText").innerHTML = "Trạng thái: An toàn";
      document.getElementById("alertText").className = "";
    }
  });
}
</script>
</body>
</html>
)rawliteral";

/* ========== XỬ LÝ SERVER ========== */
void handleStatus() {
  int fanSpeed = isAutoMode ? map(currentSmokePercent, 0, 100, 0, 100) : manualSpeed;
  String json = "{\"smoke\":" + String(currentSmokePercent) + 
                ",\"fan\":" + String(fanSpeed) + 
                ",\"isAuto\":" + String(isAutoMode ? 1 : 0) + "}";
  server.send(200, "application/json", json);
}

void handleMode() {
  if (server.hasArg("auto")) {
    isAutoMode = (server.arg("auto") == "1");
  }
  server.send(200, "text/plain", "OK");
}

void handleSetSpeed() {
  if (!isAutoMode && server.hasArg("speed")) {
    manualSpeed = server.arg("speed").toInt();
    int duty = map(manualSpeed, 0, 100, 0, 255);
    ledcWrite(FAN_PIN, duty);
    ledcWrite(LED_PIN, duty);
  }
  server.send(200, "text/plain", "OK");
}

/* ========== SETUP ========== */
void setup() {
  Serial.begin(115200);
  
  ledcAttach(FAN_PIN, PWM_FREQ, PWM_RESOLUTION);
  ledcAttach(LED_PIN, PWM_FREQ, PWM_RESOLUTION);
  pinMode(SMOKE_PIN, INPUT);

  // THIẾT LẬP ACCESS POINT
  Serial.println("Dang khoi tao WiFi...");
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("WiFi phat ra: "); Serial.println(ssid);
  Serial.print("Truy cap vao IP: "); Serial.println(myIP);

  server.on("/", [](){ server.send(200, "text/html", webpage); });
  server.on("/status", handleStatus);
  server.on("/mode", handleMode);
  server.on("/set", handleSetSpeed);
  server.begin();
}

void loop() {
  server.handleClient();

  static unsigned long prevMs = 0;
  if (millis() - prevMs > 500) {
    int raw = analogRead(SMOKE_PIN);
    currentSmokePercent = map(raw, 0, 4095, 0, 100);

    if (isAutoMode) {
      int autoDuty = (currentSmokePercent < 15) ? 0 : map(currentSmokePercent, 0, 100, 0, 255);
      ledcWrite(FAN_PIN, autoDuty);
      ledcWrite(LED_PIN, autoDuty);
    }
    prevMs = millis();
  }
}