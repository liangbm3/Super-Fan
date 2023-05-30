/*定义一个任务，开机时提供密码验证*/
void task_key(void *pd)
{
  u8g2.begin();//启用u8g2库
  u8g2.setFont(u8g2_font_wqy12_t_gb2312a);
  u8g2.enableUTF8Print();
  for (;;)
  {
    int key = key_main();
    if (key == 1)
    {
      xTaskCreatePinnedToCore(task_page_index,
                              "task_page_index",
                              1024 * 100,
                              NULL,
                              1,
                              &task_page_index_handle,
                              1);
      xTaskCreatePinnedToCore(task_turn,
                              "task_turn",
                              1024 * 4,
                              NULL,
                              1,
                              NULL,
                              1);
      xTaskCreatePinnedToCore(task_fan,
                              "task_fan",
                              1024 * 5,
                              NULL,
                              1,
                              &task_fan_handle,
                              1);
      if (wifi == 1)
      {
        xTaskCreatePinnedToCore( task_wifi,
                                 "task_wifi",
                                 1024 * 20,
                                 NULL,
                                 1,
                                 &task_wifi_handle,
                                 0);
      }
      vTaskDelete(NULL);
    }
  }

}

/*密码验证的主函数，密码正确后返回1*/
int key_main()
{
  int first = 0;
  int second = 0;
  int third = 0;
  int fourth = 0;
  int one = 0;
  int two = 0;
  int three = 0;
  int four = 0;
  int state = 1;
  for (;;)
  {
    task_key_print(0);
    one = get_command();
    state = 1;
    while (state == 1 && one != 0)
    {
      task_key_print(1);
      first = one;
      two = get_command();
      while (state == 1 && two != 0)
      {
        task_key_print(2);
        second = two;
        three = get_command();
        while (state == 1 && three != 0)
        {
          task_key_print(3);
          third = three;
          four = get_command();
          if (four != 0)
          {
            task_key_print(4);
            fourth = four;
            if (first == first_key && second == second_key && third == third_key && fourth == fourth_key)
            {
              return 1;
            }
            else
            {
              print_error();
              vTaskDelay(500);
              return 0;
            }

          }
        }
      }
    }
  }
}

int change_key()
{
  int first = 0;
  int second = 0;
  int third = 0;
  int fourth = 0;
  int one = 0;
  int two = 0;
  int three = 0;
  int four = 0;
  int state = 1;
  for (;;)
  {
    task_key_print(0);
    one = get_command();
    state = 1;
    while (state == 1 && one != 0)
    {
      task_key_print(1);
      first = one;
      two = get_command();
      while (state == 1 && two != 0)
      {
        task_key_print(2);
        second = two;
        three = get_command();
        while (state == 1 && three != 0)
        {
          task_key_print(3);
          third = three;
          four = get_command();
          if (four != 0)
          {
            task_key_print(4);
            fourth = four;
            second_key = first;
            first_key = second;
            third_key = third;
            fourth_key = fourth;
            vTaskDelay(200);
            return 1;
          }
        }
      }
    }
  }
}

/*定义一个函数，在屏幕上打印密码输入提示*/
void task_key_print(int x)
{
  u8g2.clearBuffer();
  u8g2.setCursor(15, 20);
  u8g2.print("请输入四位密码:");
  for (int i = 0; i <= x; i++)
  {
    u8g2.drawDisc(10 + (i - 1) * 30, 50, 4, U8G2_DRAW_ALL);
  }
  u8g2.sendBuffer();
}
/*定义一个函数，在密码错误时弹出错误提示*/
void print_error()
{
  u8g2.clearBuffer();
  u8g2.setCursor(15, 30);
  u8g2.print("密码错误!!!");
  u8g2.sendBuffer();
  delay(700);
}
