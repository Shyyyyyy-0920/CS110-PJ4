//绘制菜单、蛇、金币、墙、排行榜等
#include "render.h"

#include "config.h"
#include "lcd/lcd.h"

/*
 * 清空屏幕。
 */
void render_clear(void) {
    BACK_COLOR = BLACK;
    LCD_Clear(BLACK);
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
 * 绘制一个格子。
 */
static void render_cell(Point p, u16 color) {
    int x = render_cell_x(p.x);
    int y = render_cell_y(p.y);

    LCD_Fill(x,
             y,
             x + CELL_SIZE - 1,
             y + CELL_SIZE - 1,
             color);
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
 * LCD_ShowNum(x, y, num, len, color) 会固定显示 len 位。
 * 这里用 3 位足够当前阶段测试，例如 000, 001, 012。
 */
static void render_score(int score) {
    LCD_ShowString(0, 0, (u8 *)"S:", WHITE);
    LCD_ShowNum(16, 0, (u16)score, 3, YELLOW);
}

/*
 * 绘制金币。
 */
static void render_coin(const Item *coin) {
    if (coin == 0 || !coin->active) {
        return;
    }

    if (coin->type == ITEM_COIN) {
        render_cell(coin->pos, RED);
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
 * 绘制基础游戏画面。
 *
 * 当前仍然采用全屏重画，后续再做防闪烁优化。
 */
void render_game_basic(const Snake *snake, const Item *coin, int score) {
    render_clear();

    render_game_border();
    render_coin(coin);
    render_snake(snake);
    render_score(score);
}

/*
 * 绘制死亡页面。
 */
void render_game_over(int score) {
    render_clear();

    LCD_ShowString(20, 16, (u8 *)"Game Over", RED);
    LCD_ShowString(20, 40, (u8 *)"Score:", WHITE);
    LCD_ShowNum(76, 40, (u16)score, 3, YELLOW);
}