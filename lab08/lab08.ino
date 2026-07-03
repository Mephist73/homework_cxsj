#include <WiFi.h>
#include <WebServer.h>

// WiFi配置，修改为自己的WiFi
const char* ssid = "小狗怎么叫";
const char* password = "wangwang";

// 硬件引脚定义
const int ledPin = 2;
const int touchPin = T0;  // T0对应GPIO4

// 全局状态变量
bool isArm = false;    // false=撤防，true=布防
bool isAlarm = false;  // 是否正在报警

WebServer server(80);

// 网页HTML：布防、撤防两个按钮
String htmlPage = R"HTML(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>物联网安防报警器</title>
    <style>
        body{text-align:center;margin-top:100px;font-size:24px;}
        button{width:200px;height:80px;font-size:26px;margin:20px;cursor:pointer;}
        #armBtn{background:red;color:white;}
        #disarmBtn{background:green;color:white;}
    </style>
</head>
<body>
    <h2>安防主机控制面板</h2>
    <button id="armBtn" onclick="sendArm()">布防 Arm</button>
    <br>
    <button id="disarmBtn" onclick="sendDisarm()">撤防 Disarm</button>

    <script>
        // 发送布防请求
        function sendArm(){
            fetch("/arm");
            alert("已布防，触碰触摸引脚将触发报警！");
        }
        // 发送撤防请求
        function sendDisarm(){
            fetch("/disarm");
            alert("已撤防，报警解除");
        }
    </script>
</body>
</html>
)HTML";

// 主页页面
void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

// 布防接口
void handleArm() {
  isArm = true;
  server.send(200, "text/plain", "系统已布防");
}

// 撤防接口，同时清除报警
void handleDisarm() {
  isArm = false;
  isAlarm = false;
  digitalWrite(ledPin, LOW);
  server.send(200, "text/plain", "系统已撤防，报警关闭");
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // WiFi连接
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi连接成功，设备IP：");
  Serial.println(WiFi.localIP());

  // 绑定网页路由
  server.on("/", handleRoot);
  server.on("/arm", handleArm);
  server.on("/disarm", handleDisarm);
  server.begin();
}

void loop() {
  server.handleClient(); // 持续处理网页按钮请求

  // 仅布防状态才检测触摸
  if (isArm == true && isAlarm == false) {
    int touchVal = touchRead(touchPin);
    // 触摸阈值，数值变小代表有触碰，可根据板子微调
    if (touchVal < 30) {
      isAlarm = true;
    }
  }

  // 报警状态：LED高频闪烁，松手也不会停
  if (isAlarm == true) {
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
  }
}