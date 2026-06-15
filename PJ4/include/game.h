//贪吃蛇基本游戏逻辑
#ifndef GAME_H
#define GAME_H

#include "types.h"

/*
 * 运行一局游戏。
 *
 * 当前阶段实现：
 * 1. 蛇自动移动；
 * 2. 摇杆控制方向；
 * 3. SW2 加速；
 * 4. 撞墙/自撞死亡；
 * 5. 金币生成；
 * 6. 吃金币加分；
 * 7. 吃金币增长；
 * 8. 金币 10 秒刷新；
 * 9. 实时分数显示。
 */
GameResult game_run(LevelId level);

#endif