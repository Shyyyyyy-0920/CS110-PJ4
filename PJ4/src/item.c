//金币 / 宝石生成和生命周期
#include "item.h"

#include "config.h"

/*
 * 简单伪随机数种子。
 */
static unsigned int item_rand_seed = 12345;

/*
 * 生成 [0, max_value - 1] 范围内的伪随机数。
 */
static int item_rand_range(int max_value) {
    item_rand_seed = item_rand_seed * 1103515245u + 12345u;
    return (int)((item_rand_seed >> 16) % (unsigned int)max_value);
}

/*
 * 判断某个位置是否可以生成食物。
 *
 * 食物不能生成在：
 * 1. 屏幕外
 * 2. 蛇身体上
 * 3. 墙上
 * 4. 传送门上
 */
static int item_can_spawn_at(Point p, const Snake *snake, const LevelData *level) {
    if (p.x < 0 || p.x >= GRID_WIDTH) {
        return 0;
    }

    if (p.y < 0 || p.y >= GRID_HEIGHT) {
        return 0;
    }
    /*
     * 避开左上角分数显示区域。
     */
    if (p.x <= 6 && p.y <= 1) {
        return 0;
    }
    
    if (snake_contains_point(snake, p)) {
        return 0;
    }

    if (level_is_blocked_cell(level, p)) {
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
 * 根据关卡决定生成金币还是宝石。
 */
static ItemType item_choose_type(const LevelData *level) {
    if (level->id == LEVEL_1_2 && level->enable_gem) {
        /*
         * Level 1-2：50% 概率金币，50% 概率宝石。
         */
        if (item_rand_range(2) == 0) {
            return ITEM_COIN;
        } else {
            return ITEM_GEM;
        }
    }

    /*
     * 其他关卡当前只生成金币。
     */
    return ITEM_COIN;
}

/*
 * 生成一个食物。
 */
void item_spawn_food(Item *item, const Snake *snake, const LevelData *level) {
    Point p;

    /*
     * 先随机尝试生成。
     */
    for (int tries = 0; tries < 100; ++tries) {
        p.x = item_rand_range(GRID_WIDTH);
        p.y = item_rand_range(GRID_HEIGHT);

        if (item_can_spawn_at(p, snake, level)) {
            item->pos = p;
            item->type = item_choose_type(level);
            item->active = 1;
            item->life_ms = 0;
            return;
        }
    }

    /*
     * 随机失败时，顺序扫描兜底。
     */
    for (int y = 0; y < GRID_HEIGHT; ++y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            p.x = x;
            p.y = y;

            if (item_can_spawn_at(p, snake, level)) {
                item->pos = p;
                item->type = item_choose_type(level);
                item->active = 1;
                item->life_ms = 0;
                return;
            }
        }
    }

    /*
     * 极端情况：地图没有空位。
     */
    item->active = 0;
    item->type = ITEM_NONE;
    item->life_ms = 0;
}

/*
 * 更新食物生命周期。
 */
void item_update_life(Item *item, const Snake *snake, const LevelData *level, int delta_ms) {
    int life_limit;

    if (!item->active) {
        item_spawn_food(item, snake, level);
        return;
    }

    item->life_ms += delta_ms;

    if (item->type == ITEM_GEM) {
        life_limit = GEM_LIFE_MS;
    } else {
        life_limit = COIN_LIFE_MS;
    }

    if (item->life_ms >= life_limit) {
        item_spawn_food(item, snake, level);
    }
}

/*
 * 判断蛇头是否吃到食物。
 */
int item_is_eaten_by_snake(const Item *item, const Snake *snake) {
    Point head = snake->body[0];

    if (!item->active) {
        return 0;
    }

    if (item->type == ITEM_NONE) {
        return 0;
    }

    return head.x == item->pos.x && head.y == item->pos.y;
}

/*
 * 获取食物分数。
 */
int item_get_score(const Item *item) {
    if (!item->active) {
        return 0;
    }

    if (item->type == ITEM_GEM) {
        return 2;
    }

    if (item->type == ITEM_COIN) {
        return 1;
    }

    return 0;
}