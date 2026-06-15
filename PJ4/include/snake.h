//蛇的数据结构和移动逻辑
#ifndef SNAKE_H
#define SNAKE_H

#include "types.h"
#include "config.h"

/*
 * Snake 表示一条蛇。
 *
 * body[0] 是蛇头。
 * body[length - 1] 是蛇尾。
 */
typedef struct {
    Point body[MAX_SNAKE_LEN];
    int length;
    Direction dir;
} Snake;

/*
 * 初始化玩家蛇。
 */
void snake_init(Snake *snake);

/*
 * 设置蛇的移动方向。
 * 不允许直接反向。
 */
void snake_set_direction(Snake *snake, Direction new_dir);

/*
 * 蛇正常前进一步，不增长。
 */
void snake_move(Snake *snake);

/*
 * 蛇前进一步，并增长一格。
 * 做法是移动蛇头，但不删除蛇尾。
 */
void snake_move_and_grow(Snake *snake);

/*
 * 判断蛇头是否撞到边界。
 */
int snake_hit_wall(const Snake *snake);

/*
 * 判断蛇头是否撞到自己的身体。
 */
int snake_hit_self(const Snake *snake);

/*
 * 判断某个格子是否被蛇身体占用。
 */
int snake_contains_point(const Snake *snake, Point p);

#endif