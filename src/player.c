#include <string.h>
#include "player.h"
#include "type.h"
#include "bullet.h"
#include "sound.h"
#include "event.h"
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
    player->r.x = 100;
    player->r.y = 100;
    player->health = 1;
    player->bullet_level = 1;
    player->id_sound_die = SND_PLAYER_DIE;

    player->texture = playerTexture;
    player->shield = shieldTexture;

    SDL_QueryTexture(player->texture, NULL, NULL, &player->r.w, &player->r.h);
    SDL_QueryTexture(player->shield, NULL, NULL, &player->shield_r.w, &player->shield_r.h);

    return 0;
}

void player_fire_bullet(Player* player, int bullet_level)
{
    for(int i = 0; i < bullet_level; i++)
    {
	Bullet* bullet = malloc(sizeof(Bullet));

	memset(bullet, 0, sizeof(Bullet));

	list_add_tail(&bullet->list, &player_bullets);

	bullet->r.x = player->r.x;
	bullet->r.y = player->r.y;
	bullet->dx = PLAYER_BULLET_SPEED;
	bullet->health = 1;
	bullet->texture = playerBulletTexture;
	SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->r.w, &bullet->r.h);

	bullet->r.y += i * bullet->r.w;
	bullet->r.y += (g_player.r.h / 2) - (bullet->r.h / 2);
    }

    g_player.reload = 8;

}

void logic_player(Player* player)
{
    int speed = 0;

    if (player == NULL)
	return;

    if (player->reload > 0) {
	player->reload--;
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
	player->dx /= 2;
	player->dy /= 2;
    }

    player->r.x += player->dx;
    player->r.y += player->dy;
    player->shield_r.x += player->dx;
    player->shield_r.y += player->dy;

    clip_ship(player);
    /* Z 射击, reload 控制炮弹频率 */
    if (get_keyboard(SDL_SCANCODE_Z) && player->reload == 0) {
	play_sound(SND_PLAYER_FIRE, CH_PLAYER);
	player_fire_bullet(&g_player, g_player.bullet_level);
    }
}

void draw_player(Player* player)
{
    blit(player->texture, player->r.x, player->r.y);
#if DEBUG
    draw_rect(&player->r, GREEN);
#endif
    //spin_rect(&player->r, player->texture, 45);

    //SDL_SetTextureBlendMode(player->shield, SDL_BLENDMODE_BLEND);
    //SDL_SetTextureAlphaMod(player->shield, 255);
    blit(player->shield , player->r.x + player->r.w/2 - player->shield_r.w/2,
	    player->r.y + player->r.h/2 - player->shield_r.h/2);
}
