#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
/* -------------------------------------------------------------------------- */
/*                                    基本定义                                    */
/* -------------------------------------------------------------------------- */
const char *ssid = "ESP32-Access-Point";
const char *password = "123456789";

WebServer server(80); // HTTP服务器在端口80

Servo servo1;
Servo servo2;
/* ---------------------------------- 引脚定义 ---------------------------------- */
const int ledPin = 12;   // 根据你的ESP32板子的LED引脚可能有所不同
const int servoPin1 = 8; // 舵机1连接的引脚
const int servoPin2 = 4; // 舵机2连接的引脚

const int motor1Pin1 = 2;  // 电机1的引脚1
const int motor1Pin2 = 3;  // 电机1的引脚2
const int motor2Pin1 = 10; // 电机2的引脚1
const int motor2Pin2 = 6;  // 电机2的引脚2

/* ---------------------------------- 数值初始化 --------------------------------- */
int angle1 = 90; // 舵机1的初始角度
int angle2 = 90; // 舵机2的初始角度

/* -------------------------------------------------------------------------- */
/*                          HTML页面内容，包含控制LED的按钮和显示区域                          */
/* -------------------------------------------------------------------------- */
const char *htmlPage = R"(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 Control Panel</title>
  <style>
    body { text-align: center; font-family: Arial, sans-serif; }
    .container { display: inline-block; }
    .button {
      display: inline-block;
      padding: 20px 30px;
      margin: 20px;
      font-size: 20px;
      text-align: center;
      text-decoration: none;
      outline: none;
      color: #fff;
      background-color: #4CAF50;
      border: none;
      border-radius: 15px;
      box-shadow: 0 9px #999;
    }
    .button:hover {background-color: #3e8e41}
    .button:active {
      background-color: #3e8e41;
      box-shadow: 0 5px #666;
      transform: translateY(4px);
    }
    #status {
      margin-top: 20px;
      padding: 20px;
      font-size: 20px;
      color: #333;
      border: 1px solid #ccc;
      border-radius: 10px;
    }
  </style>
</head>
<body>
  <div class="container">
    <h2>ESP32 Control Panel</h2>
    <div>
      <h3>Motion Control</h3>
      <a class="button" href="/forward">Forward</a>
      <a class="button" href="/backward">Backward</a>
      <a class="button" href="/left">Left</a>
      <a class="button" href="/right">Right</a>
    </div>
    <div>
      <h3>Servo Control</h3>
      <a class="button" href="/servo1/increase">Servo_one +10 </a>
      <a class="button" href="/servo1/decrease">Servo_one -10 </a>
      <a class="button" href="/servo2/increase">Servo_two +10 </a>
      <a class="button" href="/servo2/decrease">Servo_two -10 </a>
    </div>
    <div>
      <h3>Special Functions</h3>
      <a class="button" href="/led/on">Turn On LED</a>
      <a class="button" href="/led/off">Turn Off LED</a>
    </div>
    <div id="status">Status: Ready</div>
  </div>
  <script>
    function updateStatus(message) {
      document.getElementById('status').innerText = 'Status: ' + message;
    }

    document.querySelectorAll('.button').forEach(button => {
      button.addEventListener('click', (event) => {
        event.preventDefault();
        fetch(button.href).then(response => response.text()).then(text => {
          updateStatus(text);
        });
      });
    });
  </script>
</body>
</html>
)";

// 前进的函数
void forward()
{
    digitalWrite(motor1Pin1, HIGH);
    digitalWrite(motor1Pin2, LOW);
    digitalWrite(motor2Pin1, HIGH);
    digitalWrite(motor2Pin2, LOW);
}

// 后退的函数
void backward()
{
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, HIGH);
    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, HIGH);
}

// 向左的函数
void left()
{
    // 这里根据实际需要添加控制代码
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, HIGH);
    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, LOW);
}

// 向右的函数
void right()
{
    // 这里根据实际需要添加控制代码
    digitalWrite(motor1Pin1, HIGH);
    digitalWrite(motor1Pin2, LOW);
    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, LOW);
}

// 舵机1增加角度
void increaseServo1Angle()
{
    angle1 = min(angle1 + 10, 180);
    servo1.write(angle1);
}

// 舵机1减少角度
void decreaseServo1Angle()
{
    angle1 = max(angle1 - 10, 0);
    servo1.write(angle1);
}

// 舵机2增加角度
void increaseServo2Angle()
{
    angle2 = min(angle2 + 10, 180);
    servo2.write(angle2);
}

// 舵机2减少角度
void decreaseServo2Angle()
{
    angle2 = max(angle2 - 10, 0);
    servo2.write(angle2);
}

void setup()
{
    // 舵机部分
    servo1.attach(servoPin1);
    servo2.attach(servoPin2);

    // wifi部分
    pinMode(ledPin, OUTPUT);   // 设置LED引脚为输出模式
    digitalWrite(ledPin, LOW); // 默认关闭LED
    Serial.begin(115200);

    // 设置ESP32为AP模式
    WiFi.softAP(ssid, password);
    Serial.println("Access Point Started");
    Serial.println("IP Address: ");
    Serial.println(WiFi.softAPIP());

    // 定义URL路由和对应的处理函数
    server.on("/", HTTP_GET, []()
              { server.send(200, "text/html", htmlPage); });

    server.on("/led/on", HTTP_GET, []()
              {
        digitalWrite(ledPin, HIGH);
        server.send(200, "text/html", "LED is ON"); });

    server.on("/led/off", HTTP_GET, []()
              {
        digitalWrite(ledPin, LOW);
        server.send(200, "text/html", "LED is OFF"); });

    server.on("/forward", HTTP_GET, []()
              {
        forward();
        server.send(200, "text/html", "Moving Forward"); });

    server.on("/backward", HTTP_GET, []()
              {
        backward();
        server.send(200, "text/html", "Moving Backward"); });

    server.on("/left", HTTP_GET, []()
              {
        left();
        server.send(200, "text/html", "Turning Left"); });

    server.on("/right", HTTP_GET, []()
              {
        right();
        server.send(200, "text/html", "Turning Right"); });

    server.on("/servo1/increase", HTTP_GET, []()
              {
        increaseServo1Angle();
        server.send(200, "text/html", "Servo 1 Angle: " + String(angle1)); });

    server.on("/servo1/decrease", HTTP_GET, []()
              {
        decreaseServo1Angle();
        server.send(200, "text/html", "Servo 1 Angle: " + String(angle1)); });

    server.on("/servo2/increase", HTTP_GET, []()
              {
        increaseServo2Angle();
        server.send(200, "text/html", "Servo 2 Angle: " + String(angle2)); });

    server.on("/servo2/decrease", HTTP_GET, []()
              {
        decreaseServo2Angle();
        server.send(200, "text/html", "Servo 2 Angle: " + String(angle2)); });

    // 启动服务器
    server.begin();
}

void loop()
{
    server.handleClient(); // 处理客户端请求
}
