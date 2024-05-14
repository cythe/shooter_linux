#pragma once
#include "type.h"

extern struct list_head explosions;
extern struct list_head debrises;
extern SDL_Texture* explosionTexture;

void init_explosion_texture(void);
void gen_destroy_effect(Enemy* e);
void logic_destroy_effect(void);
void draw_destroy_effect(void);