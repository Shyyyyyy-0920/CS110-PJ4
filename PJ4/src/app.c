//主循环和状态切换
// 主循环和状态切换
#include "app.h"
#include "config.h"
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

    int menu_anim_frame = 0;
    int menu_anim_elapsed = 0;

    input_init();
    render_menu(selected_level);

    while (1) {
        InputState input = input_read();

        if (input.up) {
            selected_level = app_menu_prev_level(selected_level);

            /*
             * 切换选项后，从动画初始帧重新开始。
             */
            menu_anim_frame = 0;
            menu_anim_elapsed = 0;

            render_menu(selected_level);
        }

        if (input.down) {
            selected_level = app_menu_next_level(selected_level);

            /*
             * 切换选项后，从动画初始帧重新开始。
             */
            menu_anim_frame = 0;
            menu_anim_elapsed = 0;

            render_menu(selected_level);
        }

        if (input.center) {
            game_run(selected_level);

            /*
             * 从游戏返回菜单后，重新绘制菜单并重置动画。
             */
            menu_anim_frame = 0;
            menu_anim_elapsed = 0;

            render_menu(selected_level);
        }

        /*
         * 菜单动画刷新。
         *
         * 每 180ms 切换一次颜色。
         * 这样选中的关卡会呈现绿色呼吸灯效果。
         */
        delay_1ms(FRAME_TIME_MS);
        menu_anim_elapsed += FRAME_TIME_MS;

        if (menu_anim_elapsed >= 180) {
            menu_anim_elapsed = 0;
            menu_anim_frame++;

            render_menu_animated(selected_level, menu_anim_frame);
        }
    }
}