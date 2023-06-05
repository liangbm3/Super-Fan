#include "config.h"


//定义一个用于绘制菜单的函数


void setup(void)//初始化
{
  Serial.begin(115200);
  pinMode(Down, INPUT_PULLUP);
  pinMode(Up, INPUT_PULLUP);//输入上拉模式
  pinMode(Ok, INPUT_PULLUP);
  pinMode(Comeback, INPUT_PULLUP);
  pinMode(Fan, OUTPUT);
  xTaskCreatePinnedToCore(task_welcome,
                          "task_welcome",
                          1024 * 8,
                          NULL,
                          1,
                          NULL,
                          1);
  xTaskCreatePinnedToCore(task_data_save,
                          "task_data_save",
                          1024 * 8,
                          NULL,
                          1,
                          NULL,
                          1);

}
void loop(void)
{
  Serial.println(y_page_sixth);
  Serial.println(y_page_sixth_trg);
  Serial.println(menu_fourth_number);
  Serial.println(password);
  Serial.println(wifi);
  vTaskDelay(1000);
}
