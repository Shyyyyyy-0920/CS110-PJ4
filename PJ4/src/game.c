//单局游戏主逻辑
#include "game.h"

#include "config.h"
#include "input.h"
#include "item.h"
#include "level.h"
#include "portal.h"
#include "render.h"
#include "snake.h"
#include "systick.h"
#include "scoreboard.h"
#include "enemy.h"
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
 * 判断下一步是否会吃到食物。
 */
static int game_next_head_hits_food(const Snake *snake, const Item *food) {
    Point next = game_next_head(snake);

    if (!food->active) {
        return 0;
    }

    if (food->type == ITEM_NONE) {
        return 0;
    }

    return next.x == food->pos.x && next.y == food->pos.y;
}

/*
 * 判断玩家下一步是否会吃到敌方蛇死亡后掉落的金币。
 *
 * 返回值：
 *   >= 0：被吃到的掉落金币下标
 *   -1：没有吃到掉落金币
 */
static int game_next_head_hits_drop(const Snake *snake,
                                    const Point drops[ENEMY_SNAKE_LEN],
                                    const int active[ENEMY_SNAKE_LEN]) {
    Point next = game_next_head(snake);

    for (int i = 0; i < ENEMY_SNAKE_LEN; ++i) {
        if (active[i] &&
            drops[i].x == next.x &&
            drops[i].y == next.y) {
            return i;
        }
    }

    return -1;
}


/*
 * 判断蛇是否撞到 Level 1-2 的墙。
 */
static int game_hit_level_wall(const Snake *snake, const LevelData *level) {
    Point head = snake->body[0];

    return level_is_wall(level, head);
}

/*
 * 显示死亡页面。
 *
 * 玩家死亡后：
 * - SW1：返回菜单
 * - 摇杆中键：进入排行榜页面
 */
static void game_show_dead_screen(int score) {
    render_game_over(score);

    while (1) {
        InputState input = input_read();

        if (input.center) {
            scoreboard_show_screen();
            render_game_over(score);
        }

        if (input.sw1) {
            return;
        }

        delay_1ms(30);
    }
}

/*
 * 运行 Level 1-3。
 *
 * Level 1-3 特性：
 * 1. 生成一条长度为 3 的敌方蛇；
 * 2. 有金币时，敌方蛇朝金币方向移动；
 * 3. 没有金币时，敌方蛇保持当前方向巡航；
 * 4. 敌方蛇接近墙时优先右转；
 * 5. 玩家蛇头撞到敌方蛇身体，玩家死亡；
 * 6. 敌方蛇头撞到玩家蛇身体，敌方死亡；
 * 7. 敌方死亡后原地掉落 3 个金币。
 */
static GameResult game_run_level3(LevelId level_id) {
    Snake snake;
    EnemySnake enemy;
    Item food;
    LevelData level;

    Point drops[ENEMY_SNAKE_LEN];
    int drop_active[ENEMY_SNAKE_LEN];

    int score = 0;
    int elapsed_ms = 0;

    /*
     * 初始化掉落金币为空。
     */
    for (int i = 0; i < ENEMY_SNAKE_LEN; ++i) {
        drops[i].x = -1;
        drops[i].y = -1;
        drop_active[i] = 0;
    }

    snake_init(&snake);
    enemy_init(&enemy);

    /*
     * Level 1-3 没有墙、宝石、传送门。
     * 但仍然初始化 level，方便 item_spawn_food() 判断默认规则。
     */
    level_init(&level, level_id);

    item_init();
    item_spawn_food(&food, &snake, &level);

    render_game_level3(&snake, &food, score, &level, &enemy, drops, drop_active);

    while (1) {
        InputState input = input_read();

        /*
         * 游戏中按 SW1 返回菜单。
         */
        if (input.sw1) {
            return GAME_RESULT_EXIT_TO_MENU;
        }

        /*
         * 玩家方向输入。
         */
        game_handle_direction_input(&snake, input);

        /*
         * SW2 按住时加速。
         */
        int step_ms = input.sw2 ? SNAKE_FAST_STEP_MS : SNAKE_NORMAL_STEP_MS;

        delay_1ms(FRAME_TIME_MS);
        elapsed_ms += FRAME_TIME_MS;

        /*
         * Level 1-3 中普通食物是金币，10 秒刷新。
         */
        item_update_life(&food, &snake, &level, FRAME_TIME_MS);

        if (elapsed_ms >= step_ms) {
            elapsed_ms = 0;

            /*
             * 玩家移动前，预判是否吃到普通金币或掉落金币。
             */
            int will_eat_food = game_next_head_hits_food(&snake, &food);
            int drop_index = game_next_head_hits_drop(&snake, drops, drop_active);

            if (will_eat_food || drop_index >= 0) {
                snake_move_and_grow(&snake);
            } else {
                snake_move(&snake);
            }

            /*
             * 玩家吃普通金币。
             */
            if (will_eat_food) {
                score += item_get_score(&food);
                item_spawn_food(&food, &snake, &level);
            }

            /*
             * 玩家吃敌方蛇死亡后掉落的金币。
             * 每个掉落金币 +1 分并增长。
             */
            if (drop_index >= 0) {
                score += 1;
                drop_active[drop_index] = 0;
            }

            /*
             * 更新敌方蛇。
             */
            if (enemy.alive) {
                enemy_update(&enemy, &food);

                /*
                 * 如果敌方蛇头吃到普通金币，则让金币刷新。
                 * 敌方蛇不加分。
                 */
                if (food.active &&
                    food.type == ITEM_COIN &&
                    enemy.body[0].x == food.pos.x &&
                    enemy.body[0].y == food.pos.y) {
                    item_spawn_food(&food, &snake, &level);
                }
            }

            /*
             * 玩家基础死亡判定：撞边界或自撞。
             */
            if (snake_hit_wall(&snake) || snake_hit_self(&snake)) {
                scoreboard_update(level_id, score);
                game_show_dead_screen(score);
                return GAME_RESULT_PLAYER_DEAD;
            }

            /*
             * 玩家蛇头撞到敌方蛇身体，玩家死亡。
             */
            if (enemy_player_hits_enemy(&enemy, &snake)) {
                scoreboard_update(level_id, score);
                game_show_dead_screen(score);
                return GAME_RESULT_PLAYER_DEAD;
            }

            /*
             * 敌方蛇头撞到玩家身体，敌方蛇死亡。
             */
            if (enemy_hits_player_body(&enemy, &snake)) {
                enemy_make_drops(&enemy, drops, drop_active);
                enemy.alive = 0;
            }

            render_game_level3(&snake, &food, score, &level, &enemy, drops, drop_active);
        }
    }
}

/*
 * 运行一局游戏。
 */
GameResult game_run(LevelId level_id) {

    /*
     * Level 1-3 使用敌方蛇 AI 逻辑。
     */
    if (level_id == LEVEL_1_3) {
        return game_run_level3(level_id);
    }

    Snake snake;
    Item food;
    LevelData level;

    int score = 0;
    int elapsed_ms = 0;

    snake_init(&snake);
    level_init(&level, level_id);

    item_init();
    item_spawn_food(&food, &snake, &level);

    render_game_basic(&snake, &food, score, &level);

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

        delay_1ms(FRAME_TIME_MS);
        elapsed_ms += FRAME_TIME_MS;

        /*
         * 更新金币/宝石生命周期。
         * 金币 10 秒刷新，宝石 5 秒刷新。
         */
        item_update_life(&food, &snake, &level, FRAME_TIME_MS);

        if (elapsed_ms >= step_ms) {
            elapsed_ms = 0;

            /*
             * 移动前预判下一格是否是食物。
             */
            int will_eat_food = game_next_head_hits_food(&snake, &food);

            if (will_eat_food) {
                snake_move_and_grow(&snake);
                score += item_get_score(&food);
            } else {
                snake_move(&snake);
            }

            /*
             * Level 1-2：传送门。
             *
             * 蛇头进入一个传送门后，会被移动到另一个传送门。
             */
            portal_try_teleport(&level, &snake);

            /*
             * 移动后进行死亡判定。
             *
             * snake_hit_wall：屏幕边界
             * snake_hit_self：自撞
             * game_hit_level_wall：Level 1-2 静态墙
             */
            if (snake_hit_wall(&snake) ||
                snake_hit_self(&snake) ||
                game_hit_level_wall(&snake, &level)) {

                /*
                 * 死亡后立刻更新排行榜。
                 * 即使玩家不进入 scoreboard 页面，记录也已经保存。
                 */
                scoreboard_update(level_id, score);

                game_show_dead_screen(score);
                return GAME_RESULT_PLAYER_DEAD;
            }

            /*
             * 如果吃到食物，重新生成。
             */
            if (will_eat_food) {
                item_spawn_food(&food, &snake, &level);
            }

            render_game_basic(&snake, &food, score, &level);
        }
    }
}