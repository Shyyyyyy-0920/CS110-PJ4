//摇杆 / SW1 / SW2 输入封装
#ifndef INPUT_H
#define INPUT_H

#include "types.h"

/*
 * 初始化输入模块。
 * 主要负责配置摇杆和按键对应的 GPIO 输入模式。
 */
void input_init(void);

/*
 * 读取一次输入。
 *
 * 注意：
 * up / down / left / right / center / sw1 返回的是“按下瞬间事件”。
 * 也就是说，按住不放不会一直触发，只有从未按下变为按下时触发一次。
 *
 * sw2 目前保留为实时状态，后续用于“按住加速”。
 */
InputState input_read(void);

#endif