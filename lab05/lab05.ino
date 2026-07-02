#include <WiFi.h>
// 硬件定义
const int ledPin = 2;
const int touchPin = T0; // T0=GPIO4

// 呼吸参数
int brightness = 0;    // 当前亮度 0~255
int fadeDir = 1;       // 渐变方向 1变亮 / -1变暗
int speedGear = 1;     // 初始档位1
int step = 2;          // 基础步长

// 触摸防抖变量
uint16_t touchVal;
unsigned long lastTouchTime = 0;
const unsigned long touchDelay = 300; // 触摸防抖300ms

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  analogWrite(ledPin, 0);
}

void loop() {
  // 1. 触摸检测 + 档位切换
  touchVal = touchRead(touchPin);
  // ESP32触摸值越小=触摸越强烈，阈值自行微调（默认<300判定触摸）
  if (touchVal < 300 && millis() - lastTouchTime > touchDelay) {
    lastTouchTime = millis();
    // 档位循环切换
    speedGear++;
    if(speedGear > 3) speedGear = 1;
    Serial.print("切换档位：");
    Serial.println(speedGear);
  }

  // 2. 根据档位修改渐变步长，控制呼吸快慢
  switch(speedGear){
    case 1: step = 2; break; // 慢速
    case 2: step = 5; break; // 中速
    case 3: step = 9; break; // 快速
  }

  // 3. PWM呼吸渐变逻辑
  brightness = brightness + fadeDir * step;
  // 边界反转渐变方向
  if(brightness <= 0){
    brightness = 0;
    fadeDir = 1;
  }
  if(brightness >= 255){
    brightness = 255;
    fadeDir = -1;
  }
  analogWrite(ledPin, brightness);
  
  delay(10); // 基础延时统一，仅靠步长区分速度
}