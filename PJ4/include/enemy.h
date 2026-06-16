//level1-3敌方蛇AI
// Level 1-3 敌方蛇 AI
#ifndef ENEMY_H
#define ENEMY_H

#include "types.h"
#include "config.h"
#include "snake.h"
#include "item.h"

/*
 * Level 1-3 的敌方蛇。
 *
 * body[0] 是敌方蛇头。
 * 敌方蛇长度固定为 ENEMY_SNAKE_LEN，也就是 3。
 */
typedef struct {
    Point body[ENEMY_SNAKE_LEN];
    Direction dir;
    int alive;
} EnemySnake;

/*
 * 初始化敌方蛇。
 */
void enemy_init(EnemySnake *enemy);

/*
 * 更新敌方蛇。
 *
 * 如果场上有金币，则敌方蛇朝金币移动。
 * 如果没有金币，则保持当前方向巡航。
 * 如果距离边界过近，则优先右转。
 */
void enemy_update(EnemySnake *enemy, const Item *food);

/*
 * 判断玩家蛇头是否撞到敌方蛇。
 *
 * 返回 1 表示玩家死亡。
 */
int enemy_player_hits_enemy(const EnemySnake *enemy, const Snake *player);

/*
 * 判断敌方蛇头是否撞到玩家蛇身体。
 *
 * 返回 1 表示敌方蛇死亡。
 */
int enemy_hits_player_body(const EnemySnake *enemy, const Snake *player);

/*
 * 敌方蛇死亡后，在原来的 3 个身体格子处掉落 3 个金币。
 */
void enemy_make_drops(const EnemySnake *enemy,
                      Point drops[ENEMY_SNAKE_LEN],
                      int active[ENEMY_SNAKE_LEN]);

#endif