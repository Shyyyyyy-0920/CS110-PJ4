//读取遥感，SW1,SW2
#include "input.h"

#include "utils.h"
#include "gd32vf103_libopt.h"

/*
 * 输入模块。
 *
 * 当前开发板按键映射来自 include/utils.h：
 *
 * JOY_LEFT   -> 摇杆左
 * JOY_DOWN   -> 摇杆下
 * JOY_RIGHT  -> 摇杆右
 * JOY_UP     -> 摇杆上
 * JOY_CTR    -> 摇杆中键
 * BUTTON_1   -> SW1
 * BUTTON_2   -> SW2
 *
 * Get_Button(ch) 返回 1 表示按下，0 表示未按下。
 */

/*
 * 上一次读取到的原始输入状态。
 * 用它来做“边沿检测”，避免按住一个键时菜单连续跳动。
 */
static InputState last_raw_state;

/*
 * 读取当前硬件原始状态。
 * 这个函数返回的是“当前是否正在按下”。
 */
static InputState input_read_raw(void) {
    InputState state;

    state.up     = Get_Button(JOY_UP);
    state.down   = Get_Button(JOY_DOWN);
    state.left   = Get_Button(JOY_LEFT);
    state.right  = Get_Button(JOY_RIGHT);
    state.center = Get_Button(JOY_CTR);
    state.sw1    = Get_Button(BUTTON_1);
    state.sw2    = Get_Button(BUTTON_2);

    return state;
}

/*
 * 初始化摇杆和按键对应的 GPIO。
 *
 * 根据 utils.c 里的映射：
 * JOY_LEFT  -> PA1
 * JOY_DOWN  -> PA2
 * JOY_RIGHT -> PA3
 * JOY_UP    -> PC13
 * JOY_CTR   -> PA0
 * BUTTON_1  -> PC15
 * BUTTON_2  -> PC14
 */
void input_init(void) {
    /* 使能 GPIOA 和 GPIOC 时钟 */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOC);

    /*
     * 配置 GPIOA 上的摇杆输入：
     * PA0: 中键
     * PA1: 左
     * PA2: 下
     * PA3: 右
     */
    gpio_init(GPIOA,
              GPIO_MODE_IN_FLOATING,
              GPIO_OSPEED_50MHZ,
              GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

    /*
     * 配置 GPIOC 上的摇杆和按键输入：
     * PC13: 上
     * PC14: SW2
     * PC15: SW1
     */
    gpio_init(GPIOC,
              GPIO_MODE_IN_FLOATING,
              GPIO_OSPEED_50MHZ,
              GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);

    /* 初始化 last_raw_state，避免第一次读取产生假触发 */
    last_raw_state = input_read_raw();
}

/*
 * 读取输入事件。
 *
 * 对菜单来说，我们希望“按下一次只移动一次”，所以这里把方向键、
 * 中键、SW1 都处理成边沿触发：
 *
 * 当前为 1 且上一次为 0 -> 这次返回 1
 * 其他情况 -> 返回 0
 *
 * SW2 后续用于游戏加速，因此保留为实时状态。
 */
InputState input_read(void) {
    InputState raw = input_read_raw();
    InputState event;

    event.up     = raw.up     && !last_raw_state.up;
    event.down   = raw.down   && !last_raw_state.down;
    event.left   = raw.left   && !last_raw_state.left;
    event.right  = raw.right  && !last_raw_state.right;
    event.center = raw.center && !last_raw_state.center;
    event.sw1    = raw.sw1    && !last_raw_state.sw1;

    /*
     * SW2 暂时返回实时状态。
     * 后续在 game.c 中可以用它判断是否进入高速移动。
     */
    event.sw2    = raw.sw2;

    last_raw_state = raw;

    return event;
}