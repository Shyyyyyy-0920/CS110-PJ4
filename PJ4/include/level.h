//关卡配置
#ifndef LEVEL_H
#define LEVEL_H

#include "types.h"
#include "config.h"

/*
 * 关卡配置结构体。
 *
 * Level 1-1:
 *   无墙、无传送门、无宝石。
 *
 * Level 1-2:
 *   有墙、有传送门、有宝石。
 *
 * Level 1-3:
 *   后续再加入敌方蛇。
 */
typedef struct {
    LevelId id;

    Point walls[MAX_WALLS];
    int wall_count;

    Point portal_a;
    Point portal_b;
    int has_portal;

    int enable_gem;
} LevelData;

/*
 * 初始化关卡数据。
 */
void level_init(LevelData *level, LevelId id);

/*
 * 判断某个点是否是墙。
 */
int level_is_wall(const LevelData *level, Point p);

/*
 * 判断某个点是否是传送门。
 */
int level_is_portal(const LevelData *level, Point p);

/*
 * 判断某个点是否被关卡障碍占用。
 * 主要用于金币/宝石生成时避开墙和传送门。
 */
int level_is_blocked_cell(const LevelData *level, Point p);

#endif