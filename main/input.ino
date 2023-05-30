/*定义一个函数，在被调用时获取按键值*/
int32_t get_command()
{
  int state = 0;
  down_state = digitalRead(Down);
  up_state = digitalRead(Up);
  ok_state = digitalRead(Ok);
  comeback_state = digitalRead(Comeback);
  current_tick = xTaskGetTickCount();
  if ((current_tick - privious_tick) >= delay_time)
  {
    if ((down_state == HIGH) && (down_state_last == LOW))
    {
      state = 1;
      privious_tick = current_tick;
    }

    else if ((up_state == HIGH) && (up_state_last == LOW))
    {
      state = 2;
      privious_tick = current_tick;
    }
    else if ((ok_state == HIGH) && (ok_state_last == LOW))
    {
      state = 3;
      privious_tick = current_tick;
    }
    else if ((comeback_state == HIGH) && (comeback_state_last == LOW))
    {
      state = 4;
      privious_tick = current_tick;
    }
    else
    {
      state = 0;
    }
    down_state_last = down_state;
    up_state_last = up_state;
    ok_state_last = ok_state;
    comeback_state_last = comeback_state;
    return state;
  }
}

/*定义一个任务，每两秒更新一次温度和湿度*/
void task_sensor(void *pt)
{
  dht.begin(); // 启动传感器
  for (;;)
  {
    sensor_current_tick = xTaskGetTickCount();
    if ((sensor_current_tick - sensor_privious_tick) >= 2000)
    {
      humidity = dht.readHumidity();       // 读取湿度
      temperature = dht.readTemperature(); // 读取摄氏度
      sensor_privious_tick = sensor_current_tick;
    }
    vTaskDelay(200);
  }
}
