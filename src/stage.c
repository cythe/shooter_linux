#include <stdio.h>
#include "type.h"
#include "stage.h"
#include "sound.h"
#include "bullet.h"
#include "theme.h"
#include "player.h"
#include "enemy.h"
#include "point.h"
#include "effect.h"
#include "hud.h"
#include "event.h"

LIST_HEAD(stage1_enemies);

int score = 0;
int highscore = 0;

SDL_Window* window;
SDL_Renderer* render;

Player* get_player(void)
{
    return &g_player;
}

int get_screen_width(void)
{
    return SCREEN_WIDTH;
}

int get_screen_height(void)
{
    return SCREEN_HEIGHT;
}

int init_stage(void)
{
    score = 0;
    init_bullet_texture();
    init_sounds();
    init_theme();
    init_enemies_texture();
    init_point_texture();
    init_explosion_texture();
    init_hud_texture();
    init_player_texture();
    if (init_player(&g_player)) {
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
		"Couldn't create player: %s\n", SDL_GetError());
	exit(1);
    }

    return 0;
}

int stage_frame = 0;
int current_frame = 0;

void spawn_five_enemy(int t)
{
    Enemy* e;

    e = malloc(sizeof(Player));
    memset(e, 0, sizeof(Player));

    list_add_tail(&e->list, &stage1_enemies);

    e->r.x = 0;
    e->r.y = SCREEN_HEIGHT / 4;
    e->health = 1;
    e->bullet_reload = 60 * (1 + (rand() % 3));
    e->texture = enemieTexture;
    e->appear_frame = stage_frame + t;
    printf("%s: appear_frame = %d\n", __func__, e->appear_frame);

    SDL_QueryTexture(e->texture, NULL, NULL, &e->r.w, &e->r.h);

    e->dx = 6;

    if (e->r.y > SCREEN_HEIGHT - e->r.h) {
	e->r.y = SCREEN_HEIGHT - e->r.h;
    }
}

int reset_stage(void)
{
    int i;

    for(i=0; i<5; i++)
	spawn_five_enemy((i+2)*30);

    return 0;
}
