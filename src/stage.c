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
    g_game_exit = 0; 
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

int reset_stage(void)
{
    return 0;
}
