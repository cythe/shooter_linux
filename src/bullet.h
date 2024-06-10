#pragma once
#include "type.h"

/* ----------------------------------------------------------------------*/
/**
 * @brief : 子弹链表们
 */
/* ----------------------------------------------------------------------*/
extern struct list_head enemies_bullets;
extern struct list_head player_bullets;
extern struct list_head player_missiles;
extern struct list_head player_bomb_bullets;

extern SDL_Texture* playerBulletTexture;
extern SDL_Texture* enemieBulletTexture;

void init_bullet_texture(void);

void logic_bullets(void);

void draw_bullets(void);

/* ----------------------------------------------------------------------*/
/**
 * @brief : 发射单个子弹
 *	    子弹会判断当前玩家位置瞄准
 *
 * @param : arg (可选)
 *	    子弹的属性
 */
/* ----------------------------------------------------------------------*/
void fire_single_bullet(struct _ship *s, void* arg);

/* ----------------------------------------------------------------------*/
/**
 * @brief : 发射一圈子弹
 *
 * @param : arg (可选)
 *	    子弹的属性
 */
/* ----------------------------------------------------------------------*/
void fire_circle_bullet(struct _ship *s, void* arg);

/* ----------------------------------------------------------------------*/
/**
 * @brief : 发射扇型子弹
 *
 * @param : arg (必须)
 *	    子弹的属性, struct _sector_bullet* sector.
 */
/* ----------------------------------------------------------------------*/
void fire_sector_bullet(struct _ship *s, void* arg);
