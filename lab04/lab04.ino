// 定义触摸引脚 (T0对应GPIO4)
#define TOUCH_PIN 4
// 定义LED引脚 (ESP32 DevKit板载LED通常是GPIO2)
#define LED_PIN 2
// 中断模式设置：0为轮询模式，1为中断模式
#define EXT_ISR_MODE 0

// 阈值，需要通过串口监视器观察并调整
int threshold = 200; 
// 触摸值
int touchValue;

void setup() {
  Serial.begin(115200);
  delay(1000); // 等待串口稳定

  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  touchValue = touchRead(TOUCH_PIN);
  Serial.print("Touch Value: ");
  Serial.println(touchValue);

  // 简单的阈值判断逻辑
  if (touchValue < threshold) {
    digitalWrite(LED_PIN, HIGH); // 触摸时，值变小，LED亮
  } else {
    digitalWrite(LED_PIN, LOW);  // 未触摸，LED灭
  }
  
  delay(100);
}