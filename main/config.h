//引入外部库
#include <U8g2lib.h>
#include <Wire.h>
#include <WiFi.h>
#include <Preferences.h>
#include "DHT.h"
#include <ESPAsyncWebServer.h>
//引入自定义头文件
#include "page.h"

//定义四个引脚，分别为上，下，ok，返回，风扇，温湿度传感器，舵机
#define Down 4
#define Up 5
#define Ok 18
#define Comeback 26
#define Fan 33
#define Sensor 25
#define Turn 32
#define pwm_turn 1
#define pwm_fan 2
#define DHTTYPE DHT11 // 定义传感器类似 DHT11

//oled显示屏驱动函数
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

/*******************定义全局变量************************/

//菜单全局变量
short frame_w, frame_w_trg; //定义变量：框的宽度和宽度的目标值
short y, y_trg; //定义框的坐标y，框的坐标的目标值y_tyg
int menu_number = 0; //定义一个状态量，表明选中是第几个菜单，初始化为0
short menu_y, menu_y_trg; //定义菜单的y坐标及其目标值
const short menu_x = 5;
const short frame_h = 17; //初始化方框高度
const short frame_x = 4;
short y_page_first, y_page_first_trg;
short y_page_third , y_page_third_trg ;
short y_page_fifth = 10, y_page_fifth_trg = 10;
short y_page_sixth, y_page_sixth_trg ;
int32_t wifi, wifi_member;
int menu_first_number = 0;
int menu_third_number = 0;
int menu_fourth_number = 0;
int menu_fifth_number = 0;
int menu_sixth_number = 0;

//按钮变量
volatile int32_t up;
volatile int32_t down;
volatile int32_t ok;
volatile int32_t comeback;
byte down_state = HIGH;
byte up_state = HIGH;
byte ok_state = HIGH;
byte comeback_state = HIGH;
byte down_state_last = HIGH;
byte up_state_last = HIGH;
byte ok_state_last = HIGH;
byte comeback_state_last = HIGH;
unsigned long privious_tick = 0;
unsigned long current_tick;
int delay_time = 50;

//温湿度传感器变量
float humidity;
float temperature;
unsigned long sensor_current_tick;
unsigned long sensor_privious_tick = 0;


//风扇变量
volatile int32_t fan_speed;
volatile int32_t fan_duty;
volatile int32_t fan_min;
volatile int32_t fan_max;
volatile int32_t fan_mode;
String fanMode = "";
String fanSpeed = "";


//舵机变量
volatile int32_t turn_angle;
volatile int32_t turn_speed;
volatile int32_t turn_duty;
volatile int32_t turn_step;
volatile int32_t turn_min;
volatile int32_t turn_max;
String turnAngle;

//网络变量
char* ssid = "LBM";
char* password = "12345678";

//密码变量
int first_key;
int second_key;
int third_key;
int fourth_key;
int key_state;

//定时操作变量
volatile int32_t open_hours = 0;
volatile int32_t open_minutes = 0;
volatile int32_t open_seconds = 0;
volatile int32_t close_hours = 0;
volatile int32_t close_minutes = 0;
volatile int32_t close_seconds = 0;
volatile int32_t time_open = 0;
volatile int32_t time_close = 0;
volatile int32_t open_show = 0;
volatile int32_t close_show = 0;

/*******************声明********************/

//声明多任务句柄
TimerHandle_t open_handle;
TimerHandle_t close_handle;
TaskHandle_t task_key_handle = NULL;
TaskHandle_t task_sensor_handle = NULL;
TaskHandle_t task_page_index_handle = NULL;
TaskHandle_t task_page_first_handle = NULL;
TaskHandle_t task_wifi_handle = NULL;
TaskHandle_t task_fan_handle = NULL;
TaskHandle_t task_timer_handle = NULL;
TickType_t timeOut = 1000; //用于获取信号量的Timeout 1000 ticks

//声明任务
void task_welcome(void *pd);
void task_page_index(void *pd);
void task_page_first(void *pd);

//声明函数
void page_first();
void page_first_wifi_config();
void page_first_print();
void page_first_wifi_off();
void page_idex();
void print_error();
void page_second();
void page_third();



//声明 dht 函数
DHT dht(Sensor, DHTTYPE);

//定义菜单结构体
typedef struct
{
  char *str;
  byte len;
} menu_list;

//初始化
menu_list list[] =
{
  {"热点", 2},
  {"调速", 2},
  {"摇头设置", 4},
  {"模式选择", 4},
  {"本机密码", 4},
  {"定时器", 3},
  {"壁纸设置", 4},
  {"关于", 2},
};

//设置密码时用到的数组
char index_key[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
                    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
                   };
