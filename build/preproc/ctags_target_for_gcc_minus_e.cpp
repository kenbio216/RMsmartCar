# 1 "E:\\win_code_git\\alist_ACTIVITY\\RMsmartCar\\main\\main.ino"
/*

 * @Author: xuyang

 * @Date: 2024-06-09 16:05:06

 * @LastEditors: xuyang

 * @LastEditTime: 2024-07-05 19:55:09

 * @FilePath: \RMsmartCar\main\main.ino

 * @Description:

 *

 * Copyright (c) 2024 by xuyang, All Rights Reserved

 */
# 12 "E:\\win_code_git\\alist_ACTIVITY\\RMsmartCar\\main\\main.ino"
# 13 "E:\\win_code_git\\alist_ACTIVITY\\RMsmartCar\\main\\main.ino" 2
# 14 "E:\\win_code_git\\alist_ACTIVITY\\RMsmartCar\\main\\main.ino" 2

const char *ssid = "ESP32-Access-Point";
const char *password = "123456789";

WebServer server(80); // HTTP服务器在端口80

const int ledPin = 12; // 根据你的ESP32板子的LED引脚可能有所不同

// HTML页面内容，包含控制LED的按钮
const char *htmlPage = R"(
<!DOCTYPE html>
<html>
<body>
<h2>ESP32 LED Control</h2>
<p><a href="/led/on">Turn On</a></p>
<p><a href="/led/off">Turn Off</a></p>
</body>
</html>
)";

// 前进的函数
void forward()
{
    digitalWrite(2, 0x1);
    digitalWrite(3, 0x0);
}

//接收10号引脚的编码器数据，并且在串口上打印它
void encoder()
{
    int encoder = digitalRead(10);
    Serial.println(encoder);
}

void setup()
{
    // 编码器部分
    pinMode(10, 0x01);
    // 轮子部分
    pinMode(2, 0x03);
    pinMode(3, 0x03);
    // digitalWrite(2, HIGH);
    // digitalWrite(3, LOW);
    // wifi部分
    pinMode(ledPin, 0x03); // 设置LED引脚为输出模式
    digitalWrite(ledPin, 0x0); // 默认关闭LED
    Serial.begin(115200);

    // 设置ESP32为AP模式
    WiFi.softAP(ssid, password);
    Serial.println("Access Point Started");
    Serial.println("IP Address: ");
    Serial.println(WiFi.softAPIP());

    // 定义URL路由和对应的处理函数
    server.on("/", HTTP_GET, []()
              { server.send(200, "text/html", htmlPage); });
    server.on("/go", HTTP_GET, []()
              {
    digitalWrite(ledPin, 0x1);
    forward();
    server.send(200, "text/html", "<p>LED is ON</p><a href='/'>Go Back</a>"); });
    server.on("/back", HTTP_GET, []()
              {
    digitalWrite(ledPin, 0x0);
    encoder();
    server.send(200, "text/html", "<p>LED is OFF</p><a href='/'>Go Back</a>"); });

    // 启动服务器
    server.begin();
}

void loop()
{
    server.handleClient(); // 处理客户端请求
}
