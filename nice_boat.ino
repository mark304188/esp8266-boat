#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <StreamString.h>

//外置HTML
#include "index_html.h"
extern const char INDEX_HTML[] PROGMEM;

// ============ 配置 ============
#ifndef APSSID
#define APSSID "NiceBoat";
#define APPSK "nbnbnbnb"; 
#endif

const char *ssid = APSSID;
const char *password = APPSK;


#define LEFT_PIN_PWM  D1  //EN1
#define LEFT_PIN_DIR1 D5  //IN1
#define LEFT_PIN_DIR2 D4  //IN2

#define RIGHT_PIN_PWM D0  //EN2
#define RIGHT_PIN_DIR1 D2 //IN4
#define RIGHT_PIN_DIR2 D3 //IN3

ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

// 简易解析变量
int targetLeft = 0;
int targetRight = 0;

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    delay(1000);
    // 1. 配置串口、wifi
    Serial.begin(250000);
    Serial.println();
    Serial.print("Configuring access point...");
    WiFi.softAP(ssid, password);
    IPAddress myIP = WiFi.softAPIP();

    Serial.print("AP IP address: ");
    Serial.println(myIP); // 默认应该是 192.168.4.1

    // 2. 初始化引脚
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(LEFT_PIN_PWM, OUTPUT);
    pinMode(LEFT_PIN_DIR1, OUTPUT);
    pinMode(LEFT_PIN_DIR2, OUTPUT);
    pinMode(RIGHT_PIN_PWM, OUTPUT);
    pinMode(RIGHT_PIN_DIR1, OUTPUT);
    pinMode(RIGHT_PIN_DIR2, OUTPUT);

    stopMotors(); // 初始停止

    // 3. 启动WebSocket
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
    Serial.println("WebSocket Server started on port 81");
    
    // 4. mdns
    if (MDNS.begin("esp8266")) { Serial.println("MDNS responder started"); }
    
    // 5. 启动http服务器
    server.on("/", handleRoot);
    server.begin();
    Serial.println("HTTP server started");
    server.on("/", handleRoot);
    server.onNotFound(handleNotFound);
    server.begin();
    Serial.println("HTTP server started");

}

void loop() {
    // 必须在loop中不断调用此函数处理通信
    webSocket.loop();
    server.handleClient();
    MDNS.update();

}

//webserver
void handleRoot() {
    digitalWrite(LED_BUILTIN, 1);
    server.send_P(200, "text/html", INDEX_HTML);
    digitalWrite(LED_BUILTIN, 0);
}
void handleNotFound() {
    digitalWrite(LED_BUILTIN, 1);
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";

    for (uint8_t i = 0; i < server.args(); i++) { message += " " + server.argName(i) + ": " + server.arg(i) + "\n"; }

    server.send(404, "text/plain", message);
    digitalWrite(LED_BUILTIN, 0);
}

// WebSocket 事件回调
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    if (type == WStype_TEXT) {
        // 解析数据格式: [L,R] 例如 [255,-100]
        // 为了100Hz的高频，我们使用轻量级的字符解析，避免String类开销
    
    if (length > 2 && payload[0] == '[') {
        // 寻找逗号分隔符
        char *comma = (char *)memchr(payload, ',', length);
        if (comma != NULL) {
        // 解析左电机 (跳过 '[')
        targetLeft = atoi((const char *)(payload + 1));
        
        // 解析右电机 (跳过 ',')
        targetRight = atoi((const char *)(comma + 1));
//        for debug
//        Serial.print("l:");
//        Serial.print(targetLeft);
//        Serial.print(" R:");
//        Serial.print(targetRight);
//        Serial.print("\n");
        // 执行电机控制
        driveMotor(targetLeft, LEFT_PIN_PWM, LEFT_PIN_DIR1, LEFT_PIN_DIR2);
        driveMotor(targetRight, RIGHT_PIN_PWM, RIGHT_PIN_DIR1, RIGHT_PIN_DIR2);

        // 立即回复，维持前端心跳计数 (发送任意简短字符即可)
        webSocket.sendTXT(num, "OK"); 
      }
    }
  }
}

// 电机驱动函数
// value: -255 到 255
void driveMotor(int value, int pwmPin, int dir1, int dir2) {
    if (value > 0) {
        // 正转
        digitalWrite(dir1, HIGH);
        digitalWrite(dir2, LOW);
        analogWrite(pwmPin, value);
    } else if (value < 0) {
        // 反转
        digitalWrite(dir1, LOW);
        digitalWrite(dir2, HIGH);
        analogWrite(pwmPin, -value); // PWM值需为正
    } else {
        // 停止 (或者刹车: HIGH, HIGH)
        digitalWrite(dir1, LOW);
        digitalWrite(dir2, LOW);
        analogWrite(pwmPin, 0);
    }
}

void stopMotors() {
    driveMotor(0, LEFT_PIN_PWM, LEFT_PIN_DIR1, LEFT_PIN_DIR2);
    driveMotor(0, RIGHT_PIN_PWM, RIGHT_PIN_DIR1, RIGHT_PIN_DIR2);
}
