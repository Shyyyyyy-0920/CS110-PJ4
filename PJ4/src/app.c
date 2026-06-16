//主循环和状态切换
// 主循环和状态切换
#include "app.h"

#include "types.h"
#include "input.h"
#include "render.h"
#include "game.h"
#include "systick.h"

/*
 * 菜单选择上一项。
 * 当前是 1-1 时，再向上会循环到 1-3。
 */
static LevelId app_menu_prev_level(LevelId level) {
    if (level == LEVEL_1_1) {
        return LEVEL_1_3;
    } else if (level == LEVEL_1_2) {
        return LEVEL_1_1;
    } else {
        return LEVEL_1_2;
    }
}

/*
 * 菜单选择下一项。
 * 当前是 1-3 时，再向下会循环到 1-1。
 */
static LevelId app_menu_next_level(LevelId level) {
    if (level == LEVEL_1_1) {
        return LEVEL_1_2;
    } else if (level == LEVEL_1_2) {
        return LEVEL_1_3;
    } else {
        return LEVEL_1_1;
    }
}

/*
 * PJ4 应用主循环。
 *
 * 当前阶段实现：
 * 1. 显示关卡选择菜单；
 * 2. 摇杆上/下切换关卡；
 * 3. 摇杆中键进入基础游戏；
 * 4. 游戏中 SW1 返回菜单；
 * 5. 撞墙或自撞后返回菜单。
 */
void app_run(void) {
    LevelId selected_level = LEVEL_1_1;

    input_init();
    render_menu(selected_level);

    while (1) {
        InputState input = input_read();

        if (input.up) {
            selected_level = app_menu_prev_level(selected_level);
            render_menu(selected_level);
        }

        if (input.down) {
            selected_level = app_menu_next_level(selected_level);
            render_menu(selected_level);
        }

        if (input.center) {
            game_run(selected_level);
            /*
             * 从游戏返回后重新显示菜单。
             */
            render_menu(selected_level);
        }

        delay_1ms(30);
    }
}