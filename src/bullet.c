#include "bullet.h"
#include "point.h"
#include "player.h"
#include "enemy.h"
#include "theme.h"
#include "sound.h"
#include "stage.h"
#include "effect.h"
#include "buffer.h"
#include "tools.h"

LIST_HEAD(player_bullets);
LIST_HEAD(player_missiles);
LIST_HEAD(player_bomb_bullets);
LIST_HEAD(enemies_bullets);

SDL_Texture* enemieBulletTexture;
SDL_Texture* playerBulletTexture;

// 判断子弹是否到达目标点
static int catch_target(Bullet* b)
{
    return b->r.x == b->target.x && b->r.y == b->target.y;
}

// 判断子弹是否打中敌人
static int bullet_hit_enemy(Bullet* b)
{
    struct list_head* pos;

    list_for_each(pos, &enemies)
    {
	Enemy* e = list_to_Enemy(pos);
	if (collision(&b->r, &e->r)) {
	    play_sound(SND_ALIEN_DIE, CH_ANY);
	    score++;
	    highscore = MAX(score, highscore);
	    e->health -= b->health;
	    if (e->health <= 0) {
		pos = list_del_update_pos(pos);
		if (e->health <= 0) {
		    gen_point_pod(e->r.x + e->r.w / 2, e->r.y + e->r.h / 2);
		    gen_destroy_effect(e);
		}
		free(e);
	    }

	    if(b->type != BULLET_TYPE_BOMB)
		b->health = 0;

	    return 1;
	}
    }

    return 0;
}

static int bullet_hit_player(Bullet* b, Player* p)
{
    if (collision(&b->r, &p->r)) {
	play_sound(SND_PLAYER_DIE, CH_PLAYER);
	b->health = 0;

	if (p->shield_health > 0) {
	    play_sound(SND_SHIELD_SHATTER, CH_PLAYER);
	    sheild_shatter(p);
	    p->shield_health --;
	} else {
	    if (p->health>0)
		p->health--;
	}

	return 1;
    }

    return 0;
}

// 计算子弹的位置
static void move_float_bullet(Bullet *b)
{
    int flagdx, flagdy;

    b->r.x += b->dx;
    b->r.y += b->dy;

    if (b->fdx < 0)
	flagdx = -1;
    else
	flagdx = 1;

    if (b->fdy < 0)
	flagdy = -1;
    else
	flagdy = 1;

    b->fdx_sum += fabs(b->fdx);
    b->fdy_sum += fabs(b->fdy);

    if ((b->fdx_sum) > 0.5) {
	b->fdx_sum -= 1.0;
	b->r.x += flagdx * 1;
    }
    if ((b->fdy_sum) > 0.5) {
	b->fdy_sum -= 1.0;
	b->r.y += flagdy * 1;
    }

}

static void logic_player_bomb(void)
{
    struct list_head* pos;
    Bullet* b;

    list_for_each(pos, &player_bomb_bullets)
    {
	b = list_to_Bullet(pos);

	move_float_bullet(b);

	bullet_hit_enemy(b);

	if (out_of_screen(&b->r) || b->health <= 0) {
	    pos = list_del_update_pos(pos);
	    free(b);
	}
    }
}

static void __logic_player_missile(Bullet *b)
{
    SDL_Point p;
    Enemy* e;

    if (list_empty(&enemies)) {
	b->target.x = SCREEN_WIDTH/2;
	b->target.y = -SCREEN_HEIGHT;
    } else {
	e = list_to_Enemy(enemies.next);
	get_center(&e->r, &p);
	b->target = p;
    }

    float fdx, fdy;
    float dx, dy;
    calc_slope(b->target.x, b->target.y, b->r.x, b->r.y, &fdx, &fdy);
    fdx *= PLAYER_MISSILE_SPEED;
    fdy *= PLAYER_MISSILE_SPEED;
    b->fdx = modff(fdx, &dx);
    b->fdy = modff(fdy, &dy);
    b->dx = (int)dx;
    b->dy = (int)dy;

    b->r.x += b->dx;
    b->r.y += b->dy;
}

static void logic_player_missile(void)
{
    struct list_head* pos;
    Bullet* b;

    list_for_each(pos, &player_missiles)
    {
	b = list_to_Bullet(pos);

	__logic_player_missile(b);

	if (catch_target(b) || out_of_screen(&b->r) || bullet_hit_enemy(b)) {
	    pos = list_del_update_pos(pos);
	    free(b);
	}
    }
}

static void logic_player_bullets(void)
{
    struct list_head* pos;
    Bullet* b;

    list_for_each(pos, &player_bullets)
    {
	b = list_to_Bullet(pos);

	b->r.x += b->dx;
	b->r.y += b->dy;

	if (out_of_screen(&b->r) || bullet_hit_enemy(b)) {
	    pos = list_del_update_pos(pos);
	    free(b);
	}
    }
}

static void update_bullet_speed(Bullet *b)
{
    float speedx;
    float speedy;
    float speed;
    float dx, dy;

    speedx = b->dx + b->fdx;
    speedy = b->dy + b->fdy;
    speed = speedx*speedx + speedy*speedy;
    if (b->accelerate || speed < 36) {
	speedx *= b->accelerate;
	speedy *= b->accelerate;
    }

    b->fdx = modff(speedx, &dx);
    b->fdy = modff(speedy, &dy);
    b->dx = (int)dx;
    b->dy = (int)dy;
}

static void logic_enemy_bullets(void)
{
    struct list_head* pos;
    Bullet* b;

    list_for_each(pos, &enemies_bullets)
    {
	b = list_to_Bullet(pos);

	move_float_bullet(b);
	update_bullet_speed(b);

	if (out_of_screen(&b->r) || bullet_hit_player(b, &g_player)) {
	    pos = list_del_update_pos(pos);
	    free(b);
	}
    }
}

static void __fire_sector_bullet(Enemy *e, struct _sector_bullet *s)
{
    Bullet * bullet;

    //print_sector(s);
    bullet = malloc(sizeof(Bullet));
    memset(bullet, 0, sizeof(Bullet));

    list_add(&bullet->list, &enemies_bullets);
    bullet->r.x = e->r.x;
    bullet->r.y = e->r.y;
    bullet->health = 1;
    bullet->speed = 1;
    bullet->accelerate = 1.04;
    bullet->texture = enemieBulletTexture;
    SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->r.w, &bullet->r.h);

    bullet->r.x += (e->r.w / 2) - (bullet->r.w / 2);
    bullet->r.y += (e->r.h / 2) - (bullet->r.h / 2);

    SDL_Point p = { e->r.x, e->r.y };
    SDL_Point dp;

    calculate_circle_point(p, 300, s->start + s->current, &dp);
    //calculate_line_point(p, bullet->target, &dp);
    float fdx, fdy;
    float dx, dy;
    calc_slope(dp.x, dp.y, e->r.x, e->r.y, &fdx, &fdy);
    fdx *= bullet->speed;
    fdy *= bullet->speed;
    calculate_circle_speed(bullet->speed, s->start + s->current, &fdx, &fdy);
    bullet->fdx = modff(fdx, &dx);
    bullet->fdy = modff(fdy, &dy);
    bullet->dx = (int)dx;
    bullet->dy = (int)dy;
    s->current += s->delta;
    if (abs(s->current) > s->angle) {
	s->status = 0;
	s->count-- ;
	e->bullet_reload = 80; // (rand() % 60);
    } else {
	e->bullet_reload = 5; // (rand() % 60);
    }
}

void fire_sector_bullet(struct _ship *s, void* arg)
{
    struct _sector_bullet *sector = arg;

    if (sector->count == 0) {
	put_sector(sector);
	return;
    }

    if (sector->status == 0) {
	// 发射完一组后换向
	if (sector->change_direction)
	{
	    if (sector->delta > 0)
		sector->start += sector->angle;
	    else
		sector->start -= sector->angle;

	    sector->delta = 0 - sector->delta;
	    sector->current = 0;
	    sector->status = 1;
	}
	// free(sector);
	play_sound(SND_ALIEN_FIRE, CH_ALIEN_FIRE);
	__fire_sector_bullet(s, sector);
    } else {
	play_sound(SND_ALIEN_FIRE, CH_ALIEN_FIRE);
	__fire_sector_bullet(s, sector);
    }
}

void fire_circle_bullet(struct _ship *s, void *arg)
{
    Bullet * bullet;
    int delta = 10;

    play_sound(SND_ALIEN_FIRE, CH_ALIEN_FIRE);

    for (int i = 0; i < 360; i+=delta)
    {
	bullet = malloc(sizeof(Bullet));
	memset(bullet, 0, sizeof(Bullet));

	list_add(&bullet->list, &enemies_bullets);
	bullet->r.x = s->r.x;
	bullet->r.y = s->r.y;
	bullet->health = 1;
	bullet->speed = 8;
	bullet->texture = enemieBulletTexture;
	SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->r.w, &bullet->r.h);

	bullet->r.x += (s->r.w / 2) - (bullet->r.w / 2);
	bullet->r.y += (s->r.h / 2) - (bullet->r.h / 2);

	SDL_Point p = { s->r.x, s->r.y };
	SDL_Point dp;

	calculate_circle_point(p, 300, i, &dp);
	//calculate_line_point(p, bullet->target, &dp);
	float fdx, fdy;
	float dx, dy;
	calc_slope(dp.x, dp.y, s->r.x, s->r.y, &fdx, &fdy);
	fdx *= bullet->speed;
	fdy *= bullet->speed;
	calculate_circle_speed(bullet->speed, i, &fdx, &fdy);
	bullet->fdx = modff(fdx, &dx);
	bullet->fdy = modff(fdy, &dy);
	bullet->dx = (int)dx;
	bullet->dy = (int)dy;
	//printf("x=%d, y=%d, dx=%f, dy=%f\n", p.x, p.y, bullet->dy, bullet->dy);
    }
    s->bullet_reload = 10; // (rand() % 60);
}

void fire_single_bullet(struct _ship *s, void *arg)
{
    Bullet* bullet;
    Player* player = get_player();

    play_sound(SND_ALIEN_FIRE, CH_ALIEN_FIRE);

    bullet = malloc(sizeof(Bullet));
    memset(bullet, 0, sizeof(Bullet));

    list_add(&bullet->list, &enemies_bullets);

    bullet->r.x = s->r.x;
    bullet->r.y = s->r.y;
    bullet->type = BULLET_TYPE_NORMAL;
    bullet->health = 1;
    bullet->texture = enemieBulletTexture;
    SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->r.w, &bullet->r.h);

    bullet->r.x += (s->r.w / 2) - (bullet->r.w / 2);
    bullet->r.y += (s->r.h / 2) - (bullet->r.h / 2);

    float fdx, fdy;
    float dx, dy;
    calc_slope(player->r.x + (player->r.w / 2), player->r.y + (player->r.h / 2), s->r.x, s->r.y, &fdx, &fdy);
    fdx *= ENEMY_BULLET_SPEED;
    fdy *= ENEMY_BULLET_SPEED;
    bullet->fdx = modff(fdx, &dx);
    bullet->fdy = modff(fdy, &dy);
    bullet->dx = (int)dx;
    bullet->dy = (int)dy;
    s->bullet_reload = (rand() % 60 * 2);
}


static void draw_enemy_bullets(void)
{
    struct list_head* pos;

    list_for_each(pos, &enemies_bullets)
    {
	Bullet* b = list_to_Bullet(pos);
	blit(b->texture, b->r.x, b->r.y);
#if DEBUG
	draw_rect(&b->r, 0xffaa0000);
#endif
    }
}

static void draw_enemies_bullets(void)
{
    struct list_head* pos;
    Enemy* e;

    list_for_each(pos, &enemies)
    {
	e = list_to_Enemy(pos);
	draw_enemy_bullets();
    }
}

static void draw_player_bomb(void)
{
    struct list_head* pos;

    list_for_each(pos, &player_bomb_bullets)
    {
	Bullet* b = list_to_Bullet(pos);
#if 1
	blit(b->texture, b->r.x, b->r.y);
#if DEBUG
	draw_rect(&b->r, 0xffaa0000);
#endif
#else
	spin_rect(&b->r, b->texture, 45);
#endif
    }
}

static void draw_player_missile(void)
{
    struct list_head* pos;

    list_for_each(pos, &player_missiles)
    {
	Bullet* b = list_to_Bullet(pos);
#if 1
	blit(b->texture, b->r.x, b->r.y);
#if DEBUG
	draw_rect(&b->r, 0xffaa0000);
#endif
#else
	spin_rect(&b->r, b->texture, 45);
#endif
    }
}
static void draw_player_bullets(void)
{
    struct list_head* pos;

    list_for_each(pos, &player_bullets)
    {
	Bullet* b = list_to_Bullet(pos);
#if 1
	blit(b->texture, b->r.x, b->r.y);
#if DEBUG
	draw_rect(&b->r, 0xffaabb00);
#endif
#else
	spin_rect(&b->r, b->texture, 45);
#endif
    }
}

void init_bullet_texture(void)
{
    playerBulletTexture = loadTexture("./resource/player_bit.png");
    if (!playerBulletTexture)
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
		"Couldn't load bullet texture: %s\n", SDL_GetError());

    enemieBulletTexture = loadTexture("resource/alienBullet.png");
    if (!enemieBulletTexture)
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
		"Couldn't load enemie bullet texture: %s\n", SDL_GetError());
}

void logic_bullets(void)
{
    logic_player_bullets();
    logic_player_missile();
    logic_player_bomb();
    logic_enemy_bullets();
}

void draw_bullets(void)
{
    draw_player_bullets();
    draw_player_missile();
    draw_player_bomb();
    draw_enemy_bullets();
}
