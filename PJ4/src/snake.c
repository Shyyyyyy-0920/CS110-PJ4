//玩家蛇移动、增长、碰撞
#include "snake.h"

/*
 * 判断两个方向是否互为反方向。
 */
static int snake_is_opposite_direction(Direction a, Direction b) {
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
 * 初始化玩家蛇。
 *
 * 初始长度为 3，方向向右。
 */
void snake_init(Snake *snake) {
    int start_x = GRID_WIDTH / 2;
    int start_y = GRID_HEIGHT / 2;

    snake->length = 3;
    snake->dir = DIR_RIGHT;

    snake->body[0].x = start_x;
    snake->body[0].y = start_y;

    snake->body[1].x = start_x - 1;
    snake->body[1].y = start_y;

    snake->body[2].x = start_x - 2;
    snake->body[2].y = start_y;
}

/*
 * 设置蛇的方向。
 */
void snake_set_direction(Snake *snake, Direction new_dir) {
    if (snake_is_opposite_direction(snake->dir, new_dir)) {
        return;
    }

    snake->dir = new_dir;
}

/*
 * 根据当前方向计算下一格蛇头位置。
 */
static Point snake_next_head(const Snake *snake) {
    Point next = snake->body[0];

    if (snake->dir == DIR_UP) {
        next.y -= 1;
    } else if (snake->dir == DIR_DOWN) {
        next.y += 1;
    } else if (snake->dir == DIR_LEFT) {
        next.x -= 1;
    } else if (snake->dir == DIR_RIGHT) {
        next.x += 1;
    }

    return next;
}

/*
 * 蛇正常移动一格。
 */
void snake_move(Snake *snake) {
    Point next = snake_next_head(snake);

    for (int i = snake->length - 1; i > 0; --i) {
        snake->body[i] = snake->body[i - 1];
    }

    snake->body[0] = next;
}

/*
 * 蛇移动并增长。
 *
 * 与 snake_move 的区别：
 * 1. 如果长度未达到上限，则 length 先增加；
 * 2. 然后整体后移；
 * 3. 新蛇头放到下一格。
 *
 * 这样尾巴不会被删除，蛇就增长了一格。
 */
void snake_move_and_grow(Snake *snake) {
    Point next = snake_next_head(snake);

    if (snake->length < MAX_SNAKE_LEN) {
        snake->length += 1;
    }

    for (int i = snake->length - 1; i > 0; --i) {
        snake->body[i] = snake->body[i - 1];
    }

    snake->body[0] = next;
}

/*
 * 判断蛇头是否撞到边界。
 */
int snake_hit_wall(const Snake *snake) {
    Point head = snake->body[0];

    if (head.x < 0 || head.x >= GRID_WIDTH) {
        return 1;
    }

    if (head.y < 0 || head.y >= GRID_HEIGHT) {
        return 1;
    }

    return 0;
}

/*
 * 判断蛇头是否撞到自己的身体。
 */
int snake_hit_self(const Snake *snake) {
    Point head = snake->body[0];

    for (int i = 1; i < snake->length; ++i) {
        if (snake->body[i].x == head.x && snake->body[i].y == head.y) {
            return 1;
        }
    }

    return 0;
}

/*
 * 判断某个格子是否被蛇占用。
 */
int snake_contains_point(const Snake *snake, Point p) {
    for (int i = 0; i < snake->length; ++i) {
        if (snake->body[i].x == p.x && snake->body[i].y == p.y) {
            return 1;
        }
    }

    return 0;
}