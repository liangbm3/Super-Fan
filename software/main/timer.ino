/********************************本文件是与定时相关代码*********************************/
/*定义一个定时任务，定时开启或关闭风扇*/
void task_timer(void *pt)
{
  unsigned long openTime;
  unsigned long closeTime;
  for (;;)
  {
    if (time_open == 1)
    {
      time_open = 0;
      openTime = 1000 * (60 * 60 * open_hours + 60 * open_minutes + open_seconds);
      open_handle = xTimerCreate("open_fan",
                                 openTime,
                                 pdFALSE,
                                 (void *)0,
                                 open_fan);

      if (xTimerStart(open_handle, 3000) == pdPASS)
      {
        open_show = 1;
      }
    }
    if (time_close == 1)
    {
      time_close = 0;
      closeTime = 1000 * (60 * 60 * close_hours + 60 * close_minutes + close_seconds);
      close_handle = xTimerCreate("close_fan",
                                 closeTime,
                                 pdFALSE,
                                 (void *)1,
                                 close_fan);
      if (xTimerStart(close_handle, 3000) == pdPASS)
      {
        close_show = 1;
      }
    }

  }
}

/*风扇开启函数*/
void open_fan(TimerHandle_t xTimer)
{
  fan_speed = 4;
}

/*风扇关闭函数*/
void close_fan(TimerHandle_t xTimer)
{
  fan_speed = 0;
}
