//LCD 绘制封装
#ifndef RENDER_H
#define RENDER_H

#include "types.h"
#include "snake.h"
#include "item.h"
#include "level.h"

/*
 * 渲染模块只负责“怎么画”。
 */

void render_clear(void);
void render_menu(LevelId selected_level);

/*
 * 绘制游戏画面。
 *
 * snake：玩家蛇
 * food：金币或宝石
 * score：当前分数
 * level：当前关卡配置，用于绘制墙和传送门
 */
void render_game_basic(const Snake *snake, const Item *food, int score, const LevelData *level);

/*
 * 绘制死亡提示。
 */
void render_game_over(int score);

/*
 * 绘制排行榜页面。
 */
void render_scoreboard(void);

#endif