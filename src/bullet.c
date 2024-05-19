#include "bullet.h"
#include "point.h"
#include "player.h"
#include "enemy.h"
#include "sound.h"
#include "stage.h"
#include "tools.h"

LIST_HEAD(player_bullets);
LIST_HEAD(player_missiles);
LIST_HEAD(player_bomb_bullets);
LIST_HEAD(enemies_bullets);

SDL_Texture* enemieBulletTexture;
SDL_Texture* playerBulletTexture;

static int catch_target(Bullet* b)
{
    return b->r.x == b->target.x && b->r.y == b->target.y;
}

static int bullet_hit_enemy(Bullet* b)
{
    struct list_head* pos;

    list_for_each(pos, &enemies)
    {
	Enemy* e = list_to_Enemy(pos);
	if (collision(&b->r, &e->r)) {
	    play_sound(SND_ALIEN_DIE, CH_ANY);
	    gen_point_pod(e->r.x + e->r.w / 2, e->r.y + e->r.h / 2);
	    score++;
	    highscore = MAX(score, highscore);
	    e->health -= b->health;

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

static void logic_player_bomb(void)
{
    struct list_head* pos;
    Bullet* b;
    int flagdx, flagdy;

    list_for_each(pos, &player_bomb_bullets)
    {
	b = list_to_Bullet(pos);
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

	bullet_hit_enemy(b);

	if (out_of_screen(&b->r) || b->health <= 0) {
	    pos = list_del_update_pos(pos);
	    free(b);
	}
    }
}

static void __logic_player_missile(Bullet *b)
{
    float fdx, fdy;
    float dx, dy;
    SDL_Point target = b->target;

    Enemy* e = list_to_Enemy(enemies.next);
    SDL_Point p;
    get_center(&e->r, &p);
    b->target = p;

    calc_slope(target.x, target.y, b->r.x, b->r.y, &fdx, &fdy);
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

static void logic_enemy_bullets(void)
{
    struct list_head* pos;
    Bullet* b;
    int flagdx, flagdy;
    list_for_each(pos, &enemies_bullets)
    {
	b = list_to_Bullet(pos);
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

	if (out_of_screen(&b->r) || bullet_hit_player(b, &g_player)) {
	    pos = list_del_update_pos(pos);
	    free(b);
	}
    }
}


static void draw_enemy_bullets(void)
{
    struct list_head* pos;

    list_for_each(pos, &enemies_bullets)
    {
	Bullet* b = list_to_Bullet(pos);
	blit(b->texture, b->r.x, b->r.y);
#if DEBUG
	draw_rect(&b->r, YELLOW);
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
	draw_rect(&b->r, YELLOW);
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
	draw_rect(&b->r, YELLOW);
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
	draw_rect(&b->r, YELLOW);
#endif
#else
	spin_rect(&b->r, b->texture, 45);
#endif
    }
}

void init_bullet_texture(void)
{
    playerBulletTexture = loadTexture("./resource/bullet.png");
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
