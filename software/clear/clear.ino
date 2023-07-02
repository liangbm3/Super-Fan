#include <Preferences.h>

void setup() {
    Serial.begin(115200);
    Serial.println();
    delay(2000);

    Preferences data;
    data.begin("data_space");
    data.clear(); // 删除当前命名空间中的所有元素
    Serial.printf("0k");

    data.end();
}

void loop() {}
