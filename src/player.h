#pragma once
#include "type.h"

#define PLAYER_SPEED 8
#define PLAYER_BULLET_SPEED 16
#define PLAYER_BULLET_LEVEL_MAX 5

#define PLAYER_MISSILE_SPEED 8
#define PLAYER_MISSILE_LEVEL_MAX 2

extern Player g_player;

void init_player_texture(void);

int init_player(Player* player);
void logic_player(Player* player);
void draw_player(Player* player);
