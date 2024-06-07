#pragma once
#include "type.h"

extern struct list_head enemies_bullets;
extern struct list_head player_bullets;
extern struct list_head player_missiles;
extern struct list_head player_bomb_bullets;

extern SDL_Texture* playerBulletTexture;
extern SDL_Texture* enemieBulletTexture;

void init_bullet_texture(void);

void logic_bullets(void);
void draw_bullets(void);

// 各种子弹的发射函数组
void fire_single_bullet(struct _ship *s, void* arg);
void fire_circle_bullet(struct _ship *s, void* arg);
void fire_sector_bullet(struct _ship *s, void* arg);
