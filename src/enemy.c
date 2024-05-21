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

LIST_HEAD(enemies);
SDL_Texture* enemieTexture;

int balabala = 720;
int delta = 0;
int current = 0;
int start = 0;
int end = 0;
static void __enemy_fire_sector_bullet(Enemy * e)
{
    Bullet * bullet;

    balabala = 0;
    bullet = malloc(sizeof(Bullet));
    memset(bullet, 0, sizeof(Bullet));

    list_add(&bullet->list, &enemies_bullets);
    bullet->r.x = e->r.x;
    bullet->r.y = e->r.y;
    bullet->health = 1;
    bullet->speed = 4;
    bullet->texture = enemieBulletTexture;
    SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->r.w, &bullet->r.h);

    bullet->r.x += (e->r.w / 2) - (bullet->r.w / 2);
    bullet->r.y += (e->r.h / 2) - (bullet->r.h / 2);

    SDL_Point p = { e->r.x, e->r.y };
    SDL_Point dp;

    calculate_circle_point(p, 300, current, &dp);
    //calculate_line_point(p, bullet->target, &dp);
    float fdx, fdy;
    float dx, dy;
    calc_slope(dp.x, dp.y, e->r.x, e->r.y, &fdx, &fdy);
    fdx *= bullet->speed;
    fdy *= bullet->speed;
    calculate_circle_speed(bullet->speed, current, &fdx, &fdy);
    bullet->fdx = modff(fdx, &dx);
    bullet->fdy = modff(fdy, &dy);
    bullet->dx = (int)dx;
    bullet->dy = (int)dy;
    current += delta;
    if (current == end) {
	balabala = 1;
	e->bullet_reload = 80; // (rand() % 60);
    } else {
	e->bullet_reload = 5; // (rand() % 60);
    }
}

void _enemy_fire_sector_bullet(Enemy *e)
{
    if (balabala > 0) {
	start = 90;
	end = 180;
	delta = 10;
	current = 90;
	__enemy_fire_sector_bullet(e);
    }else{
	__enemy_fire_sector_bullet(e);
    }
}

static void __enemy_fire_circle_bullet(Enemy * e, int d)
{
    Bullet * bullet;

    for (int i = 0; i < 360; i+=d)
    {
	printf("i = %d\n", i);
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

	calculate_circle_point(p, 300, i, &dp);
	//calculate_line_point(p, bullet->target, &dp);
	float fdx, fdy;
	float dx, dy;
	calc_slope(dp.x, dp.y, e->r.x, e->r.y, &fdx, &fdy);
	fdx *= bullet->speed;
	fdy *= bullet->speed;
	calculate_circle_speed(bullet->speed, i, &fdx, &fdy);
	bullet->fdx = modff(fdx, &dx);
	bullet->fdy = modff(fdy, &dy);
	bullet->dx = (int)dx;
	bullet->dy = (int)dy;
	//printf("x=%d, y=%d, dx=%f, dy=%f\n", p.x, p.y, bullet->dy, bullet->dy);
    }
    e->bullet_reload = 10; // (rand() % 60);
}

static void __enemy_fire_single_bullet(Player* e)
{
    Bullet* bullet;
    Player* player = get_player();

    bullet = malloc(sizeof(Bullet));
    memset(bullet, 0, sizeof(Bullet));

    list_add(&bullet->list, &enemies_bullets);

    bullet->r.x = e->r.x;
    bullet->r.y = e->r.y;
    bullet->type = BULLET_TYPE_NORMAL;
    bullet->health = 1;
    bullet->texture = enemieBulletTexture;
    SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->r.w, &bullet->r.h);

    bullet->r.x += (e->r.w / 2) - (bullet->r.w / 2);
    bullet->r.y += (e->r.h / 2) - (bullet->r.h / 2);

    float fdx, fdy;
    float dx, dy;
    calc_slope(player->r.x + (player->r.w / 2), player->r.y + (player->r.h / 2), e->r.x, e->r.y, &fdx, &fdy);
    fdx *= ENEMY_BULLET_SPEED;
    fdy *= ENEMY_BULLET_SPEED;
    bullet->fdx = modff(fdx, &dx);
    bullet->fdy = modff(fdy, &dy);
    bullet->dx = (int)dx;
    bullet->dy = (int)dy;
    e->bullet_reload = (rand() % 60 * 2);

}

static void enemy_fire_bullet(Player* e)
{
    //__enemy_fire_single_bullet(e);
    //__enemy_fire_circle_bullet(e, 15);
    _enemy_fire_sector_bullet(e);
}

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
	e->bullet_reload = 60; // * (1 + (rand() % 3));
	e->texture = enemieTexture;

	SDL_QueryTexture(e->texture, NULL, NULL, &e->r.w, &e->r.h);

	//e->dx = -(2 + (rand() % 4));

	if (e->r.y > SCREEN_HEIGHT - e->r.h) {
	    e->r.y = SCREEN_HEIGHT - e->r.h;
	}

	enemySpawnTimer = 6000;//30 + (rand() % 60);

    }
}

void spawn_one_enemy(void)
{
    Enemy* e;

    if (--enemySpawnTimer <= 0) {
	e = malloc(sizeof(Player));
	memset(e, 0, sizeof(Player));

	list_add_tail(&e->list, &enemies);

	e->r.x = rand() % SCREEN_HEIGHT;
	e->r.y = 0;
	e->health = 1;
	e->bullet_reload = 60 * (1 + (rand() % 3));
	e->texture = enemieTexture;

	SDL_QueryTexture(e->texture, NULL, NULL, &e->r.w, &e->r.h);

	e->dy = 2 + (rand() % 4);

	if (e->r.y > SCREEN_HEIGHT - e->r.h) {
	    e->r.y = SCREEN_HEIGHT - e->r.h;
	}

	enemySpawnTimer = 30 + (rand() % 60);
    }
}

void spawn_enemies(void)
{
    //spawn_one_enemy();
    spawn_one_circle_enemy();
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

	if (out_of_screen(&e->r) || e->health <= 0) {
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


