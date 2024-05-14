#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "SDL2/SDL_image.h"
#include "sound.h"
#include "player.h"
#include "enemy.h"
#include "bullet.h"
#include "stage.h"
#include "theme.h"
#include "hud.h"
#include "point.h"
#include "effect.h"
#include "event.h"

static void capFrameRate(unsigned* then, int* remainder)
{
    long wait, frameTime;

    wait = 16 + *remainder;

    *remainder -= (int)*remainder;

    frameTime = SDL_GetTicks() - *then;

    wait -= frameTime;

    if (wait < 1) {
	wait = 1;
    }

    SDL_Delay(wait);

    *remainder += 1;

    *then = SDL_GetTicks();
}

int main(int argc, char* argv[])
{
    unsigned then;
    int remainder;

    then = SDL_GetTicks();
    remainder = 0;

    int windowFlags = 0;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
		"Couldn't initialize SDL: %s\n", SDL_GetError());
	exit(1);
    }

    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

    SDL_ShowCursor(0);

    window = SDL_CreateWindow("Shooter", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
    if (!window) {
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
		"Couldn't open %d x %d window: %s\n",
		SCREEN_WIDTH, SCREEN_HEIGHT, SDL_GetError());
	exit(1);
    }
    //SDL_RenderSetScale(render, TT_PIC_POWER, TT_PIC_POWER);
    //SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!render) {
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
		"Couldn't create render: %s\n", SDL_GetError());
	exit(1);
    }

    init_stage();

    while (!g_game_exit)
    {
	/* 每一帧都应该清除Render内数据 */
	SDL_SetRenderDrawColor(render, 96, 128, 255, 255);
	SDL_RenderClear(render);

	/* 事件查询 */
	accept_event();

	/* 环境逻辑运算 */
	logic_theme();

	/* 各对象逻辑运算 */
	logic_player(&g_player);
	logic_bullets();
	logic_enemies();
	logic_destroy_effect();
	logic_points();

	/* 绘制环境 */
	draw_theme();

	/* 绘制各对象 */
	draw_player(&g_player);
	draw_enemies();
	draw_bullets();
	draw_destroy_effect();
	draw_point_pods();
	draw_hud();

	/* 召唤敌军 */
	spawn_enemies();

	/* 输出一帧图像 */
	SDL_RenderPresent(render);

	/* 帧数控制 */
	capFrameRate(&then, &remainder);
    }

    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
    SDL_Quit();
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
	    "bye-bye: %s\n", SDL_GetError());

    return 0;
}
