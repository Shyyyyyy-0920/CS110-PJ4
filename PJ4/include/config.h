//全局配置：屏幕尺寸、网格大小、FPS、颜色等
#ifndef CONFIG_H
#define CONFIG_H

/*
 * 当前 LCD 驱动 USE_HORIZONTAL = 3，因此是 160 x 80 横屏。
 */
#define SCREEN_WIDTH        160
#define SCREEN_HEIGHT       80

/*
 * LCD_ShowString 使用 8x16 字体。
 */
#define FONT_WIDTH          8
#define FONT_HEIGHT         16
#define MAX_TEXT_Y          64

/*
 * 贪吃蛇网格配置。
 * 160 x 80，每格 8 像素，得到 20 x 10 地图。
 */
#define CELL_SIZE           8
#define GRID_WIDTH          (SCREEN_WIDTH / CELL_SIZE)
#define GRID_HEIGHT         (SCREEN_HEIGHT / CELL_SIZE)

/*
 * 当前阶段的游戏速度。
 */
#define GAME_FPS            50
#define FRAME_TIME_MS       (1000 / GAME_FPS)

#define SNAKE_NORMAL_STEP_MS 350
#define SNAKE_FAST_STEP_MS   170

/*
 * 蛇最大长度。
 */
#define MAX_SNAKE_LEN       200

/*
 * 金币存在时间：10 秒。
 */
#define COIN_LIFE_MS        10000

/*
 * Level 1-2 / 1-3 后续使用。
 */
#define MAX_WALLS           16
#define MAX_PORTALS         2
#define ENEMY_SNAKE_LEN     3
#define GEM_LIFE_MS         5000

/*
 * 菜单配置。
 */
#define LEVEL_COUNT         3

#define MENU_TITLE_X        0
#define MENU_TITLE_Y        0

#define MENU_CURSOR_X       0
#define MENU_ITEM_X         16
#define MENU_ITEM_Y0        20
#define MENU_ITEM_GAP       16

#endif