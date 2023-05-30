/*定义一个风扇任务，根据速度调整输出脉宽*/
void task_fan(void *pt)
{
  float count = pow(2, 12);
  ledcSetup(pwm_fan, 1000, 12);
  ledcAttachPin(Fan, pwm_fan);
  for (;;)
  {
    if (fan_mode == 0)
    {
      if (fan_speed == 0)
      {
        fan_duty = 0;
      }
      if (fan_speed == 1)
      {
        fan_duty = (int)(0.4 * count);
      }
      else if (fan_speed == 2)
      {
        fan_duty = (int)(0.6 * count);
      }
      else if (fan_speed == 3)
      {
        fan_duty = (int)(0.8 * count);
      }
      else if (fan_speed == 4)
      {
        fan_duty = (int)count;
      }
    }
    else if (fan_mode == 1)
    {
      fan_duty = map(temperature, fan_min, fan_max, (int)(0.4 * count), (int)count);
    }
    ledcWrite(pwm_fan, fan_duty);
    vTaskDelay(200);
  }
}

/*定义一个舵机任务，根据角度和速度来调整控制脉宽输出*/
void task_turn(void *pd )
{
  float count = pow(2, 14);
  int direction = 0;
  ledcSetup(pwm_turn, 50, 14);
  ledcAttachPin(Turn, pwm_turn);
  for (;;)
  {
    if (turn_angle == 0)
    {
      turn_min = 0;
      turn_max = 0;
    }
    else if (turn_angle == 4)
    {
      turn_min = (int)(0.5 / (20 / count));
      turn_max = (int)(2.5 / (20 / count));
    }
    else if (turn_angle == 3)
    {
      turn_min = (int)(2 / 3 / (20 / count));
      turn_max = (int)(7 / 3 / (20 / count));
    }
    else if (turn_angle == 2)
    {
      turn_min = (int)(5 / 6 / (20 / count));
      turn_max = (int)(13 / 6 / (20 / count));
    }
    else if (turn_angle == 1)
    {
      turn_min = (int)(1 / (20 / count));
      turn_max = (int)(2 / (20 / count));
    }
    if (turn_speed == 0)
    {
      turn_step = 0;
    }
    else if (turn_speed == 1)
    {
      turn_step = 2;
    }
    else if (turn_speed == 2)
    {
      turn_step = 10;
    }
    else if (turn_speed == 3)
    {
      turn_step = 15;
    }
    else if (turn_speed == 4)
    {
      turn_step = 20;
    }

    if (direction == 0)
    {
      turn_duty += turn_step;
    }
    else if (direction == 1)
    {
      turn_duty -= turn_step;
    }
    if (turn_duty >= turn_max)
    {
      turn_duty = turn_max;
      direction = 1;
    }
    else if (turn_duty <= turn_min)
    {
      turn_duty = turn_min;
      direction = 0;
    }
    ledcWrite(pwm_turn, turn_duty);
    vTaskDelay(20);
  }
}
