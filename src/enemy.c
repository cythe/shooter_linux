#include <stdlib.h>
#include <string.h>
#include "type.h"
#include "bullet.h"
#include "sound.h"
#include "stage.h"
#include "enemy.h"
#include "effect.h"
#include "theme.h"
#include "tools.h"
#include <stdio.h>

LIST_HEAD(enemies);
SDL_Texture* enemieTexture;

static void enemy_fire_bullet(struct _ship *s)
{
    //__enemy_fire_single_bullet(e);
    //__enemy_fire_circle_bullet(e, 15);
    //_enemy_fire_sector_bullet(s);
    s->fire_bullet(s, s->bullet_arg);
}

void spawn_enemies(void)
{
    struct list_head* pos;
    Enemy* e;

    list_for_each(pos, &stage1_enemies)
    {
	e = list_to_Enemy(pos);

	if (e->appear_frame == current_frame) {
	    pos = list_del_update_pos(pos);
	    list_add_tail(&e->list, &enemies);
	} else {
	    break;
	}
    }
}

void init_enemies_texture(void)
{
    enemieTexture = loadTexture("./resource/enemie.png");
    if (!enemieTexture)
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
		"Couldn't load enemie texture: %s\n", SDL_GetError());
}

void logic_enemies(void)
{
    struct list_head* pos;
    Enemy* e;

    list_for_each(pos, &enemies)
    {
	e = list_to_Enemy(pos);

	e->r.x += e->dx;
	e->r.y += e->dy;

	if (out_of_screen(&e->r)) {
	    pos = list_del_update_pos(pos);
	    if (e->health <= 0) {
		gen_destroy_effect(e);
	    }
	    free(e);
	} else if (--e->bullet_reload <= 0) {
	    play_sound(SND_ALIEN_FIRE, CH_ALIEN_FIRE);
	    enemy_fire_bullet(e);
	}
    }
}

void draw_enemies(void)
{
    struct list_head* pos;

    list_for_each(pos, &enemies)
    {
	Enemy* e = list_to_Enemy(pos);
	blit(e->texture, e->r.x, e->r.y);
#if DEBUG
	draw_rect(&e->r, RED);
#endif
    }
}
