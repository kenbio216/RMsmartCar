#include <WiFi.h>
#include <WebServer.h>

const char *ssid = "ESP32-Access-Point";
const char *password = "123456789";

WebServer server(80); // HTTP服务器在端口80

const int ledPin = 12; // 根据你的ESP32板子的LED引脚可能有所不同
const int servoPin = 8; // 舵机连接的引脚

int pwmChannel = 4;
int pwmFreq = 50;  // 50 Hz for servo
int pwmResolution = 8;  // 8-bit resolution

// HTML页面内容，包含控制LED的按钮和显示区域
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
      <a class="button" href="/forward">Forward</a>
      <a class="button" href="/backward">Backward</a>
      <a class="button" href="/left">Left</a>
      <a class="button" href="/right">Right</a>
    </div>
    <div>
      <a class="button" href="/led/on">Turn On LED</a>
      <a class="button" href="/led/off">Turn Off LED</a>
      <a class="button" href="/full_forward">Full Speed Forward</a>
      <a class="button" href="/full_backward">Full Speed Backward</a>
    </div>
    <div>
      <a class="button" href="/servo/0">Servo 0°</a>
      <a class="button" href="/servo/90">Servo 90°</a>
      <a class="button" href="/servo/180">Servo 180°</a>
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
    digitalWrite(2, HIGH);
    digitalWrite(3, LOW);
}

// 后退的函数
void backward()
{
    digitalWrite(2, LOW);
    digitalWrite(3, HIGH);
}

// 向左的函数
void left()
{
    // 这里根据实际需要添加控制代码
}

// 向右的函数
void right()
{
    // 这里根据实际需要添加控制代码
}

// 全速前进的函数
void fullForward()
{
    analogWrite(2, 255);
    analogWrite(3, 0);
}

// 全速后退的函数
void fullBackward()
{
    analogWrite(2, 0);
    analogWrite(3, 255);
}

// 舵机控制函数
void setServoAngle(int angle)
{
    int dutyCycle = map(angle, 0, 180, 26, 128);  // 将角度映射到舵机的PWM占空比范围
    ledcWrite(pwmChannel, dutyCycle);
}

//接收10号引脚的编码器数据，并且在串口上打印它
void encoder()
{
    int encoderValue = digitalRead(10);
    Serial.println(encoderValue);
}

void setup()
{
    // 编码器部分
    pinMode(10, INPUT);
    // 轮子部分
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
    // 舵机部分
    ledcSetup(pwmChannel, pwmFreq, pwmResolution); // 设置PWM通道
    ledcAttachPin(servoPin, pwmChannel);           // 将PWM通道附加到指定引脚
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
    server.on("/", HTTP_GET, []() {
        server.send(200, "text/html", htmlPage);
    });

    server.on("/led/on", HTTP_GET, []() {
        digitalWrite(ledPin, HIGH);
        server.send(200, "text/html", "LED is ON");
    });

    server.on("/led/off", HTTP_GET, []() {
        digitalWrite(ledPin, LOW);
        server.send(200, "text/html", "LED is OFF");
    });

    server.on("/forward", HTTP_GET, []() {
        forward();
        server.send(200, "text/html", "Moving Forward");
    });

    server.on("/backward", HTTP_GET, []() {
        backward();
        server.send(200, "text/html", "Moving Backward");
    });

    server.on("/left", HTTP_GET, []() {
        left();
        server.send(200, "text/html", "Turning Left");
    });

    server.on("/right", HTTP_GET, []() {
        right();
        server.send(200, "text/html", "Turning Right");
    });

    server.on("/full_forward", HTTP_GET, []() {
        fullForward();
        server.send(200, "text/html", "Full Speed Forward");
    });

    server.on("/full_backward", HTTP_GET, []() {
        fullBackward();
        server.send(200, "text/html", "Full Speed Backward");
    });

    server.on("/servo/0", HTTP_GET, []() {
        setServoAngle(0);
        server.send(200, "text/html", "Servo at 0°");
    });

    server.on("/servo/90", HTTP_GET, []() {
        setServoAngle(90);
        server.send(200, "text/html", "Servo at 90°");
    });

    server.on("/servo/180", HTTP_GET, []() {
        setServoAngle(180);
        server.send(200, "text/html", "Servo at 180°");
    });

    // 启动服务器
    server.begin();
}

void loop()
{
    server.handleClient(); // 处理客户端请求
    encoder(); // 不断读取编码器数据并在串口打印
}
