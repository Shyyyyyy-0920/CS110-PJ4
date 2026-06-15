//程序入口，只负责初始化和进入app_run()
// 初始化硬件
// 初始化 LCD
// 初始化 systick
// 调用 app_run()
#include "gd32v_pjt_include.h"
#include "systick.h"
#include "utils.h"
#include "lcd/lcd.h"

#include "app.h"
int main(void) {
    Lcd_Init();

    BACK_COLOR = BLACK;
    LCD_Clear(BLACK);
    app_run();

    while (1) {
    }

    return 0;
}