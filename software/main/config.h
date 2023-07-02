/*该文件为配置文件*/

/*引入外部库*/
#include <U8g2lib.h>//u8g2库，开源地址：https://github.com/olikraus/u8g2
#include <WiFi.h>//WiFi库，地址：https://www.arduino.cc/reference/en/libraries/wifi
#include <Preferences.h>//存储库，地址：https://github.com/vshymanskyy/Preferences
#include "DHT.h"//DHT传感器库，开源地址：https://github.com/adafruit/DHT-sensor-library
#include <ESPAsyncWebServer.h>//异步服务器库，开源地址：https://github.com/dvarrel/AsyncTCP

/*引入储存图片信息的自定义库*/
#include "page.h"

/*引脚定义*/
#define Down 4//按键down
#define Up 5//按键up
#define Ok 18//按键on
#define Comeback 26//按键comeback
#define Fan 33//风扇控制引脚
#define Sensor 25//传感器数据传输引脚
#define Turn 32//舵机控制引脚

/*LEDC通道配置*/
#define pwm_turn 1
#define pwm_fan 2

/*传感器类型定义*/
#define DHTTYPE DHT11 // 定义传感器类似 DHT11

/*oled显示屏驱动函数*/
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

/*******************定义全局变量************************/

/*菜单全局*/
short frame_w, frame_w_trg; //框的宽度，框的宽度的目标值
short y, y_trg; //框的坐标y，框的坐标的目标值y_tyg
int menu_number = 0; //选中的菜单序号
short menu_y, menu_y_trg; //菜单的y坐标，菜单y坐标的目标值
const short menu_x = 5;//菜单x坐标
const short frame_h = 17; //框的高度
const short frame_x = 4;//框的x坐标
short y_page_first, y_page_first_trg;//第一个二级菜单的y坐标，第一个二级菜单的y坐标的坐标值
short y_page_third , y_page_third_trg ;//第三个二级菜单的y坐标，第三个二级菜单的y坐标的目标值
short y_page_fifth = 10, y_page_fifth_trg = 10;//第五个二级菜单的y坐标，第五个二级菜单y坐标的目标值
short y_page_sixth, y_page_sixth_trg ;//第六个二级菜单的y坐标，第六个二级菜单的y坐标的目标值
short y_page_seventh, y_page_seventh_trg ;//第七个二级菜单的y坐标，第七个二级菜单y坐标的目标值
int32_t wifi;//WiFi状态量，
int menu_first_number = 0;//一级菜单选择的菜单序号
int menu_third_number = 0;//三级菜单选择的菜单序号
int menu_fourth_number = 0;//四级菜单选择的菜单序号
int menu_fifth_number = 0;//五级菜单选择的菜单序号
int menu_sixth_number = 0;//六级菜单选择的菜单序号
int menu_seventh_number = 0;//七级菜单选择的菜单序号

/*温湿度传感器变量*/
float humidity;//湿度值
float temperature;//温度值

/*风扇变量*/
volatile int32_t fan_speed;//风速
volatile int32_t fan_mode;//风扇模式
  volatile int32_t fan_duty;
  byte down_state = HIGH;
byte up_state = HIGH;
byte ok_state = HIGH;
byte comeback_state = HIGH;
byte down_state_last = HIGH;
byte up_state_last = HIGH;
byte ok_state_last = HIGH;
byte comeback_state_last = HIGH;
volatile int32_t fan_min = 24;
volatile int32_t fan_max = 30;
String fanMode = "";
String fanSpeed = "";


/*舵机变量*/
volatile int32_t turn_angle;//转动角度
volatile int32_t turn_speed;//转动速度
volatile int32_t turn_duty;
volatile int32_t turn_step;
volatile int32_t turn_min;
volatile int32_t turn_max;
String turnAngle;

/*网络变量*/
char* ssid = "LCW小风扇";
char* password = "12345678";

/*密码变量*/
int first_key;
int second_key;
int third_key;
int fourth_key;
int key_state;

/*定时操作变量*/
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
int Stateh;

/*壁纸变量*/
volatile int32_t photos = 1;
volatile int32_t photos_state = 0;

/*******************声明********************/

/*声明多任务句柄*/
TimerHandle_t open_handle;
TimerHandle_t close_handle;
TaskHandle_t task_key_handle = NULL;
TaskHandle_t task_sensor_handle = NULL;
TaskHandle_t task_page_index_handle = NULL;
TaskHandle_t task_wifi_handle = NULL;
TaskHandle_t task_fan_handle = NULL;
TaskHandle_t task_timer_handle = NULL;
TickType_t timeOut = 1000;

/*声明任务*/
void task_welcome(void *pd);
void task_page_index(void *pd);
void task_timer(void *pt);
void task_data_save(void *pt);
void task_key(void *pd);
void task_wifi(void *pt);
void task_sensor(void *pt);
void task_fan(void *pt);
void task_turn(void *pd );

/*声明函数*/
void task_main(void);
void menu_draw(void);
int move(short *x, short *x_trg, short step);
void page_idex(void);
void page_first_print(void);
void page_first_wifi_config(void);
void page_first_wifi_off(void);
void page_first_wifi_set(void);
void page_first(void);
void page_second(void);
void page_third(void);
void page_fourth(void);
void page_fifth(void);
void page_sixth(void);
void page_seventh(void);
void page_eighth(void);

void open_fan(TimerHandle_t xTimer);
void close_fan(TimerHandle_t xTimer);

int key_main(void);
int change_key(void);
void task_key_print(int x);
void print_error(void);

void handleRootPage(AsyncWebServerRequest *request) ;
void handleDataRequest(AsyncWebServerRequest *request);
void handleCommandRequest(AsyncWebServerRequest *request);

int32_t get_command(void);

/*声明 dht 函数*/
DHT dht(Sensor, DHTTYPE);

/*定义菜单结构体*/
typedef struct
{
  char *str;
  byte len;
} menu_list;

/*结构体初始化*/
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
