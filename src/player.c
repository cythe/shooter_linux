#include <string.h>
#include "player.h"
#include "enemy.h"
#include "type.h"
#include "theme.h"
#include "bullet.h"
#include "sound.h"
#include "event.h"
#include "stage.h"
#include "tools.h"

Player g_player = { 0 };
SDL_Texture* playerTexture;
SDL_Texture* shieldTexture;

void init_player_texture(void)
{
    shieldTexture = loadTexture("resource/shield.png");
    if (!shieldTexture) {
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
		"Couldn't load shield texture: %s\n", SDL_GetError());
    }

    playerTexture = loadTexture("resource/player.png");
    if (!playerTexture) {
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
		"Couldn't load player texture: %s\n", SDL_GetError());
    }
}

int init_player(Player* player)
{
    player->speed = PLAYER_SPEED;
    player->r.x = SCREEN_WIDTH/2;
    player->r.y = SCREEN_HEIGHT - player->r.h - 100;
    player->health = 3;
    player->shield_health = 1;
    player->bullet_level = 1;
    player->missile_level = 1;
    player->bomb = 3;
    player->id_sound_die = SND_PLAYER_DIE;

    player->texture = playerTexture;
    player->shield = shieldTexture;

    SDL_QueryTexture(player->texture, NULL, NULL, &player->r.w, &player->r.h);
    SDL_QueryTexture(player->shield, NULL, NULL, &player->shield_r.w, &player->shield_r.h);

    return 0;
}

static void player_fire_bomb(Player * e)
{
    Bullet * bullet;
    Player * player = get_player();

    int d = 0;
    for (int i = 0; i < 360; i++)
    {
	bullet = malloc(sizeof(Bullet));
	memset(bullet, 0, sizeof(Bullet));

	list_add(&bullet->list, &player_bomb_bullets);
	bullet->r.x = e->r.x;
	bullet->r.y = e->r.y;
	bullet->type = BULLET_TYPE_BOMB;
	bullet->health = 1;
	bullet->speed = 8;
	bullet->texture = enemieBulletTexture;
	SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->r.w, &bullet->r.h);

	bullet->r.x += (e->r.w / 2) - (bullet->r.w / 2);
	bullet->r.y += (e->r.h / 2) - (bullet->r.h / 2);

	SDL_Point p = { e->r.x, e->r.y };
	SDL_Point dp;

	calculate_circle_point(p, 600, d, &dp);
	d += 1;
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
    g_player.bomb_reload = 100;
}

void player_fire_missile(Player* player)
{
    // 导弹
    for(int i = 0; i < player->missile_level; i++)
    {
	Bullet* bullet = malloc(sizeof(Bullet));

	memset(bullet, 0, sizeof(Bullet));

	list_add_tail(&bullet->list, &player_missiles);

	bullet->r.x = player->r.x;
	bullet->r.y = player->r.y;
	bullet->type = BULLET_TYPE_MISSILE;
	bullet->health = 1;
	bullet->texture = playerBulletTexture;
	SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->r.w, &bullet->r.h);

	bullet->r.x += i * bullet->r.w;
	bullet->r.x += (g_player.r.w / 2) - (bullet->r.w / 2);

	Enemy* e = list_to_Enemy(enemies.next);
	SDL_Point p;
	get_center(&e->r, &p);
	bullet->target = p;

	float fdx, fdy;
	float dx, dy;
	calc_slope(p.x, p.y, e->r.x, e->r.y, &fdx, &fdy);
	fdx *= PLAYER_MISSILE_SPEED;
	fdy *= PLAYER_MISSILE_SPEED;
	bullet->fdx = modff(fdx, &dx);
	bullet->fdy = modff(fdy, &dy);
	bullet->dx = (int)dx;
	bullet->dy = (int)dy;
    }

    g_player.missile_reload = 40;
}

void player_fire_bullet(Player* player)
{
    int jiange = 14;
    // 普通子弹
    for(int i = 0; i < player->bullet_level; i++)
    {
	Bullet* bullet = malloc(sizeof(Bullet));

	memset(bullet, 0, sizeof(Bullet));

	list_add_tail(&bullet->list, &player_bullets);

	bullet->r.x = player->r.x;
	bullet->r.y = player->r.y;
	bullet->dy = -PLAYER_BULLET_SPEED;
	bullet->type = BULLET_TYPE_NORMAL;
	bullet->health = 1;
	bullet->texture = playerBulletTexture;
	SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->r.w, &bullet->r.h);

	bullet->r.x = (player->r.x + player->r.w/2);
	bullet->r.x += i*(jiange+bullet->r.w);
	bullet->r.x -= i*((i-1)*bullet->r.w + i*jiange)/2;
    }

    g_player.bullet_reload = 8;

}

void logic_player(Player* player)
{
    int speed = 0;

    if (player == NULL)
	return;

    if (player->bullet_reload > 0) {
	player->bullet_reload--;
    }

    if (player->missile_reload > 0) {
	player->missile_reload--;
    }
    if (player->bomb_reload > 0) {
	player->bomb_reload--;
    }

    player->dx = player->dy = 0;

    speed = PLAYER_SPEED;

    /* 上下左右移动 */
    if (get_keyboard(SDL_SCANCODE_UP)) {
	player->dy = -speed;
    }

    if (get_keyboard(SDL_SCANCODE_DOWN)) {
	player->dy = speed;
    }

    if (get_keyboard(SDL_SCANCODE_LEFT)) {
	player->dx = -speed;
    }

    if (get_keyboard(SDL_SCANCODE_RIGHT)) {
	player->dx = speed;
    }

    /* X 变速, 速度减半 */
    if (get_keyboard(SDL_SCANCODE_X)) {
	player->dx /= 3;
	player->dy /= 3;
    }

    player->r.x += player->dx;
    player->r.y += player->dy;
    player->shield_r.x += player->dx;
    player->shield_r.y += player->dy;

    clip_ship(player);
    /* Z 射击, reload 控制炮弹频率 */
    if (get_keyboard(SDL_SCANCODE_Z)) {
	if (player->bullet_reload == 0) {
	    play_sound(SND_PLAYER_FIRE, CH_PLAYER);
	    player_fire_bullet(&g_player);
	}

	if (player->missile_reload ==0 ){
	    //play_sound(SND_PLAYER_FIRE, CH_PLAYER);
	    player_fire_missile(&g_player);
	}
    }

    if (get_keyboard(SDL_SCANCODE_C)) {
	if (player->bomb_reload == 0 && player->bomb > 0) {
	    player->bomb --;
	    player_fire_bomb(&g_player);
	}
    }
}

void draw_player(Player* player)
{
    blit(player->texture, player->r.x, player->r.y);
#if DEBUG
    draw_rect(&player->r, 0xff00ff00);
#endif

    //SDL_SetTextureBlendMode(player->shield, SDL_BLENDMODE_BLEND);
    //SDL_SetTextureAlphaMod(player->shield, 255);
    if (player->shield_health > 0) {
	blit(player->shield , player->r.x + player->r.w/2 - player->shield_r.w/2,
		player->r.y + player->r.h/2 - player->shield_r.h/2);
    }
}
