//金币/宝石生成与判定
#ifndef ITEM_H
#define ITEM_H

#include "types.h"
#include "snake.h"
#include "level.h"

/*
 * Item 表示地图上的一个物品。
 *
 * Level 1-1:
 *   只生成金币。
 *
 * Level 1-2:
 *   50% 概率生成金币，50% 概率生成宝石。
 */
typedef struct {
    Point pos;
    ItemType type;
    int active;
    int life_ms;
} Item;

/*
 * 初始化物品模块。
 */
void item_init(void);

/*
 * 根据当前关卡生成食物。
 *
 * Level 1-1：只生成金币。
 * Level 1-2：50% 金币，50% 宝石。
 */
void item_spawn_food(Item *item, const Snake *snake, const LevelData *level);

/*
 * 更新物品生命周期。
 *
 * 金币 10 秒刷新。
 * 宝石 5 秒刷新。
 */
void item_update_life(Item *item, const Snake *snake, const LevelData *level, int delta_ms);

/*
 * 判断蛇头是否吃到物品。
 */
int item_is_eaten_by_snake(const Item *item, const Snake *snake);

/*
 * 获取物品对应分数。
 *
 * 金币 +1
 * 宝石 +2
 */
int item_get_score(const Item *item);

#endif