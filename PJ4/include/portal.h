//Level1-3传送门逻辑
#ifndef PORTAL_H
#define PORTAL_H

#include "level.h"
#include "snake.h"

/*
 * 如果蛇头进入传送门，则把蛇头传送到另一个传送门。
 *
 * 返回值：
 *   1 表示发生了传送
 *   0 表示没有发生传送
 */
int portal_try_teleport(const LevelData *level, Snake *snake);

#endif