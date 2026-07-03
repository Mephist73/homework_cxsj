#include <WiFi.h>
#include <WebServer.h>

// WiFi信息，改成你自己的
const char* ssid     = "小狗怎么叫";
const char* password = "wangwang";

// LED引脚
const int ledPin = 2;

WebServer server(80);

// 网页页面，包含题目要求的range滑动条+JS fetch
String htmlPage = R"HTML(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <title>ESP32 网页无极调光</title>
    <style>
        body { text-align: center; margin-top: 80px; font-size: 22px; }
        .slider { width: 80%; height: 30px; margin: 30px 0; }
        #val { color: red; font-weight: bold; font-size: 26px; }
    </style>
</head>
<body>
    <h2>LED亮度调节滑块</h2>
    <input class="slider" type="range" min="0" max="255" id="brightSlider">
    <p>当前亮度值：<span id="val">0</span></p >

    <script>
        const slider = document.getElementById("brightSlider");
        const valText = document.getElementById("val");

        slider.addEventListener("input", function(){
            let bright = this.value;
            valText.innerText = bright;
            fetch(`/set?val=${bright}`)
                .catch(err => console.log("请求失败", err));
        });
    </script>
</body>
</html>
)HTML";

// 首页
void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

// 接收滑块亮度值，用analogWrite输出
void handleSetBright() {
  int brightness = 0;
  if(server.hasArg("val")){
    brightness = server.arg("val").toInt();
    brightness = constrain(brightness, 0, 255);
    // 替代ledc，直接模拟PWM输出
    analogWrite(ledPin, brightness);
  }
  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  analogWrite(ledPin, 0); // 初始熄灭

  // 连接WiFi
  Serial.print("连接WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi连接成功");
  Serial.print("访问地址：http://");
  Serial.println(WiFi.localIP());

  // 路由绑定
  server.on("/", handleRoot);
  server.on("/set", handleSetBright);
  server.begin();
  Serial.println("Web服务启动完成");
}

void loop() {
  server.handleClient();
}