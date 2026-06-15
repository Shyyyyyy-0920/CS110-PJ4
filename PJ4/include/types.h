//通用结构体，枚举类型
#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

/*
 * 二维网格坐标。
 */
typedef struct {
    int x;
    int y;
} Point;

/*
 * 方向枚举。
 */
typedef enum {
    DIR_UP = 0,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction;

/*
 * 关卡编号。
 */
typedef enum {
    LEVEL_1_1 = 1,
    LEVEL_1_2 = 2,
    LEVEL_1_3 = 3
} LevelId;

/*
 * App 顶层状态。
 */
typedef enum {
    APP_STATE_MENU = 0,
    APP_STATE_GAME,
    APP_STATE_GAME_OVER,
    APP_STATE_SCOREBOARD
} AppState;

/*
 * 游戏运行结果。
 */
typedef enum {
    GAME_RESULT_NONE = 0,
    GAME_RESULT_EXIT_TO_MENU,
    GAME_RESULT_PLAYER_DEAD
} GameResult;

/*
 * 物品类型。
 * 当前阶段只用 ITEM_COIN。
 * ITEM_GEM 后续 Level 1-2 再使用。
 */
typedef enum {
    ITEM_NONE = 0,
    ITEM_COIN,
    ITEM_GEM
} ItemType;

/*
 * 输入状态。
 * 0 表示未触发，非 0 表示触发。
 */
typedef struct {
    int up;
    int down;
    int left;
    int right;
    int center;
    int sw1;
    int sw2;
} InputState;

#endif