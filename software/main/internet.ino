AsyncWebServer server(80);
void task_wifi(void *pt)
{
  WiFi.mode(WIFI_AP); //设置工作在AP模式
  WiFi.softAP(ssid, password);
  server.on("/", HTTP_GET, handleRootPage);
  server.on("/data", HTTP_GET, handleDataRequest);
  server.on("/command", HTTP_GET, handleCommandRequest);
  // 开启服务器
  server.begin();

  for (;;)
  {
    if (fan_mode == 0)
    {
      fanMode = "手动模式";
      fanSpeed = String(fan_speed) + "档";
    }
    else if (fan_mode == 1)
    {
      fanMode = "智能模式";
      fanSpeed = String(100 * fan_duty / pow(2, 12)) + "%";
    }
    if (turn_angle == 0)
    {
      turnAngle = "未开启";
    }
    else if (turn_angle == 1)
    {
      turnAngle = "90℃";
    }
    else if (turn_angle == 2)
    {
      turnAngle = "120℃";
    }
    else if (turn_angle == 3)
    {
      turnAngle = "150℃";
    }
    else if (turn_angle == 4)
    {
      turnAngle = "180℃";
    }
    vTaskDelay(200);
  }
}

void handleRootPage(AsyncWebServerRequest *request) 
{
  // 构建网页内容
  String htmlContent = "<html><head>";
  htmlContent += "<title>超智能小风扇</title>";
  htmlContent += "<style>";
  htmlContent += "body { background-color: lightblue; font-family: Arial, sans-serif; margin: 0; padding: 20px; }";
  htmlContent += "h1 { text-align: center; }";
  htmlContent += ".data-container { display: flex; justify-content: space-between; align-items: center; margin-bottom: 20px; }";
  htmlContent += ".data-label { font-weight: bold; }";
  htmlContent += ".data-value { font-size: 24px; }";
  htmlContent += ".btn-container { display: flex; justify-content: space-between; margin-top: 10px; }";
  htmlContent += ".btn { padding: 10px; font-size: 16px; }";
  htmlContent += "</style>";
  htmlContent += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
  htmlContent += "<script>";
  htmlContent += "function sendCommand(command) { var xhr = new XMLHttpRequest(); xhr.open('GET', '/command?cmd=' + command, true); xhr.send(); }";
  htmlContent += "function updateData() { var xhr = new XMLHttpRequest(); xhr.onreadystatechange = function() { if (xhr.readyState === 4 && xhr.status === 200) { var data = JSON.parse(xhr.responseText); document.getElementById('temperature').innerText = data.temperature; document.getElementById('humidity').innerText = data.humidity; document.getElementById('fanMode').innerText = data.fanMode;document.getElementById('fan-speed').innerText = data.fanSpeed; document.getElementById('servo-level').innerText = data.turn_speed; document.getElementById('servo-angle').innerText = data.turnAngle; } }; xhr.open('GET', '/data', true); xhr.send(); }";
  htmlContent += "setInterval(updateData, 2000);";
  htmlContent += "</script>";
  htmlContent += "</head><body>";
  htmlContent += "<h1>超智能小风扇</h1>";
  htmlContent += "<div class='data-container'><span class='data-label'>温度:</span><span class='data-value' id='temperature'>--</span></div>";
  htmlContent += "<div class='data-container'><span class='data-label'>湿度:</span><span class='data-value' id='humidity'>--</span></div>";
  htmlContent += "<div class='data-container'><span class='data-label'>模式:</span><span class='data-value' id='fanMode'>--</span><div class='btn-container'><button class='btn' onclick=\"sendCommand('change_mode')\">切换</button></div></div>";
  htmlContent += "<div class='data-container'><span class='data-label'>风扇档位:</span><span class='data-value' id='fan-speed'>--</span><div class='btn-container'><button class='btn' onclick=\"sendCommand('increase_fan_speed')\">+</button><button class='btn' onclick=\"sendCommand('decrease_fan_speed')\">-</button></div></div>";
  htmlContent += "<div class='data-container'><span class='data-label'>舵机档位:</span><span class='data-value' id='servo-level'>--</span><div class='btn-container'><button class='btn' onclick=\"sendCommand('increase_servo_level')\">+</button><button class='btn' onclick=\"sendCommand('decrease_servo_level')\">-</button></div></div>";
  htmlContent += "<div class='data-container'><span class='data-label'>舵机角度:</span><span class='data-value' id='servo-angle'>--</span><div class='btn-container'><button class='btn' onclick=\"sendCommand('increase_servo_angle')\">+</button><button class='btn' onclick=\"sendCommand('decrease_servo_angle')\">-</button></div></div>";
  htmlContent += "</body></html>";

  // 发送网页内容
  request->send(200, "text/html", htmlContent);
}

// 处理数据请求的函数
void handleDataRequest(AsyncWebServerRequest *request) 
{
  // 构建 JSON 格式的数据
  String data = "{";
  data += "\"temperature\": \"" + String(temperature) + " °C\",";
  data += "\"fanMode\": \"" + fanMode + "\",";
  data += "\"humidity\": \"" + String(humidity) + "%\",";
  data += "\"fanSpeed\": \"" + fanSpeed + "\",";
  data += "\"turn_speed\": \"" + String(turn_speed) + "档\",";
  data += "\"turnAngle\": \"" + turnAngle + "°\"";
  data += "}";

  // 发送数据
  request->send(200, "application/json", data);
}

// 处理命令请求的函数
void handleCommandRequest(AsyncWebServerRequest *request) 
{
  if (request->hasParam("cmd")) {
    // 获取命令参数值
    String command = request->getParam("cmd")->value();

    // 根据命令执行相应操作
    if (command == "increase_fan_speed" && fan_speed <= 4)
    {
      fan_speed += 1;
    }
    else if (command == "decrease_fan_speed" && fan_speed >= 0)
    {
      fan_speed -= 1;
    }
    else if (command == "increase_servo_level" && turn_speed <= 4)
    {
      turn_speed += 1;
    }
    else if (command == "decrease_servo_level" && turn_speed >= 0)
    {
      turn_speed -= 1;
    }
    else if (command == "increase_servo_angle" && turn_angle <= 3)
    {
      turn_angle += 1;
    }
    else if (command == "decrease_servo_angle" && turn_angle >= 0)
    {
      turn_angle -= 1;
    }
    else if (command == "change_mode" && fan_mode == 0)
    {
      fan_mode = 1;
    }
    else if (command == "change_mode" && fan_mode == 1)
    {
      fan_mode = 0;
    }

    // 发送成功响应
    request->send(200, "text/plain", "Command received: " + command);
  }
}
