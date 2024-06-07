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

static Enemy* _spawn_one_enemy(void)
{
    Enemy* e;
    e = malloc(sizeof(Enemy));
    memset(e, 0, sizeof(Enemy));

    list_add_tail(&e->list, &stage1_enemies);

    e->texture = enemieTexture;
    SDL_QueryTexture(e->texture, NULL, NULL, &e->r.w, &e->r.h);

    return e;
}

int stage_frame = 0;
int current_frame = 0;

void spawn_little_boss(int t)
{
    Enemy* e;

    e = _spawn_one_enemy();

    e->health = 1000;
    e->bullet_reload = 60 * (1 + (rand() % 3));
    e->appear_frame = stage_frame + t;

    struct _sector_bullet *sector = malloc(sizeof(struct _sector_bullet));
    sector->start = 90;
    sector->angle = 720;
    sector->delta = -10;
    sector->change_direction = 1;
    e->bullet_arg = sector;
    e->fire_bullet = fire_sector_bullet;

    //printf("%s: appear_frame = %d\n", __func__, e->appear_frame);

    e->r.x = SCREEN_WIDTH /2;
    e->r.y = SCREEN_HEIGHT / 4;
}

void spawn_five_enemy(int direction, int t)
{
    Enemy* e;

    e = _spawn_one_enemy();

    e->health = 1;
    e->bullet_reload = 60 * (1 + (rand() % 3));
    e->appear_frame = stage_frame + t;
    e->fire_bullet = fire_single_bullet;
    //printf("%s: appear_frame = %d\n", __func__, e->appear_frame);

    if (direction > 0) {
	e->r.x = 0;
	e->r.y = SCREEN_HEIGHT / 4;
	e->dx = 6;
    } else {
	e->r.x = SCREEN_WIDTH;
	e->r.y = SCREEN_HEIGHT / 4;
	e->dx = -6;
    }
}

int reset_stage(void)
{
    int i;
    int appear_frame;

    appear_frame = 2*60;
    // 400ms出一个敌人, 计算间隔帧数: 60帧/1000ms * 400ms = 24帧
    for(i=appear_frame; i<appear_frame+24*5; i+=24)
	spawn_five_enemy(1, i);

    appear_frame = 4*60;
    for(i=appear_frame; i<appear_frame+24*5; i+=24)
	spawn_five_enemy(-1, i);

    appear_frame = 8*60;
    for(i=appear_frame; i<appear_frame+24*5; i+=24)
	spawn_five_enemy(1, i);

    appear_frame = 10*60;
    for(i=appear_frame; i<appear_frame+24*5; i+=24)
	spawn_five_enemy(-1, i);

    // 召唤小boss
    appear_frame = 15*60;
    spawn_little_boss(appear_frame);

    return 0;
}
