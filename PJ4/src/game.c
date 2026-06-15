//单局游戏主逻辑
#include "game.h"

#include "config.h"
#include "input.h"
#include "item.h"
#include "render.h"
#include "snake.h"
#include "systick.h"

/*
 * 处理方向输入。
 */
static void game_handle_direction_input(Snake *snake, InputState input) {
    if (input.up) {
        snake_set_direction(snake, DIR_UP);
    } else if (input.down) {
        snake_set_direction(snake, DIR_DOWN);
    } else if (input.left) {
        snake_set_direction(snake, DIR_LEFT);
    } else if (input.right) {
        snake_set_direction(snake, DIR_RIGHT);
    }
}

/*
 * 根据当前蛇头和方向，预测下一格蛇头位置。
 */
static Point game_next_head(const Snake *snake) {
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
 * 判断下一步是否会吃到金币。
 */
static int game_next_head_hits_coin(const Snake *snake, const Item *coin) {
    Point next = game_next_head(snake);

    if (!coin->active) {
        return 0;
    }

    if (coin->type != ITEM_COIN) {
        return 0;
    }

    return next.x == coin->pos.x && next.y == coin->pos.y;
}

/*
 * 显示死亡页面。
 */
static void game_show_dead_screen(int score) {
    render_game_over(score);
    delay_1ms(900);
}

/*
 * 运行一局游戏。
 */
GameResult game_run(LevelId level) {
    Snake snake;
    Item coin;

    int score = 0;
    int elapsed_ms = 0;

    /*
     * 当前阶段 1-1 / 1-2 / 1-3 都先运行同一套基础蛇逻辑。
     */
    (void)level;

    snake_init(&snake);
    item_init();
    item_spawn_coin(&coin, &snake);

    render_game_basic(&snake, &coin, score);

    while (1) {
        InputState input = input_read();

        /*
         * 游戏中按 SW1 返回菜单。
         */
        if (input.sw1) {
            return GAME_RESULT_EXIT_TO_MENU;
        }

        /*
         * 更新方向。
         */
        game_handle_direction_input(&snake, input);

        /*
         * SW2 按住时加速。
         */
        int step_ms = input.sw2 ? SNAKE_FAST_STEP_MS : SNAKE_NORMAL_STEP_MS;

        delay_1ms(20);
        elapsed_ms += 20;

        /*
         * 金币生命周期每 20ms 更新一次。
         */
        item_update_coin_life(&coin, &snake, 20);

        if (elapsed_ms >= step_ms) {
            elapsed_ms = 0;

            /*
             * 在移动前预判下一格是否是金币。
             */
            int will_eat_coin = game_next_head_hits_coin(&snake, &coin);

            if (will_eat_coin) {
                snake_move_and_grow(&snake);
                score += 1;
            } else {
                snake_move(&snake);
            }

            /*
             * 移动后进行死亡判定。
             */
            if (snake_hit_wall(&snake) || snake_hit_self(&snake)) {
                game_show_dead_screen(score);
                return GAME_RESULT_PLAYER_DEAD;
            }

            /*
             * 如果吃到了金币，重新生成金币。
             */
            if (will_eat_coin) {
                item_spawn_coin(&coin, &snake);
            }

            render_game_basic(&snake, &coin, score);
        }
    }
}