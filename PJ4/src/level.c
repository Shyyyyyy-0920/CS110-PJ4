//不同关卡初始化
#include "level.h"

/*
 * 初始化 Level 1-2。
 *
 * 蛇初始位置大致是：
 *   头：(10, 5)
 *   身：(9, 5)
 *   尾：(8, 5)
 *
 * 所以墙和传送门都避开了蛇的初始位置。
 */
static void level_init_1_2(LevelData *level) {
    level->wall_count = 4;

    /*
     * 至少 4 个 1x1 静态墙。
     */
    level->walls[0].x = 4;
    level->walls[0].y = 2;

    level->walls[1].x = 15;
    level->walls[1].y = 2;

    level->walls[2].x = 4;
    level->walls[2].y = 7;

    level->walls[3].x = 15;
    level->walls[3].y = 7;

    /*
     * 一对传送门。
     */
    level->portal_a.x = 2;
    level->portal_a.y = 4;

    level->portal_b.x = 17;
    level->portal_b.y = 4;

    level->has_portal = 1;

    /*
     * Level 1-2 开启宝石生成。
     */
    level->enable_gem = 1;
}

/*
 * 初始化关卡。
 */
void level_init(LevelData *level, LevelId id) {
    level->id = id;

    level->wall_count = 0;
    level->has_portal = 0;
    level->enable_gem = 0;

    level->portal_a.x = -1;
    level->portal_a.y = -1;
    level->portal_b.x = -1;
    level->portal_b.y = -1;

    if (id == LEVEL_1_2) {
        level_init_1_2(level);
    }
}

/*
 * 判断某个格子是否是墙。
 */
int level_is_wall(const LevelData *level, Point p) {
    for (int i = 0; i < level->wall_count; ++i) {
        if (level->walls[i].x == p.x && level->walls[i].y == p.y) {
            return 1;
        }
    }

    return 0;
}

/*
 * 判断某个格子是否是传送门。
 */
int level_is_portal(const LevelData *level, Point p) {
    if (!level->has_portal) {
        return 0;
    }

    if (level->portal_a.x == p.x && level->portal_a.y == p.y) {
        return 1;
    }

    if (level->portal_b.x == p.x && level->portal_b.y == p.y) {
        return 1;
    }

    return 0;
}

/*
 * 判断某个格子是否被关卡障碍占用。
 */
int level_is_blocked_cell(const LevelData *level, Point p) {
    if (level_is_wall(level, p)) {
        return 1;
    }

    if (level_is_portal(level, p)) {
        return 1;
    }

    return 0;
}