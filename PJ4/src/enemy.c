// 敌方蛇行为
// Level 1-3 敌方蛇行为逻辑
#include "enemy.h"

/*
 * 判断两个方向是否互为反方向。
 * 敌方蛇也不能直接 180 度掉头。
 */
static int enemy_is_opposite_direction(Direction a, Direction b) {
    if (a == DIR_UP && b == DIR_DOWN) {
        return 1;
    }
    if (a == DIR_DOWN && b == DIR_UP) {
        return 1;
    }
    if (a == DIR_LEFT && b == DIR_RIGHT) {
        return 1;
    }
    if (a == DIR_RIGHT && b == DIR_LEFT) {
        return 1;
    }
    return 0;
}

/*
 * 根据当前方向计算左转方向。
 *
 * 上 -> 左
 * 左 -> 下
 * 下 -> 右
 * 右 -> 上
 */
static Direction enemy_turn_left(Direction dir) {
    if (dir == DIR_UP) {
        return DIR_LEFT;
    } else if (dir == DIR_LEFT) {
        return DIR_DOWN;
    } else if (dir == DIR_DOWN) {
        return DIR_RIGHT;
    } else {
        return DIR_UP;
    }
}

/*
 * 根据当前方向计算右转方向。
 *
 * 上 -> 右
 * 右 -> 下
 * 下 -> 左
 * 左 -> 上
 */
static Direction enemy_turn_right(Direction dir) {
    if (dir == DIR_UP) {
        return DIR_RIGHT;
    } else if (dir == DIR_RIGHT) {
        return DIR_DOWN;
    } else if (dir == DIR_DOWN) {
        return DIR_LEFT;
    } else {
        return DIR_UP;
    }
}

/*
 * 计算从 p 沿 dir 前进一步后的位置。
 */
static Point enemy_next_point(Point p, Direction dir) {
    if (dir == DIR_UP) {
        p.y -= 1;
    } else if (dir == DIR_DOWN) {
        p.y += 1;
    } else if (dir == DIR_LEFT) {
        p.x -= 1;
    } else if (dir == DIR_RIGHT) {
        p.x += 1;
    }

    return p;
}

/*
 * 曼哈顿距离。
 * 用于判断敌方蛇朝哪个方向走会更接近金币。
 */
static int enemy_manhattan_distance(Point a, Point b) {
    int dx = a.x - b.x;
    int dy = a.y - b.y;

    if (dx < 0) {
        dx = -dx;
    }

    if (dy < 0) {
        dy = -dy;
    }

    return dx + dy;
}

/*
 * 判断某个方向是否危险。
 *
 * 严格按照“距离墙还有 1 格必须转向”处理：
 * - 如果下一步会走出地图，危险；
 * - 如果下一步会进入最外圈格子，也认为危险。
 *
 * 因此敌方蛇只允许在内部安全区域移动：
 *   x = 1 ~ GRID_WIDTH - 2
 *   y = 1 ~ GRID_HEIGHT - 2
 */
static int enemy_direction_is_dangerous(Point head, Direction dir) {
    Point next = enemy_next_point(head, dir);

    /*
     * 下一步走出地图，肯定危险。
     */
    if (next.x < 0 || next.x >= GRID_WIDTH ||
        next.y < 0 || next.y >= GRID_HEIGHT) {
        return 1;
    }

    /*
     * 下一步进入最外圈格子，也视为危险。
     * 这对应“距离墙还有 1 格必须转向”。
     */
    if (next.x <= 0 || next.x >= GRID_WIDTH - 1) {
        return 1;
    }

    if (next.y <= 0 || next.y >= GRID_HEIGHT - 1) {
        return 1;
    }

    return 0;
}

/*
 * 判断某个方向是否允许选择。
 *
 * 规则：
 * 1. 不能直接反向；
 * 2. 不能走向危险边界。
 */
static int enemy_direction_is_allowed(const EnemySnake *enemy, Direction dir) {
    if (enemy_is_opposite_direction(enemy->dir, dir)) {
        return 0;
    }

    if (enemy_direction_is_dangerous(enemy->body[0], dir)) {
        return 0;
    }

    return 1;
}

/*
 * 在“前进、左转、右转”三个方向中，选择追金币方向。
 *
 * 优先级：
 * 1. 如果金币就在前/左/右相邻一格，直接吃；
 * 2. 否则选择下一步后距离金币最近的方向；
 * 3. 不允许 180 度反向；
 * 4. 如果方向太靠近边界，正常情况下不选。
 */
static Direction enemy_choose_food_direction(const EnemySnake *enemy, const Item *food) {
    Direction candidates[3];
    Direction best_dir;
    int best_dist;
    int found = 0;

    Point head = enemy->body[0];

    candidates[0] = enemy->dir;
    candidates[1] = enemy_turn_left(enemy->dir);
    candidates[2] = enemy_turn_right(enemy->dir);

    /*
    * 第一轮：如果金币就在前/左/右相邻格，并且这个方向安全，
    * 就直接选择该方向。
    *
    * 注意：
    * 即使金币就在旁边，也不能违反“距离墙 1 格必须转向”的规则。
    */
    for (int i = 0; i < 3; ++i) {
        Direction dir = candidates[i];

        if (!enemy_direction_is_allowed(enemy, dir)) {
            continue;
        }

        Point next = enemy_next_point(head, dir);

        if (next.x == food->pos.x && next.y == food->pos.y) {
            return dir;
        }
    }

    /*
     * 第二轮：选择距离金币最近的安全方向。
     */
    best_dir = enemy->dir;
    best_dist = 9999;

    for (int i = 0; i < 3; ++i) {
        Direction dir = candidates[i];

        if (!enemy_direction_is_allowed(enemy, dir)) {
            continue;
        }

        Point next = enemy_next_point(head, dir);
        int dist = enemy_manhattan_distance(next, food->pos);

        if (!found || dist < best_dist) {
            found = 1;
            best_dist = dist;
            best_dir = dir;
        }
    }

    /*
     * 如果三个方向都因为危险边界被排除，则按要求优先右转。
     */
    if (!found) {
        best_dir = enemy_turn_right(enemy->dir);
    }

    return best_dir;
}
/*
 * 选择巡航方向。
 *
 * 没有金币时，敌方蛇保持当前方向巡航。
 * 如果前方危险，则优先右转。
 */
static Direction enemy_choose_cruise_direction(const EnemySnake *enemy) {
    Direction dir = enemy->dir;

    if (!enemy_direction_is_dangerous(enemy->body[0], dir)) {
        return dir;
    }

    /*
     * 前方危险时，优先右转。
     */
    dir = enemy_turn_right(enemy->dir);
    if (!enemy_direction_is_dangerous(enemy->body[0], dir)) {
        return dir;
    }

    /*
     * 右转也危险，再尝试左转。
     */
    dir = enemy_turn_left(enemy->dir);
    if (!enemy_direction_is_dangerous(enemy->body[0], dir)) {
        return dir;
    }

    /*
     * 极端兜底：继续右转。
     */
    return enemy_turn_right(enemy->dir);
}

/*
 * 初始化敌方蛇。
 *
 * 玩家蛇初始头部大约在 (10, 5)，方向向右。
 * 敌方蛇放在右上区域，方向向左，避免开局直接撞到玩家。
 */
void enemy_init(EnemySnake *enemy) {
    enemy->alive = 1;
    enemy->dir = DIR_LEFT;

    enemy->body[0].x = GRID_WIDTH - 3;
    enemy->body[0].y = 2;

    enemy->body[1].x = GRID_WIDTH - 2;
    enemy->body[1].y = 2;

    enemy->body[2].x = GRID_WIDTH - 1;
    enemy->body[2].y = 2;
}

/*
 * 敌方蛇向当前方向前进一步。
 *
 * 敌方蛇长度固定为 3，所以移动方式和普通蛇不增长时一样：
 * 后一节复制前一节，蛇头前进。
 */
static void enemy_move(EnemySnake *enemy) {
    Point next = enemy_next_point(enemy->body[0], enemy->dir);

    for (int i = ENEMY_SNAKE_LEN - 1; i > 0; --i) {
        enemy->body[i] = enemy->body[i - 1];
    }

    enemy->body[0] = next;
}

/*
 * 更新敌方蛇。
 *
 * 有金币时：
 *   只从“前进、左转、右转”中选一个最接近金币的方向。
 *
 * 没有金币时：
 *   保持当前方向巡航；如果接近边界则优先右转。
 */
void enemy_update(EnemySnake *enemy, const Item *food) {
    if (!enemy->alive) {
        return;
    }

    if (food != 0 && food->active && food->type == ITEM_COIN) {
        enemy->dir = enemy_choose_food_direction(enemy, food);
    } else {
        enemy->dir = enemy_choose_cruise_direction(enemy);
    }

    enemy_move(enemy);
}

/*
 * 玩家蛇头撞到敌方蛇任意一节，玩家死亡。
 */
int enemy_player_hits_enemy(const EnemySnake *enemy, const Snake *player) {
    if (!enemy->alive) {
        return 0;
    }

    Point player_head = player->body[0];

    for (int i = 0; i < ENEMY_SNAKE_LEN; ++i) {
        if (enemy->body[i].x == player_head.x &&
            enemy->body[i].y == player_head.y) {
            return 1;
        }
    }

    return 0;
}

/*
 * 敌方蛇头撞到玩家蛇身体，敌方蛇死亡。
 *
 * 注意：
 * 这里从 i = 1 开始，只检查玩家身体，不检查玩家蛇头。
 * 如果玩家蛇头撞敌方蛇，enemy_player_hits_enemy 会处理为玩家死亡。
 */
int enemy_hits_player_body(const EnemySnake *enemy, const Snake *player) {
    if (!enemy->alive) {
        return 0;
    }

    Point enemy_head = enemy->body[0];

    for (int i = 1; i < player->length; ++i) {
        if (player->body[i].x == enemy_head.x &&
            player->body[i].y == enemy_head.y) {
            return 1;
        }
    }

    return 0;
}

/*
 * 敌方蛇死亡后，原地掉落 3 个金币。
 */
void enemy_make_drops(const EnemySnake *enemy,
                      Point drops[ENEMY_SNAKE_LEN],
                      int active[ENEMY_SNAKE_LEN]) {
    for (int i = 0; i < ENEMY_SNAKE_LEN; ++i) {
        drops[i] = enemy->body[i];
        active[i] = 1;
    }
}