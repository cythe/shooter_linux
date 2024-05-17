#pragma once
#include "type.h"

#define PLAYER_SPEED 6
#define PLAYER_BULLET_SPEED 16
#define PLAYER_BULLET_LEVEL_MAX 5

extern Player g_player;

void init_player_texture(void);

int init_player(Player* player);
void logic_player(Player* player);
void draw_player(Player* player);
