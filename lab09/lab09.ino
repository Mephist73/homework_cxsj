#include <WiFi.h>
#include <WebServer.h>

// WiFi配置，自行修改
const char* ssid = "小狗怎么叫";
const char* password = "wangwang";

// 触摸引脚 T0 = GPIO4
const int touchPin = T0;

WebServer server(80);

// 仪表盘网页HTML，内置AJAX定时拉取数据
String dashHtml = R"HTML(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <title>触摸传感器实时仪表盘</title>
    <style>
        body{
            text-align:center;
            background:#111;
            color:#0ff;
            font-family:Arial;
            margin-top:120px;
        }
        h2{font-size:32px;}
        #sensorVal{
            font-size:100px;
            font-weight:bold;
            margin:40px 0;
        }
        .tip{font-size:20px;color:#ccc;}
    </style>
</head>
<body>
    <h2>触摸传感器实时数值仪表盘</h2>
    <div id="sensorVal">0</div>
    <p class="tip">手指靠近引脚 → 数字变小｜松开手指 → 数字变大</p >

    <script>
        // 定时拉取传感器数据，50ms刷新一次
        function getData(){
            fetch("/data")
                .then(res => res.text())
                .then(val => {
                    document.getElementById("sensorVal").innerText = val;
                })
                .catch(err => console.log("数据获取失败"));
        }
        // 每50毫秒执行一次拉取
        setInterval(getData, 50);
    </script>
</body>
</html>
)HTML";

// 首页：展示仪表盘页面
void handleRoot(){
    server.send(200, "text/html", dashHtml);
}

// 数据接口：返回实时触摸传感器数值
void handleGetData(){
    int touchValue = touchRead(touchPin);
    server.send(200, "text/plain", String(touchValue));
}

void setup(){
    Serial.begin(115200);

    // 连接WiFi
    WiFi.begin(ssid, password);
    while(WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi连接成功");
    Serial.print("仪表盘地址：http://");
    Serial.println(WiFi.localIP());

    // 注册网页路由
    server.on("/", handleRoot);
    server.on("/data", handleGetData);
    server.begin();
}

void loop(){
    server.handleClient();
}