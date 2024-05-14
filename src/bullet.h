#pragma once
#include "type.h"

extern struct list_head enemies_bullets;
extern struct list_head player_bullets;

extern SDL_Texture* playerBulletTexture;
extern SDL_Texture* enemieBulletTexture;

void init_bullet_texture(void);

void logic_bullets(void);
void draw_bullets(void);
