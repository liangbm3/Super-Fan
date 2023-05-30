/*定义一个任务，每一秒更新一次数据，存入nsv*/
void task_data_save(void *pt)
{
  unsigned long current;
  unsigned long  last = 0;
  int32_t fan_speeds;
  int32_t turn_angles;
  int32_t turn_speeds;
  int32_t wifis;
  int32_t fan_modes;
  int32_t first_keys;
  int32_t second_keys;
  int32_t third_keys;
  int32_t fourth_keys;
  String password_string;
  String password_strings;
  Preferences data;
  data.begin("data_space");
  fan_speed = data.getInt("fan_speed", 0);
  turn_angle = data.getInt("turn_angle", 0);
  turn_speed = data.getInt("turn_speed", 0);
  wifi = data.getInt("wifi", 0);
  fan_mode = data.getInt("fan_mode", 0);
  first_key = data.getInt("first_key", 2);
  second_key = data.getInt("second_key", 1);
  third_key = data.getInt("third_key", 3);
  fourth_key = data.getInt("fourth_key", 4);
  password_string = data.getString("password", "12345678");
  const char* password_cstr = password_string.c_str();
  password = new char[strlen(password_cstr) + 1]; // 分配内存
  strcpy(password, password_cstr); // 复制字符串

  for (;;)
  {
    current = xTaskGetTickCount();
    if ((current - last) > 1000)
    {
      fan_speeds = data.getInt("fan_speed", 0);
      turn_angles = data.getInt("turn_angle", 0);
      turn_speeds = data.getInt("turn_speed", 0);
      wifis = data.getInt("wifi", 0);
      fan_modes = data.getInt("fan_mode", 0);
      first_keys = data.getInt("first_key", 2);
      second_keys = data.getInt("second_key", 1);
      third_keys = data.getInt("third_key", 3);
      fourth_keys = data.getInt("fourth_key", 4);
      password_strings = data.getString("fourth_key", "12345678");
      password_string = password;
      if (fan_speeds != fan_speed)
      {
        fan_speeds = fan_speed;
        data.putInt("fan_speed", fan_speeds);
      }
      else if (turn_speeds != turn_speed)
      {
        turn_speeds = turn_speed;
        data.putInt("turn_speed", turn_speeds);
      }
      else if (turn_angles != turn_angle)
      {
        turn_angles = turn_angle;
        data.putInt("turn_angle", turn_angles);
      }
      else if (wifis != wifi )
      {
        wifis = wifi;
        data.putInt("wifi", wifis);
      }
      else if (fan_modes != fan_mode)
      {
        fan_modes = fan_mode;
        data.putInt("fan_mode", fan_modes);
      }
      else if (first_keys != first_key)
      {
        first_keys = first_key;
        data.putInt("first_key", first_key);
      }
      else if (second_keys != second_key)
      {
        second_keys = second_key;
        data.putInt("second_key", second_key);
      }
      else if (third_keys != third_key)
      {
        third_keys = third_key;
        data.putInt("third_key", third_key);
      }
      else if (fourth_keys != fourth_key)
      {
        fourth_keys = fourth_key;
        data.putInt("fourth_key", fourth_key);
      }
      else if (password_strings != password_string)
      {
        password_strings = password_string;
        data.putString("password", password_string);
      }
      last = current;
    }
    vTaskDelay(500);
  }
  data.end();
}
