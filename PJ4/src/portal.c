//传送门交互
#include "portal.h"

/*
 * 传送门逻辑。
 *
 * 当前实现：
 * - 蛇头进入 portal_a，则传送到 portal_b
 * - 蛇头进入 portal_b，则传送到 portal_a
 * - 方向保持不变
 */
int portal_try_teleport(const LevelData *level, Snake *snake) {
    Point head = snake->body[0];

    if (!level->has_portal) {
        return 0;
    }

    if (head.x == level->portal_a.x && head.y == level->portal_a.y) {
        snake->body[0] = level->portal_b;
        return 1;
    }

    if (head.x == level->portal_b.x && head.y == level->portal_b.y) {
        snake->body[0] = level->portal_a;
        return 1;
    }

    return 0;
}