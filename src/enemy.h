#pragma once
#include "list.h"

#define ENEMY_BULLET_SPEED 8

extern struct list_head enemies;

void init_enemies_texture(void);
extern SDL_Texture* enemieTexture;

/* ----------------------------------------------------------------------*/
/**
 * @brief : 召唤一名敌人
 */
/* ----------------------------------------------------------------------*/
void spawn_enemies(void);

void logic_enemies(void);
void draw_enemies(void);
