//C 和汇编之间的桥接 / 显示排行榜
#include "scoreboard.h"

#include "input.h"
#include "render.h"
#include "systick.h"

/*
 * 这些函数在 src/assembly/scoreboard.S 中实现。
 *
 * 注意：
 * - scoreboard_update_asm(level, score) 的更新和排序逻辑在汇编里完成。
 * - C 代码这里只负责调用和显示，不负责排序。
 */
extern void scoreboard_update_asm(int level, int score);
extern int scoreboard_get_level_asm(int index);
extern int scoreboard_get_score_asm(int index);

/*
 * C 层排行榜更新接口。
 */
void scoreboard_update(LevelId level, int score) {
    scoreboard_update_asm((int)level, score);
}

/*
 * 获取排行榜中的关卡。
 */
int scoreboard_get_level(int index) {
    if (index < 0 || index >= 3) {
        return 0;
    }

    return scoreboard_get_level_asm(index);
}

/*
 * 获取排行榜中的分数。
 */
int scoreboard_get_score(int index) {
    if (index < 0 || index >= 3) {
        return 0;
    }

    return scoreboard_get_score_asm(index);
}

/*
 * 显示排行榜页面，并等待 SW1 返回。
 */
void scoreboard_show_screen(void) {
    render_scoreboard();

    while (1) {
        InputState input = input_read();

        if (input.sw1) {
            return;
        }

        delay_1ms(30);
    }
}