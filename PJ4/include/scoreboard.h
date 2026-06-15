//C调用汇编scoreboard函数的接口
#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include "types.h"

/*
 * 更新排行榜。
 *
 * 这个函数是 C 层接口，内部会调用 RISC-V 汇编函数。
 * 排行榜核心更新、插入、排序逻辑必须在 scoreboard.S 中完成。
 */
void scoreboard_update(LevelId level, int score);

/*
 * 获取排行榜第 index 项的关卡。
 * index 取值范围：0, 1, 2。
 */
int scoreboard_get_level(int index);

/*
 * 获取排行榜第 index 项的分数。
 * index 取值范围：0, 1, 2。
 */
int scoreboard_get_score(int index);

/*
 * 显示排行榜页面。
 * 页面中按 SW1 返回。
 */
void scoreboard_show_screen(void);

#endif