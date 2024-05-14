#include <stdlib.h>
#include "theme.h"
#include "stage.h"
#include "tools.h"

int backgroundX = 6;
static void logic_background(void)
{
    if (--backgroundX < -SCREEN_WIDTH) {
	backgroundX = 0;
    }
}

SDL_Texture* background;

static void draw_background(void)
{
    SDL_Rect dest;
    int x;

    for (x = backgroundX; x < SCREEN_WIDTH; x += SCREEN_WIDTH)
    {
	dest.x = x;
	dest.y = 0;
	dest.w = SCREEN_WIDTH;
	dest.h = SCREEN_HEIGHT;

	SDL_RenderCopy(render, background, NULL, &dest);
    }
}

static Star stars[MAX_STARS] = { 0 };

static void init_star(void)
{
    int i;

    for (i = 0; i < MAX_STARS; i++)
    {
	stars[i].x = rand() % SCREEN_WIDTH;
	stars[i].y = rand() % SCREEN_HEIGHT;
	stars[i].speed = 1 + rand() % 8;
    }
}

static void draw_stars(void)
{
    int i, c;

    for (i = 0; i < MAX_STARS; i++)
    {
	c = 32 * stars[i].speed;

	SDL_SetRenderDrawColor(render, c, c, c, 255);

	SDL_RenderDrawLine(render, stars[i].x, stars[i].y, stars[i].x + 3, stars[i].y);
    }
}

static void logic_stars(void)
{
    int i;

    for (i = 0; i < MAX_STARS; i++)
    {
	stars[i].x -= stars[i].speed;

	if (stars[i].x < 0) {
	    stars[i].x = SCREEN_WIDTH + stars[i].x;
	}
    }
}

void init_theme_texture(void)
{
    background = loadTexture("resource/background.png");
    if (!background)
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
		"Couldn't load background: %s\n", SDL_GetError());
}

void init_theme(void)
{
    init_theme_texture();
    init_star();
}

void logic_theme(void)
{
    logic_background();
    logic_stars();
}

void draw_theme(void)
{
    draw_background();
    draw_stars();
}

