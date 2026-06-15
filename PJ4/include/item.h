//金币/宝石生成与判定
#ifndef ITEM_H
#define ITEM_H

#include "types.h"
#include "snake.h"

/*
 * Item 表示地图上的一个物品。
 *
 * 当前阶段只生成金币：
 * type = ITEM_COIN
 *
 * life_ms 用于记录金币已经存在了多久。
 * 金币超过 10 秒后刷新。
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
 * 生成一个金币。
 * 金币不能生成在蛇身体上。
 */
void item_spawn_coin(Item *item, const Snake *snake);

/*
 * 更新金币生命周期。
 * 如果金币存在超过 COIN_LIFE_MS，则刷新位置。
 */
void item_update_coin_life(Item *item, const Snake *snake, int delta_ms);

/*
 * 判断蛇头是否吃到金币。
 */
int item_is_eaten_by_snake(const Item *item, const Snake *snake);

#endif