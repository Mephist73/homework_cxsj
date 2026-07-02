#include <Arduino.h>

// 两路LED引脚定义
const int ledA = 2;
const int ledB = 4;

// 亮度变量 0~255
int brightnessA = 0;
// 渐变步长，控制交替快慢
int step = 3;
// 渐变方向 1向上，-1向下
int dir = 1;

void setup() {
  Serial.begin(115200);
  pinMode(ledA, OUTPUT);
  pinMode(ledB, OUTPUT);
}

void loop() {
  // 更新A灯亮度
  brightnessA += dir * step;

  // 边界判断，到达极值反转渐变方向
  if (brightnessA >= 255) {
    brightnessA = 255;
    dir = -1;
  }
  if (brightnessA <= 0) {
    brightnessA = 0;
    dir = 1;
  }

  // 核心反相关系：B亮度 = 255 - A亮度
  int brightnessB = 255 - brightnessA;

  // 两路独立PWM输出
  analogWrite(ledA, brightnessA);
  analogWrite(ledB, brightnessB);

  // 控制渐变顺滑度，数值越小交替速度越快
  delay(15);
}