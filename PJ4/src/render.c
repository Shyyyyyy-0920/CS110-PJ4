// 绘制菜单、蛇、金币、墙、排行榜等
#include "render.h"

#include "config.h"
#include "lcd/lcd.h"
#include "scoreboard.h"

/*
 * 防闪烁渲染说明：
 *
 * 原来的实现是每一帧都 LCD_Clear(BLACK)，然后重画整屏。
 * 这样会导致 LCD 明显闪烁。
 *
 * 现在改为：
 * 1. 第一次进入游戏时，全屏绘制一次；
 * 2. 后续每帧只擦除上一帧的动态对象；
 * 3. 再重画当前帧的动态对象；
 * 4. 墙、传送门、边框属于静态对象，只在必要时重画。
 *
 * 这样可以显著减少闪烁。
 */

/*
 * 普通关卡 1-1 / 1-2 的上一帧缓存。
 */
static int render_basic_initialized = 0;
static Snake render_prev_snake;
static Item render_prev_food;
static LevelData render_prev_level;
static int render_prev_score = 0;

/*
 * Level 1-3 的上一帧缓存。
 */
static int render_level3_initialized = 0;
static Snake render_prev_l3_snake;
static Item render_prev_l3_food;
static LevelData render_prev_l3_level;
static EnemySnake render_prev_l3_enemy;
static Point render_prev_l3_drops[ENEMY_SNAKE_LEN];
static int render_prev_l3_drop_active[ENEMY_SNAKE_LEN];
static int render_prev_l3_score = 0;

/*
 * 清空屏幕。
 *
 * 注意：
 * 只要切换到菜单、死亡页面、排行榜页面，就重置游戏渲染缓存。
 * 这样下一次进入游戏时会重新全屏绘制，避免残影。
 */
void render_clear(void) {
    BACK_COLOR = BLACK;
    LCD_Clear(BLACK);

    render_basic_initialized = 0;
    render_level3_initialized = 0;
}

/*
 * 绘制单个菜单项。
 */
static void render_menu_item(int index, const char *text, int selected) {
    u16 y = MENU_ITEM_Y0 + index * MENU_ITEM_GAP;

    if (selected) {
        LCD_ShowString(MENU_CURSOR_X, y, (u8 *)">", YELLOW);
        LCD_ShowString(MENU_ITEM_X, y, (u8 *)text, YELLOW);
    } else {
        LCD_ShowString(MENU_CURSOR_X, y, (u8 *)" ", WHITE);
        LCD_ShowString(MENU_ITEM_X, y, (u8 *)text, WHITE);
    }
}

/*
 * 绘制关卡选择菜单。
 */
void render_menu(LevelId selected_level) {
    render_clear();

    LCD_ShowString(MENU_TITLE_X, MENU_TITLE_Y, (u8 *)"PJ4 Snake", GREEN);

    render_menu_item(0, "1-1", selected_level == LEVEL_1_1);
    render_menu_item(1, "1-2", selected_level == LEVEL_1_2);
    render_menu_item(2, "1-3", selected_level == LEVEL_1_3);
}

/*
 * 网格坐标转换为像素坐标。
 */
static int render_cell_x(int grid_x) {
    return grid_x * CELL_SIZE;
}

static int render_cell_y(int grid_y) {
    return grid_y * CELL_SIZE;
}

/*
 * 判断格子是否在地图内。
 */
static int render_point_in_grid(Point p) {
    if (p.x < 0 || p.x >= GRID_WIDTH) {
        return 0;
    }

    if (p.y < 0 || p.y >= GRID_HEIGHT) {
        return 0;
    }

    return 1;
}

/*
 * 绘制一个格子。
 */
static void render_cell(Point p, u16 color) {
    int x;
    int y;

    if (!render_point_in_grid(p)) {
        return;
    }

    x = render_cell_x(p.x);
    y = render_cell_y(p.y);

    LCD_Fill(x,
             y,
             x + CELL_SIZE - 1,
             y + CELL_SIZE - 1,
             color);
}

/*
 * 擦除一个格子。
 */
static void render_erase_cell(Point p) {
    render_cell(p, BLACK);
}

/*
 * 绘制游戏边框。
 */
static void render_game_border(void) {
    LCD_DrawRectangle(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1, GRAY);
}

/*
 * 绘制分数。
 *
 * 注意：
 * 分数区域会覆盖左上角一小块网格。
 * 这和之前全屏重画版本保持一致。
 */
static void render_score(int score) {
    LCD_ShowString(0, 0, (u8 *)"S:", WHITE);
    LCD_ShowNum(16, 0, (u16)score, 3, YELLOW);
}

/*
 * 只清空分数显示区域。
 */
static void render_clear_score_area(void) {
    LCD_Fill(0, 0, 48, FONT_HEIGHT - 1, BLACK);
}

/*
 * 绘制食物。
 *
 * 金币：红色
 * 宝石：紫色
 */
static void render_food(const Item *food) {
    if (food == 0 || !food->active) {
        return;
    }

    if (food->type == ITEM_COIN) {
        render_cell(food->pos, RED);
    } else if (food->type == ITEM_GEM) {
        render_cell(food->pos, MAGENTA);
    }
}

/*
 * 绘制敌方蛇死亡后掉落的金币。
 */
static void render_dropped_coins(const Point drops[ENEMY_SNAKE_LEN],
                                 const int active[ENEMY_SNAKE_LEN]) {
    for (int i = 0; i < ENEMY_SNAKE_LEN; ++i) {
        if (active[i]) {
            render_cell(drops[i], RED);
        }
    }
}

/*
 * 绘制玩家蛇。
 */
static void render_snake(const Snake *snake) {
    for (int i = 0; i < snake->length; ++i) {
        if (i == 0) {
            render_cell(snake->body[i], YELLOW);
        } else {
            render_cell(snake->body[i], GREEN);
        }
    }
}

/*
 * 擦除玩家蛇上一帧。
 */
static void render_erase_snake(const Snake *snake) {
    for (int i = 0; i < snake->length; ++i) {
        render_erase_cell(snake->body[i]);
    }
}

/*
 * 绘制敌方蛇。
 *
 * 敌方蛇头用蓝色，身体用灰色。
 */
static void render_enemy(const EnemySnake *enemy) {
    if (enemy == 0 || !enemy->alive) {
        return;
    }

    for (int i = 0; i < ENEMY_SNAKE_LEN; ++i) {
        if (i == 0) {
            render_cell(enemy->body[i], BLUE);
        } else {
            render_cell(enemy->body[i], GRAY);
        }
    }
}

/*
 * 擦除敌方蛇上一帧。
 */
static void render_erase_enemy(const EnemySnake *enemy) {
    if (enemy == 0 || !enemy->alive) {
        return;
    }

    for (int i = 0; i < ENEMY_SNAKE_LEN; ++i) {
        render_erase_cell(enemy->body[i]);
    }
}

/*
 * 绘制 Level 1-2 的墙。
 */
static void render_walls(const LevelData *level) {
    if (level == 0) {
        return;
    }

    for (int i = 0; i < level->wall_count; ++i) {
        render_cell(level->walls[i], GRAY);
    }
}

/*
 * 绘制 Level 1-2 的传送门。
 */
static void render_portals(const LevelData *level) {
    if (level == 0 || !level->has_portal) {
        return;
    }

    render_cell(level->portal_a, BLUE);
    render_cell(level->portal_b, BLUE);
}

/*
 * 绘制静态地图元素。
 *
 * 静态元素包括：
 * - 边框
 * - 墙
 * - 传送门
 *
 * 增量渲染时，擦除上一帧动态对象后，需要把静态元素补回来。
 */
static void render_static_map(const LevelData *level) {
    render_game_border();
    render_walls(level);
    render_portals(level);
}

/*
 * 保存普通关卡上一帧状态。
 */
static void render_save_basic_state(const Snake *snake,
                                    const Item *food,
                                    int score,
                                    const LevelData *level) {
    render_prev_snake = *snake;
    render_prev_food = *food;
    render_prev_score = score;

    if (level != 0) {
        render_prev_level = *level;
    }
}

/*
 * 保存 Level 1-3 上一帧状态。
 */
static void render_save_level3_state(const Snake *snake,
                                     const Item *food,
                                     int score,
                                     const LevelData *level,
                                     const EnemySnake *enemy,
                                     const Point drops[ENEMY_SNAKE_LEN],
                                     const int drop_active[ENEMY_SNAKE_LEN]) {
    render_prev_l3_snake = *snake;
    render_prev_l3_food = *food;
    render_prev_l3_score = score;

    if (level != 0) {
        render_prev_l3_level = *level;
    }

    if (enemy != 0) {
        render_prev_l3_enemy = *enemy;
    }

    for (int i = 0; i < ENEMY_SNAKE_LEN; ++i) {
        render_prev_l3_drops[i] = drops[i];
        render_prev_l3_drop_active[i] = drop_active[i];
    }
}

/*
 * 擦除上一帧掉落金币。
 */
static void render_erase_dropped_coins(const Point drops[ENEMY_SNAKE_LEN],
                                       const int active[ENEMY_SNAKE_LEN]) {
    for (int i = 0; i < ENEMY_SNAKE_LEN; ++i) {
        if (active[i]) {
            render_erase_cell(drops[i]);
        }
    }
}

/*
 * 绘制普通游戏画面。
 *
 * 第一次调用：
 *   全屏清空并完整绘制。
 *
 * 后续调用：
 *   不再全屏清空，只擦除上一帧动态对象，然后绘制当前帧。
 */
void render_game_basic(const Snake *snake, const Item *food, int score, const LevelData *level) {
    if (!render_basic_initialized) {
        render_clear();

        render_static_map(level);
        render_food(food);
        render_snake(snake);
        render_score(score);

        render_save_basic_state(snake, food, score, level);
        render_basic_initialized = 1;
        return;
    }

    /*
     * 1. 擦除上一帧动态元素。
     */
    render_erase_snake(&render_prev_snake);

    if (render_prev_food.active) {
        render_erase_cell(render_prev_food.pos);
    }

    /*
     * 2. 补回静态元素。
     *
     * 例如蛇经过传送门后，擦除蛇身可能会把传送门擦掉，
     * 所以这里必须重画墙和传送门。
     */
    render_static_map(level);

    /*
     * 3. 如果分数变化，只清空分数区域。
     */
    if (score != render_prev_score) {
        render_clear_score_area();
    }

    /*
     * 4. 绘制当前帧动态元素。
     */
    render_food(food);
    render_snake(snake);
    render_score(score);

    /*
     * 5. 保存当前帧，用于下一帧擦除。
     */
    render_save_basic_state(snake, food, score, level);
}

/*
 * 绘制 Level 1-3 游戏画面。
 *
 * 第一次调用：
 *   全屏清空并完整绘制。
 *
 * 后续调用：
 *   只擦除上一帧的玩家蛇、敌方蛇、金币、掉落金币，
 *   然后绘制当前帧。
 */
void render_game_level3(const Snake *snake,
                        const Item *food,
                        int score,
                        const LevelData *level,
                        const EnemySnake *enemy,
                        const Point drops[ENEMY_SNAKE_LEN],
                        const int drop_active[ENEMY_SNAKE_LEN]) {
    if (!render_level3_initialized) {
        render_clear();

        render_static_map(level);
        render_food(food);
        render_dropped_coins(drops, drop_active);
        render_enemy(enemy);
        render_snake(snake);
        render_score(score);

        render_save_level3_state(snake,
                                 food,
                                 score,
                                 level,
                                 enemy,
                                 drops,
                                 drop_active);

        render_level3_initialized = 1;
        return;
    }

    /*
     * 1. 擦除上一帧动态元素。
     */
    render_erase_snake(&render_prev_l3_snake);
    render_erase_enemy(&render_prev_l3_enemy);

    if (render_prev_l3_food.active) {
        render_erase_cell(render_prev_l3_food.pos);
    }

    render_erase_dropped_coins(render_prev_l3_drops, render_prev_l3_drop_active);

    /*
     * 2. 补回静态元素。
     */
    render_static_map(level);

    /*
     * 3. 如果分数变化，只清空分数区域。
     */
    if (score != render_prev_l3_score) {
        render_clear_score_area();
    }

    /*
     * 4. 绘制当前帧动态元素。
     */
    render_food(food);
    render_dropped_coins(drops, drop_active);
    render_enemy(enemy);
    render_snake(snake);
    render_score(score);

    /*
     * 5. 保存当前帧。
     */
    render_save_level3_state(snake,
                             food,
                             score,
                             level,
                             enemy,
                             drops,
                             drop_active);
}

/*
 * 绘制死亡页面。
 *
 * y 坐标只使用 0, 20, 40, 56，避免红屏。
 */
void render_game_over(int score) {
    render_clear();

    LCD_ShowString(20, 0, (u8 *)"Game Over", RED);

    LCD_ShowString(20, 20, (u8 *)"Score:", WHITE);
    LCD_ShowNum(76, 20, (u16)score, 3, YELLOW);

    LCD_ShowString(0, 40, (u8 *)"CTR:Board", WHITE);
    LCD_ShowString(0, 56, (u8 *)"SW1:Menu", WHITE);
}

/*
 * 绘制排行榜中的关卡名。
 */
static void render_scoreboard_level(int x, int y, int level) {
    if (level == LEVEL_1_1) {
        LCD_ShowString(x, y, (u8 *)"1-1", WHITE);
    } else if (level == LEVEL_1_2) {
        LCD_ShowString(x, y, (u8 *)"1-2", WHITE);
    } else if (level == LEVEL_1_3) {
        LCD_ShowString(x, y, (u8 *)"1-3", WHITE);
    } else {
        LCD_ShowString(x, y, (u8 *)"---", GRAY);
    }
}

/*
 * 绘制排行榜中的一行。
 *
 * 坐标设计：
 *   y = 20, 36, 52
 *
 * 这些 y 坐标都不会超过 64，因此不会触发 LCD_ShowString 的越界红屏。
 */
static void render_scoreboard_row(int index, int y) {
    int level = scoreboard_get_level(index);
    int score = scoreboard_get_score(index);

    /*
     * 序号：1. / 2. / 3.
     */
    LCD_ShowNum(0, y, (u16)(index + 1), 1, YELLOW);
    LCD_ShowString(8, y, (u8 *)".", YELLOW);

    /*
     * 关卡名。
     */
    render_scoreboard_level(24, y, level);

    /*
     * 分数。
     */
    LCD_ShowString(64, y, (u8 *)"S:", WHITE);
    LCD_ShowNum(88, y, (u16)score, 3, YELLOW);
}

/*
 * 绘制排行榜页面。
 *
 * 页面说明：
 *   Title: Scoreboard
 *   1. 1-3 S:005
 *   2. 1-1 S:003
 *   3. 1-2 S:001
 *
 * SW1 返回菜单。
 */
void render_scoreboard(void) {
    render_clear();

    LCD_ShowString(0, 0, (u8 *)"Scoreboard", GREEN);

    render_scoreboard_row(0, 20);
    render_scoreboard_row(1, 36);
    render_scoreboard_row(2, 52);

    /*
     * y = 64 是 8x16 字体的最后一行安全位置。
     */
    LCD_ShowString(0, 64, (u8 *)"SW1 Back", WHITE);
}