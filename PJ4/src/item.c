//金币 / 宝石生成和生命周期
#include "item.h"

#include "config.h"

/*
 * 简单伪随机数种子。
 * 不追求真正随机，只要金币每次生成位置有变化即可。
 */
static unsigned int item_rand_seed = 12345;

/*
 * 生成一个 [0, max_value - 1] 范围内的伪随机数。
 */
static int item_rand_range(int max_value) {
    item_rand_seed = item_rand_seed * 1103515245u + 12345u;
    return (int)((item_rand_seed >> 16) % (unsigned int)max_value);
}

/*
 * 判断某个点是否可以生成金币。
 * 当前阶段只需要避开蛇身体。
 */
static int item_can_spawn_at(Point p, const Snake *snake) {
    if (p.x < 0 || p.x >= GRID_WIDTH) {
        return 0;
    }

    if (p.y < 0 || p.y >= GRID_HEIGHT) {
        return 0;
    }

    if (snake_contains_point(snake, p)) {
        return 0;
    }

    return 1;
}

/*
 * 初始化物品模块。
 */
void item_init(void) {
    item_rand_seed = 12345;
}

/*
 * 生成一个金币。
 *
 * 最多尝试 100 次随机位置。
 * 如果运气很差都没找到，就用顺序扫描兜底。
 */
void item_spawn_coin(Item *item, const Snake *snake) {
    Point p;

    for (int tries = 0; tries < 100; ++tries) {
        p.x = item_rand_range(GRID_WIDTH);
        p.y = item_rand_range(GRID_HEIGHT);

        if (item_can_spawn_at(p, snake)) {
            item->pos = p;
            item->type = ITEM_COIN;
            item->active = 1;
            item->life_ms = 0;
            return;
        }
    }

    /*
     * 兜底：顺序扫描地图，找第一个空格。
     */
    for (int y = 0; y < GRID_HEIGHT; ++y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            p.x = x;
            p.y = y;

            if (item_can_spawn_at(p, snake)) {
                item->pos = p;
                item->type = ITEM_COIN;
                item->active = 1;
                item->life_ms = 0;
                return;
            }
        }
    }

    /*
     * 极端情况：地图被蛇占满，没有位置生成金币。
     */
    item->active = 0;
    item->type = ITEM_NONE;
    item->life_ms = 0;
}

/*
 * 更新金币生命周期。
 *
 * 金币存在超过 10 秒，就重新生成。
 */
void item_update_coin_life(Item *item, const Snake *snake, int delta_ms) {
    if (!item->active) {
        item_spawn_coin(item, snake);
        return;
    }

    item->life_ms += delta_ms;

    if (item->life_ms >= COIN_LIFE_MS) {
        item_spawn_coin(item, snake);
    }
}

/*
 * 判断蛇是否吃到金币。
 */
int item_is_eaten_by_snake(const Item *item, const Snake *snake) {
    Point head = snake->body[0];

    if (!item->active) {
        return 0;
    }

    if (item->type != ITEM_COIN) {
        return 0;
    }

    return head.x == item->pos.x && head.y == item->pos.y;
}