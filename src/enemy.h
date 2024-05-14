#pragma once
#include "list.h"

extern struct list_head enemies;

void init_enemies_texture(void);

void spawn_enemies(void);
void logic_enemies(void);
void draw_enemies(void);