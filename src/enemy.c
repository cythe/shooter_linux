#include <stdlib.h>
#include <string.h>
#include "type.h"
#include "bullet.h"
#include "sound.h"
#include "stage.h"
#include "effect.h"
#include "theme.h"
#include "tools.h"

LIST_HEAD(enemies);
SDL_Texture* enemieTexture;
#if 1
static void enemy_fire_bullet(Player* e)
{
    Bullet* bullet;
    Player* player = get_player();

    bullet = malloc(sizeof(Bullet));
    memset(bullet, 0, sizeof(Bullet));

    list_add(&bullet->list, &enemies_bullets);

    bullet->r.x = e->r.x;
    bullet->r.y = e->r.y;
    bullet->health = 1;
    bullet->texture = enemieBulletTexture;
    SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->r.w, &bullet->r.h);

    bullet->r.x += (e->r.w / 2) - (bullet->r.w / 2);
    bullet->r.y += (e->r.h / 2) - (bullet->r.h / 2);

    float fdx, fdy;
    float dx, dy;
    calc_slope(player->r.x + (player->r.w / 2), player->r.y + (player->r.h / 2), e->r.x, e->r.y, &fdx, &fdy);
    fdx *= 4;
    fdy *= 4;
    bullet->fdx = modff(fdx, &dx);
    bullet->fdy = modff(fdy, &dy);
    bullet->dx = (int)dx;
    bullet->dy = (int)dy;
    e->bullet_reload = (rand() % 60 * 2);
}
#else
static void enemy_fire_bullet(Enemy * e)
{
    Bullet * bullet;
    Player * player = get_player();

    int d = 0;
    for (int i = 0; i < 36; i++)
    {
	bullet = malloc(sizeof(Bullet));
	memset(bullet, 0, sizeof(Bullet));

	list_add(&bullet->list, &enemies_bullets);
	bullet->r.x = e->r.x;
	bullet->r.y = e->r.y;
	bullet->health = 1;
	bullet->speed = 8;
	bullet->texture = enemieBulletTexture;
	SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->r.w, &bullet->r.h);

	bullet->r.x += (e->r.w / 2) - (bullet->r.w / 2);
	bullet->r.y += (e->r.h / 2) - (bullet->r.h / 2);

	SDL_Point p = { e->r.x, e->r.y };
	SDL_Point dp;

	calculate_circle_point(p, 300, d, &dp);
	d += 10;
	//calculate_line_point(p, bullet->target, &dp);
	float fdx, fdy;
	float dx, dy;
	calc_slope(dp.x, dp.y, e->r.x, e->r.y, &fdx, &fdy);
	fdx *= bullet->speed;
	fdy *= bullet->speed;
	calculate_circle_speed(bullet->speed, d, &fdx, &fdy);
	bullet->fdx = modff(fdx, &dx);
	bullet->fdy = modff(fdy, &dy);
	bullet->dx = (int)dx;
	bullet->dy = (int)dy;
	//printf("x=%d, y=%d, dx=%f, dy=%f\n", p.x, p.y, bullet->dy, bullet->dy);
    }
    e->reload = 10; // (rand() % 60);
}

#endif

int enemySpawnTimer = 30;
void spawn_one_circle_enemy(void)
{
    Enemy * e;

    if (--enemySpawnTimer <= 0) {
	e = malloc(sizeof(Player));
	memset(e, 0, sizeof(Player));

	list_add_tail(&e->list, &enemies);

	e->r.x = SCREEN_WIDTH/2;
	e->r.y = SCREEN_HEIGHT/2;
	e->health = 1;
	e->bullet_reload = 60 * (1 + (rand() % 3));
	e->texture = enemieTexture;

	SDL_QueryTexture(e->texture, NULL, NULL, &e->r.w, &e->r.h);

	e->dx = -(2 + (rand() % 4));

	if (e->r.y > SCREEN_HEIGHT - e->r.h) {
	    e->r.y = SCREEN_HEIGHT - e->r.h;
	}

	enemySpawnTimer = 30 + (rand() % 60);

    }
}

void spawn_one_enemy(void)
{
    Enemy* e;

    if (--enemySpawnTimer <= 0) {
	e = malloc(sizeof(Player));
	memset(e, 0, sizeof(Player));

	list_add_tail(&e->list, &enemies);

	e->r.x = SCREEN_WIDTH;
	e->r.y = rand() % SCREEN_HEIGHT;
	e->health = 1;
	e->bullet_reload = 60 * (1 + (rand() % 3));
	e->texture = enemieTexture;

	SDL_QueryTexture(e->texture, NULL, NULL, &e->r.w, &e->r.h);

	e->dx = -(2 + (rand() % 4));

	if (e->r.y > SCREEN_HEIGHT - e->r.h) {
	    e->r.y = SCREEN_HEIGHT - e->r.h;
	}

	enemySpawnTimer = 30 + (rand() % 60);
    }
}
void spawn_enemies(void)
{
    spawn_one_enemy();
    //spawn_one_circle_enemy();
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

	if (out_of_screen(&e->r) || e->health == 0) {
	    pos = list_del_update_pos(pos);
	    if (e->health == 0) {
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


