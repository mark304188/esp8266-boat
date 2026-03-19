# esp8266小船

一个简单的双通道 ESP8266 小船控制程序，主要由 AI 生成。  
此项目为课程作业，仅供参考。

## 项目简介

本项目是一个基于 ESP8266 的双螺旋桨遥控船控制程序，通过 WebSocket 实现浏览器与船载主控的实时通信，支持手机/电脑浏览器端摇杆控制。

主要特点：
- 双通道直流电机控制（左右螺旋桨）
- 基于 WebSocket 的 100Hz 控制刷新
- H5 前端摇杆界面，适配手机与电脑
- 前端与板端代码主要由 ChatGLM 生成，并按 GPLv3 协议开源

## 硬件说明

### 主控与电机驱动

- 主控：ESP-12E 模块（ESP8266 Arduino core）
- 电机驱动：L293D模块
## 软件依赖

- 开发环境：Arduino IDE
- ESP8266 核心库：[Arduino core for ESP8266 WiFi chip](https://github.com/esp8266/Arduino)
- WebSocket 库：[WebSocket Server and Client for Arduino](https://github.com/Links2004/arduinoWebSockets)

## 快速开始

1. 安装 Arduino IDE
2. 安装 ESP8266 开发板支持
3. 安装 `WebSockets` 库（通过库管理器搜索 “WebSockets” 或从上述 GitHub 下载）
4. 打开提供的 `nice_boat.ino`（或你自己的固件代码）
5. 根据实际接线修改引脚定义
6. 选择正确的开发板型号和端口，上传固件
7. ESP8266 会建立 AP，ssid:NiceBoat psk:nbnbnbnb
8. 手机或电脑连接该 Wi‑Fi，打开浏览器访问 `http://192.168.4.1`
9. 使用前端页面中的摇杆控制船只运动

## 前端界面说明

前端界面由 H5 + JavaScript 实现，主要功能：

- 中央摇杆控制
- 摇杆竖向左右各 10% 死区，用于保证前进/后退时不跑偏
- 通过 WebSocket 连接
- 电机 -100% ~ +100% 的显示百分比
- 右上角红绿实心圆指示 WebSocket 连接状态
- 下方可折叠区域显示左右螺旋桨输入量的动态曲线

## AI 生成说明

本项目前端与板端代码主要由 ChatGLM（GLM-5） 生成，根据 GPL 相关定义（The "source code" for a work means the preferred form of the work for making modifications to it.  "Object code" means any non-source form of a work.），开源代码与对应的提示词。

### 前端提示词如下

请帮我写一个h5前端用于遥控船只。
中心为摇杆，轮廓为圆圈，摇杆头为纯色实心圆，摇杆的竖向区域左右有10%的死区，不用显示出来（确保单纯前进后退不会跑偏）。
摇杆外轮廓应占屏幕宽度80%，通过摇杆取得用户输入，以控制左右两个螺旋桨的转动。
使用websocket链接192.168.4.1:81，处理为极简格式输出，每次发送2个数据[L,R]，每个范围-255~+255，轮询率100hz，单片机收到后会回复。
在用户界面则显示输入百分比范围-100%~+100%，0无符号黑色，正数为绿色+开头，负数为红色-开头，数字位置固定在摇杆左右上方。
右上角顶部通过红绿实心圆来指示是否连接，每次收到或没有收到30次回复则更改状态，配有文字“未连接”或“已连接”。
在下方有可折叠选项呈现两个螺旋桨的输入量动态曲线，线粗5px，展开后占屏幕25%高度，左蓝右绿，左右均分带框，底部距离下方和左右边距2%，带有平滑曲线不卡顿，显示3秒内的操作。
界面自动适配手机、电脑操作。
整体界面只保留必要元素，自动黑底白字或白底黑字，纯黑白背景，保持极简。

### 板端提示词如下

下面是我的前端网页的JS部分，请帮我修改前端代码并生成对应的arduino代码，以实现与esp8266的WebSocket服务器（WebSockets 库）实现100hz通信。（之后附上JavaScript代码，因为每次生成不同故未附上）

## 许可证

本项目采用 GPLv3 协议开源，任何基于本项目的代码修改与再发布需遵循 GPLv3协议。

## 免责声明

本项目为课程作业，仅供学习与参考，实际使用时请确保安全。
![687c57da471e10b3542d33ccea94f10f](https://github.com/user-attachments/assets/5dfd542b-8684-450e-85dd-94e96464e72e)

