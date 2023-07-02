# 超智能小风扇
**项目介绍**

这是中山大学电子设计校内赛的一个作品的软件部分开源地址，仅供学习和参考。风扇采用esp32主控，基于arduino平台开发，采用FreeRTOS实时操作系统，多线程并发。

**风扇功能介绍**

- 支持4档pwm调速
- 支持自定义摇头角度范围，摇头速度
- 支持开启开机密码，并支持自定义开机密码
- 支持开启热点进行远程操控，网页端支持查看实时温度和湿度，支持摇头，调速，风扇模式等基本设置
- 支持自定义热点密码，暂不支持热点名称更改
- 支持断电记忆，重启后可以自动恢复关闭前的状态
- 支持智能模式和手动模式之间切换，智能模式可根据温度自动调整风速
- 支持定时开启和定时关闭操作
- 支持主页更换，内置3种不同风格主页，可自定义更换
- 内置电源管理芯片，可通过type-c充电，也可以通过内置锂电池供电
- 支持4.5v-28v电压输入

**相关库链接**

- ESP32库，开源地址：https://github.com/espressif/arduino-esp32
- u8g2库，开源地址：https://github.com/olikraus/u8g2
- WiFi库，开源地址：https://www.arduino.cc/reference/en/libraries/wifi
- 存储库，地址：https://github.com/vshymanskyy/Preferences
- DHT传感器库，开源地址：https://github.com/adafruit/DHT-sensor-library
- 异步服务器库，开源地址：https://github.com/dvarrel/AsyncTCP
