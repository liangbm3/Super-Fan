/******************************本文件是与OLED屏幕显示相关代码*******************************/

/*定义一个任务，开机显示欢迎界面，并开启密码验证任务，传感器任务，风扇任务，舵机任务*/
void task_welcome(void *pd)
{
  u8g2.begin();//启用u8g2库
  unsigned long  now = xTaskGetTickCount();//获取当前时钟时间
  for (;;)//一直循环2s
  {
    u8g2.clearBuffer();//清屏
    u8g2.drawXBMP(0, 0, 128, 64, welcome_page);//绘制欢迎界面
    u8g2.sendBuffer();//发送缓存
    if (xTaskGetTickCount() - now > 2000)//判断时间是否大于2s
    {
      //开启传感器任务
      xTaskCreatePinnedToCore(task_sensor,
                              "task_sensor",
                              1024 * 4,
                              NULL,
                              1,
                              &task_sensor_handle,
                              1);
      //开启定时任务
      xTaskCreatePinnedToCore(task_timer,
                              "task_timer",
                              1024 * 5,
                              NULL,
                              1,
                              &task_timer_handle,
                              1);

      if (key_state == 1)//判断是否设置了密码
      {
        //开启密码验证任务
        xTaskCreatePinnedToCore(task_key,
                                "task_key",
                                1024 * 5,
                                NULL,
                                1,
                                &task_key_handle,
                                1);

      }
      else
      {
        //开启显示任务
        xTaskCreatePinnedToCore(task_page_index,
                                "task_page_index",
                                1024 * 60,
                                NULL,
                                1,
                                &task_page_index_handle,
                                1);
        //开启舵机任务
        xTaskCreatePinnedToCore(task_turn,
                                "task_turn",
                                1024 * 4,
                                NULL,
                                1,
                                NULL,
                                1);
        //开启风扇任务
        xTaskCreatePinnedToCore(task_fan,
                                "task_fan",
                                1024 * 5,
                                NULL,
                                1,
                                &task_fan_handle,
                                1);
        if (wifi == 1)//判断WiFi是否开启
        {
          //开启WiFi任务
          xTaskCreatePinnedToCore( task_wifi,
                                   "task_wifi",
                                   1024 * 20,
                                   NULL,
                                   1,
                                   &task_wifi_handle,
                                   0);
        }
      }
      vTaskDelete(NULL);//删除任务
    }
  }

}

/*用来OLED主页显示*/
void task_main()
{
  u8g2.clearBuffer();//清屏
  switch (photos)//判断哪张壁纸
  {
    case 1:
      u8g2.drawXBMP(0, 0, 64, 64, main_1);
      break;
    case 2:
      u8g2.drawXBMP(0, 0, 64, 64, main_2);
      break;
    case 3:
      u8g2.drawXBMP(0, 0, 64, 64, main_3);
      break;
    case 4:
      u8g2.drawXBMP(0, 0, 64, 64, main_4);
      break;
    case 5:
      u8g2.drawXBMP(0, 0, 64, 64, main_5);
      break;
    case 6:
      u8g2.drawXBMP(0, 0, 64, 64, main_6);
      break;
    case 7:
      u8g2.drawXBMP(0, 0, 64, 64, main_7);
      break;
    case 8:
      u8g2.drawXBMP(0, 0, 64, 64, main_8);
      break;
    case 9:
      u8g2.drawXBMP(0, 0, 64, 64, main_9);
      break;
    case 10:
      u8g2.drawXBMP(0, 0, 64, 64, main_10);
      break;
    case 11:
      u8g2.drawXBMP(0, 0, 64, 64, main_11);
      break;
  }
  //温度、湿度显示
  u8g2.setCursor(75, 15);
  u8g2.print("温度/℃");
  u8g2.setCursor(75, 15 + 16);
  u8g2.print(temperature);
  u8g2.setCursor(75, 15 + 16 * 2);
  u8g2.print("湿度/%");
  u8g2.setCursor(75, 15 + 16 * 3);
  u8g2.print(humidity);
  u8g2.sendBuffer();
  int command = get_command();//获取按键数据
  if (command != 0)
  {
    Stateh = 1;
  }

}

/*定义一个任务，控制OLED显示*/
void task_page_index(void *pd)
{
  u8g2.begin();//启用u8g2库
  u8g2.setFont(u8g2_font_wqy12_t_gb2312a);//设置字体
  u8g2.enableUTF8Print();//允许中文显示
  frame_w = list[menu_number].len * 12; //初始化方框宽度
  frame_w_trg = list[menu_number].len * 12; //初始化方框目标值
  for (;;)
  {
    u8g2.clearBuffer();
    switch (Stateh)
    {
      case 0:
        task_main();
        break;

      case 1:
        page_idex();
        break;
    }
    u8g2.sendBuffer();
    vTaskDelay(200);
  }
}

/*定义一个绘出菜单的函数*/
void menu_draw(void)
{
  int size = sizeof(list) / sizeof(menu_list);
  short c = menu_y;
  for (int i = 0; i < size; i = i++)
  {
    u8g2.setCursor(menu_x , (i + 1) * 20 - menu_y - 4);
    u8g2.print(list[i].str);
    i = i + 1;
  }
}

/*定义一个移动函数，使原指往目标值移动*/
int move(short *x, short *x_trg, short step)
{
  short Step;
  if (*x - *x_trg > -step && *x - *x_trg < step)
  {
    Step = abs(*x - *x_trg);
  }
  else
  {
    Step = step;
  }
  if (*x < *x_trg)
  {
    *x += Step;
  }
  else if (*x > *x_trg)
  {
    *x -= Step;
  }
  else
  {
    return 0;
  }
  return 1;
}


/*定义一个函数，用于显示菜单目录，并根据命令进入下一级*/
void page_idex()
{

  short frame = move(&frame_w, &frame_w_trg, 10);
  short loc = move(&y, &y_trg, 8);
  short menu_ok = move(&menu_y, &menu_y_trg, 8);
  int command = get_command();
  if (menu_number == 0 && command == 3)
  {
    command = 0;
    page_first();
  }
  else if (menu_number == 1 && command == 3)
  {
    command = 0;
    page_second();
  }
  else if (menu_number == 2 && command == 3)
  {
    command = 0;
    page_third();
  }
  else if (menu_number == 3 && command == 3)
  {
    command = 0;
    page_fourth();
  }
  else if (menu_number == 4 && command == 3)
  {
    command = 0;
    for (;;)
    {
      int key = key_main();
      if (key == 1)
      {
        break;
      }
    }
    page_fifth();
  }
  else if (menu_number == 5 && command == 3)
  {
    command = 0;
    page_sixth();
  }
  else if (menu_number == 6 && command == 3)
  {
    command = 0;
    page_seventh();
  }
  else if (menu_number == 7 && command == 3)
  {
    command = 0;
    page_eighth();
  }
  if (y >= 0 && y < 40 && frame == 0 && loc == 0 && menu_ok == 0)
  {
    y = y_trg;
    if (command == 2 && menu_number != 0)
    {
      y_trg = y_trg - 20;
      command = 0;
      menu_number = menu_number - 1;
    }
    else if (command == 1)
    {
      y_trg = y_trg + 20;
      command = 0;
      menu_number = menu_number + 1;
    }
  }
  else if (menu_number == 2 && y == 40 && frame == 0 && loc == 0 && menu_ok == 0)
  {
    if (command == 2)
    {
      y = y_trg;
      y_trg = y_trg - 20;
      command = 0;
      menu_number = menu_number - 1;
    }
    else if (command == 1)
    {
      menu_y = menu_y_trg;
      menu_y_trg = menu_y_trg + 20;
      command = 0;
      menu_number = menu_number + 1;
    }
  }
  else if (menu_number > 2 && menu_number < 8 && frame == 0 && loc == 0 && menu_ok == 0)
  {
    if (command == 1 && menu_number != 7)
    {
      menu_y = menu_y_trg;
      menu_y_trg = menu_y_trg + 20;
      command = 0;
      menu_number = menu_number + 1;
    }
    else if (command == 2)
    {
      menu_y = menu_y_trg;
      menu_y_trg = menu_y_trg - 20;
      command = 0;
      menu_number = menu_number - 1;
    }
  }
  if (command == 4)
  {
    Stateh = 0;
  }

  menu_draw();
  frame_w_trg = list[menu_number].len * 12 + 2;
  u8g2.drawRFrame(frame_x, y + 3, frame_w, frame_h, 2);
}

/*定义一个函数，绘制菜单一的页面，并根据命令进入下一级*/
void page_first()
{
  for (;;)
  {
    u8g2.clearBuffer();
    short location = move(&y_page_first, &y_page_first_trg, 6);
    page_first_print();

    u8g2.drawRFrame(29, y_page_first, 26, 20, 2);
    u8g2.sendBuffer();
    if (location == 0)
    {
      int command = get_command();

      if (command == 2 && menu_first_number != 0)
      {
        y_page_first = y_page_first_trg;
        y_page_first_trg = y_page_first_trg - 20;
        menu_first_number = menu_first_number - 1;
        command = 0;
      }
      else if (command == 1 && menu_first_number != 2)
      {
        y_page_first = y_page_first_trg;
        y_page_first_trg = y_page_first_trg + 20;
        menu_first_number = menu_first_number + 1;
        command = 0;
      }
      else if (command == 3 && menu_first_number == 0 && wifi == 0)
      {
        xTaskCreatePinnedToCore( task_wifi,
                                 "task_wifi",
                                 1024 * 4,
                                 NULL,
                                 1,
                                 &task_wifi_handle,
                                 0);
        wifi = 1;
        command = 0;
        for (;;)
        {
          command = get_command();
          u8g2.clearBuffer();
          page_first_wifi_config();
          u8g2.sendBuffer();
          if (command == 4)
          {
            command = 0;
            break;
          }
        }
      }
      else if (command == 3 && menu_first_number == 0 && wifi == 1)
      {
        vTaskDelete(task_wifi_handle);
        wifi = 0;
        command = 0;
        page_first_wifi_off();
      }
      else if (command == 3 && menu_first_number == 1)
      {
        command = 0;
        for (;;)
        {
          int key = key_main();
          if (key == 1)
          {
            break;
          }
        }
        page_first_wifi_set();
      }
      else if (command == 3 && menu_first_number == 2)
      {
        for (;;)
        {
          command = get_command();
          u8g2.clearBuffer();
          page_first_wifi_config();
          u8g2.sendBuffer();
          if (command == 4)
          {
            command = 0;
            break;
          }
        }
      }
      else if (command == 4)
      {

        command = 0;
        break;
      }

    }
  }
}

/*定义一个函数，在oled输出相应的文字信息*/
void page_first_print()
{
  u8g2.setCursor(0, 15);
  u8g2.print("选项");
  u8g2.setCursor(30, 15);
  u8g2.print("开关");
  u8g2.setCursor(30, 35);
  u8g2.print("设置");
  u8g2.setCursor(30, 55);
  u8g2.print("信息");
  u8g2.setCursor(65, 15);
  u8g2.print("连接数：");
  u8g2.setCursor(114, 15);
  u8g2.print(WiFi.softAPgetStationNum());
  u8g2.setCursor(70, 33);
  u8g2.print("状态：");
  u8g2.setCursor(75, 49);
  if (wifi == 1)
  {
    u8g2.print("ON");
  }
  else
  {
    u8g2.print("OFF");
  }
}

/*定义一个菜单一的二级页面，显示WiFi信息*/
void page_first_wifi_config()
{
  u8g2.clearBuffer();
  u8g2.setCursor(10, 15);
  u8g2.print("热点名称: ");
  u8g2.setCursor(10, 15 + 16);
  u8g2.print(ssid);
  u8g2.setCursor(10, 15 + 16 * 2);
  u8g2.print("ip地址: ");
  u8g2.setCursor(10, 15 + 16 * 3);
  if (wifi == 1)
  {
    u8g2.print( WiFi.softAPIP());
  }
  else
  {
    u8g2.print("请先开启热点");
  }
  u8g2.sendBuffer();
}

/*定义一个函数来在oled显示错误信息，在WiFi关闭时弹出*/
void page_first_wifi_off()
{
  u8g2.clearBuffer();
  u8g2.setCursor(25, 30);
  u8g2.print("热点已关闭");
  u8g2.sendBuffer();
  vTaskDelay(200);
}

/*定义一个菜单一的二级页面，进行WiFi相关设置*/
void page_first_wifi_set()
{
  int i = 0;
  int command = 0;
  int number = 0;
  int letter = 0;
  char drr[100];

  int x;
  for (int m = 0; m <= 15; m++)
  {
    drr[m] = '0';
  }
  for (;;)
  {
    u8g2.clearBuffer();
    u8g2.setCursor(40, 15);
    u8g2.print("更改密码");

    u8g2.drawRFrame(1, 20, 120, 20, 2);
    u8g2.setCursor(25, 60);
    u8g2.print("当前位数:");
    u8g2.setCursor(90, 60);
    u8g2.print(number + 1);

    for (int m = 0; m <= 15; m++)
    {
      u8g2.setCursor(3 + m * 8, 35);
      u8g2.print(drr[m]);
    }
    u8g2.sendBuffer();
    drr[number] = index_key[letter];
    command = get_command();
    if (command == 3)
    {
      number = number + 1;
      command = 0;
    }
    else if (command == 1)
    {
      letter = letter - 1;
      command = 0;
    }
    else if (command == 2)
    {
      letter = letter + 1;
      command = 0;
    }
    else if (command == 4)
    {
      command = 0;
      password = (char*)malloc(4 * sizeof(char)); // 分配足够的内存，包括null字符
      for (int m = 0; m <= number - 1; m++)
      {
        password[m] = drr[m]; // 将数据复制到分配的内存中
      }
      password[number] = '\0';

      break;
    }
  }
}

/*定义菜单二，显示风速并调速*/
void page_second()
{
  int command = 0;
  for (;;)
  {
    u8g2.clearBuffer();
    u8g2.setCursor(40, 15);
    u8g2.print("当前挡位");
    u8g2.setCursor(60, 35);
    u8g2.print(fan_speed);
    u8g2.sendBuffer();
    command = get_command();
    if (command == 1 && fan_speed != 0)
    {
      fan_speed = fan_speed - 1;
    }
    else if (command == 2 && fan_speed != 4)
    {
      fan_speed = fan_speed + 1;
    }
    if (command == 4)
    {
      break;
    }
  }
}

/*定义菜单三，，显示舵机角度和速度并调节*/
void page_third()
{
  char* angle;
  char* speed;
  int set = 0;
  for (;;)
  {
    if (turn_angle == 0)
    {
      angle = "未开启";
    }
    else if (turn_angle == 4)
    {
      angle = "180°";
    }
    else if (turn_angle == 3)
    {
      angle = "150°";
    }
    else if (turn_angle == 2)
    {
      angle = "120°";
    }
    else if (turn_angle == 1)
    {
      angle = "90°";
    }
    if (turn_speed == 0)
    {
      speed = "未开启";
    }
    else if (turn_speed == 1)
    {
      speed = "一档";
    }
    else if (turn_speed == 2)
    {
      speed = "二档";
    }
    else if (turn_speed == 3)
    {
      speed = "三档";
    }
    else if (turn_speed == 4)
    {
      speed = "四档";
    }
    u8g2.clearBuffer();
    short location = move(&y_page_third, &y_page_third_trg, 6);
    u8g2.setCursor(5, 15);
    u8g2.print("角度范围");
    u8g2.setCursor(65, 15);
    u8g2.print(angle);
    u8g2.setCursor(5, 35);
    u8g2.print("摇头速度");
    u8g2.setCursor(65, 35);
    u8g2.print(speed);
    u8g2.drawRFrame(4, y_page_third, 54, 20, 2);
    if (set == 1)
    {
      u8g2.setCursor(5, 55);
      u8g2.print("正在设置角度范围");
    }
    else if (set == 2)
    {
      u8g2.setCursor(5, 55);
      u8g2.print("正在设置摇头速度");
    }
    u8g2.sendBuffer();
    if (location == 0)
    {
      int command = get_command();
      if (command == 1 && set == 0 && menu_third_number != 1)
      {
        y_page_third = y_page_third_trg;
        y_page_third_trg = y_page_third_trg + 20;
        menu_third_number = menu_third_number + 1;
        command = 0;
      }
      else if (command == 2 && set == 0 && menu_third_number != 1)
      {
        y_page_third = y_page_third_trg;
        y_page_third_trg = y_page_third_trg - 20;
        menu_third_number = menu_third_number - 1;
        command = 0;
      }
      else if (command == 3 && menu_third_number == 0)
      {
        command = 0;
        set = 1;
      }
      else if (command == 3 && menu_third_number == 1)
      {
        command = 0;
        set = 2;
      }
      else if (command == 1 && set == 1)
      {
        command = 0;
        turn_angle = turn_angle - 1;
      }
      else if (command == 2 && set == 1)
      {
        command = 0;
        turn_angle = turn_angle + 1;
      }
      else if (command == 1 && set == 2)
      {
        command = 0;
        turn_speed = turn_speed - 1;
      }
      else if (command == 2 && set == 2)
      {
        command = 0;
        turn_speed = turn_speed + 1;
      }
      else if (command == 4 && set == 1)
      {
        command = 0;
        set = 0;
      }
      else if (command == 4 && set == 2)
      {
        command = 0;
        set = 0;
      }
      else if (command == 4 && set == 0)
      {
        command = 0;
        break;
      }
    }
  }
}

/*显示菜单四，来设置风扇模式*/
void page_fourth()
{
  int command = 0;
  char *mode;
  menu_fourth_number = 0;
  for (;;)
  {
    if (menu_fourth_number == 0)
    {
      mode = "手动模式";
      fan_mode = 0;
    }
    else if (menu_fourth_number == 1)
    {
      mode = "自动模式";
      fan_mode = 1;
    }
    command = get_command();
    if (command == 1 || command == 2)
    {
      command = 0;
      if (menu_fourth_number == 0)
      {
        menu_fourth_number = 1;
      }
      else
      {
        menu_fourth_number = 0;
      }

    }
    else if (command == 4)
    {
      command = 0;
      break;
    }
    u8g2.clearBuffer();
    u8g2.setCursor(40, 30);
    u8g2.print(mode);
    u8g2.sendBuffer();
  }
}

/*显示菜单五，设置密码*/
void page_fifth(void)
{
  int command;
  char* state;
  for (;;)
  {
    if (key_state == 0)
    {
      state = "关闭";
    }
    else if (key_state == 1)
    {
      state = "开启";
    }
    short location = move(&y_page_fifth, &y_page_fifth_trg, 6);
    u8g2.clearBuffer();
    u8g2.setCursor(10, 25);
    u8g2.print("状态");
    u8g2.setCursor(40, 25);
    u8g2.print(state);
    u8g2.setCursor(40, 45);
    u8g2.print("更改");
    u8g2.drawRFrame(39, y_page_fifth, 26, 20, 2);
    u8g2.sendBuffer();
    command = get_command();
    if (command == 1)
    {
      command = 0;
      y_page_fifth;
      y_page_fifth_trg = y_page_fifth + 20;
      menu_fifth_number = menu_fifth_number + 1;
    }
    else if (command == 2)
    {
      command = 0;
      y_page_fifth;
      y_page_fifth_trg = y_page_fifth - 20;
      menu_fifth_number = menu_fifth_number - 1;
    }
    else if (command == 3 && menu_fifth_number == 0)
    {
      command = 0;
      if (key_state == 0)
      {
        key_state = 1;
      }
      else if (key_state == 1)
      {
        key_state = 0;
      }
    }
    else if (command == 3 && menu_fifth_number == 1)
    {
      command = 0;
      change_key();
    }
    else if (command == 4)
    {
      command = 0;
      break;
    }
  }
}

/*显示菜单六，定时操作*/
void page_sixth()
{
  int command;
  int time = 0;
  int set_time = 0;
  int set_open = 0;
  int set_close = 0;
  char* close_dw;
  char* open_dw;
  for (;;)
  {
    Serial.println(set_open);
    Serial.println(command);
    u8g2.clearBuffer();
    short location = move(&y_page_sixth, &y_page_sixth_trg, 6);
    u8g2.setCursor(5, 15);
    u8g2.print("定时关闭");
    u8g2.setCursor(65, 15);
    u8g2.print(close_hours);
    u8g2.setCursor(85, 15);
    u8g2.print(close_minutes);
    u8g2.setCursor(105, 15);
    u8g2.print(close_seconds);
    u8g2.setCursor(5, 35);
    u8g2.print("定时打开");
    u8g2.setCursor(65, 35);
    u8g2.print(open_hours);
    u8g2.setCursor(85, 35);
    u8g2.print(open_minutes);
    u8g2.setCursor(105, 35);
    u8g2.print(open_seconds);
    u8g2.drawRFrame(4, y_page_sixth, 54, 20, 2);
    if (set_time == 2)
    {
      switch (set_close)
      {
          Serial.println(set_close);
        case 0:
          close_dw = "";
          break;
        case 1:
          close_dw = "时";
          break;
        case 2:
          close_dw = "分";
          break;
        case 3:
          close_dw = "秒";
          break;
      }
      u8g2.setCursor(5, 55);
      u8g2.print("定时关闭,单位:");
      u8g2.setCursor(100, 55);
      u8g2.print(close_dw);

    }
    else if (set_time == 1)
    {
      switch (set_open)
      {
        case 0:
          open_dw = "";
          break;
        case 1:
          open_dw = "h";
          break;
        case 2:
          open_dw = "min";
          break;
        case 3:
          open_dw = "s";
          break;
      }
      u8g2.setCursor(5, 55);
      u8g2.print("定时打开,单位:");
      u8g2.setCursor(100, 55);
      u8g2.print(open_dw);

    }
    u8g2.sendBuffer();
    if (command == 4)
    {
      command = 0;
      break;
    }
    command = get_command();
    if (command == 1 && set_time == 0 && location == 0 &&  menu_sixth_number != 1)
    {
      command = 0;
      y_page_sixth = y_page_sixth_trg;
      y_page_sixth_trg = y_page_sixth_trg + 20;
      menu_sixth_number = menu_sixth_number + 1;
    }
    else if (command == 2 && set_time == 0 && location == 0 && menu_sixth_number != 0)
    {
      command = 0;
      y_page_sixth = y_page_sixth_trg;
      y_page_sixth_trg = y_page_sixth_trg - 20;
      menu_sixth_number = menu_sixth_number - 1;
    }
    else if (command == 3 && menu_sixth_number == 0 && set_close == 0 && location == 0)
    {
      command = 0;
      set_time = 2;
      set_close = 1;
    }
    else if (command == 3 && menu_sixth_number == 1 && set_open == 0 && location == 0)
    {
      command = 0;
      set_time = 1;
      set_open = 1;
    }
    else if (command == 3 && menu_sixth_number == 0 && set_close != 0 && set_close < 4)
    {
      command = 0;
      set_close = set_close + 1;
    }
    else if (command == 3 && menu_sixth_number == 1 && set_open != 0 && set_open < 4)
    {
      command = 0;
      set_open = set_open + 1;
    }
    else if (command == 3 && menu_sixth_number == 0 && set_close == 4)
    {
      time_close = 1;
      command = 4;
    }
    else if (command == 3 && menu_sixth_number == 1 && set_open == 4)
    {
      time_open = 1;
      command = 4;
    }
    else if (command == 1 && set_time == 1)
    {
      command = 0;
      switch (set_open)
      {
        case 1:
          open_hours = open_hours - 1;
          break;
        case 2:
          open_minutes = open_minutes - 10;
          break;
        case 3:
          open_seconds = open_seconds - 1;
          break;
      }
    }
    else if (command == 2 && set_time == 1)
    {
      command = 0;
      switch (set_open)
      {
        case 1:
          open_hours = open_hours + 1;
          break;
        case 2:
          open_minutes = open_minutes + 10;
          break;
        case 3:
          open_seconds = open_seconds + 1;
          break;
      }
    }
    else if (command == 1 && set_time == 2)
    {
      command = 0;
      switch (set_close)
      {
        case 1:
          close_hours = close_hours - 1;
          break;
        case 2:
          close_minutes = close_minutes - 10;
          break;
        case 3:
          close_seconds = close_seconds - 1;
          break;
      }
    }
    else if (command == 2 && set_time == 2)
    {
      command == 0;
      switch (set_close)
      {
        case 1:
          close_hours = close_hours + 1;
          break;
        case 2:
          close_minutes = close_minutes + 10;
          break;
        case 3:
          close_seconds = close_seconds + 1;
          break;
      }
    }

  }

}

/*显示菜单七，壁纸设置*/
void page_seventh()
{
  int command = 0;
  for (;;)
  {
    u8g2.clearBuffer();
    u8g2.setCursor(40, 15);
    u8g2.print("当前壁纸");
    u8g2.setCursor(60, 35);
    u8g2.print(photos);
    u8g2.sendBuffer();
    command = get_command();
    if (command == 1 && photos != 0)
    {
      photos = photos - 1;
    }
    else if (command == 2 && photos != 11)
    {
      photos = photos + 1;
    }
    if (command == 4)
    {
      break;
    }
  }
}

/*显示菜单八，本机信息*/
void page_eighth()
{
  for (;;)
  {
    u8g2.clearBuffer();
    u8g2.setCursor(10, 15);
    u8g2.print("名称:");
    u8g2.setCursor(10, 15 + 16);
    u8g2.print("LCW小风扇");
    u8g2.setCursor(10, 15 + 16 * 2);
    u8g2.print("版本:");
    u8g2.setCursor(10, 15 + 16 * 3);
    u8g2.print("v1.0.0 ");
    u8g2.setCursor(10, 15 + 16 * 4);
    u8g2.sendBuffer();
    if (get_command() == 4)
    {
      break;
    }
  }
}
