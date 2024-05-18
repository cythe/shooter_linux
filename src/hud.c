#include <stdio.h>
#include <string.h>
#include "theme.h"
#include "type.h"
#include "hud.h"
#include "stage.h"
#include "tools.h"
#include "player.h"

SDL_Texture* fontTexture;
char drawTextBuffer[MAX_LINE_LENGTH] = { 0 };

static void draw_text(int x, int y, int r, int g, int b, char* format, ...)
{
    int i, c;
    size_t len;
    SDL_Rect rect;
    va_list args;

    memset(&drawTextBuffer, '\0', sizeof(drawTextBuffer));

    va_start(args, format);
    //vsprintf_s(drawTextBuffer, MAX_LINE_LENGTH, format, args);
    vsprintf(drawTextBuffer, format, args);
    va_end(args);

    len = strlen(drawTextBuffer);

    rect.w = GLYPH_WIDTH;
    rect.h = GLYPH_HEIGHT;
    rect.y = 0;

    SDL_SetTextureColorMod(fontTexture, r, g, b);

    for (i = 0; i < len; i++)
    {
	c = drawTextBuffer[i];

	if (c >= ' ' && c <= 'Z') {
	    rect.x = (c - ' ') * GLYPH_WIDTH;

	    blitRect(fontTexture, &rect, x, y);

	    x += GLYPH_WIDTH;
	}
    }
}

void init_hud_texture(void)
{
    fontTexture = loadTexture("resource/font.png");
    if (!fontTexture)
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
		"Couldn't load font texture: %s\n", SDL_GetError());

}

void draw_hud(void)
{
    if (score > 0 && score == highscore) {
	draw_text(SCREEN_WIDTH/2-17*18/2, 10, 0, 255, 0, "HIGH SCORE: %03d", highscore);
    } else {
	draw_text(SCREEN_WIDTH/2-17*18/2, 10, 255, 255, 255, "HIGH SCORE: %03d", highscore);
    }

    draw_text(960, 10, 255, 255, 255, "SCORE:   %03d", score);
    draw_text(960, 40, 255, 255, 255, "PLAYERS: %03d", g_player.health);
    draw_text(960, 70, 255, 255, 255, "POWER:   %03d", g_player.bullet_level);
    draw_text(960, 100, 255, 255, 255, "MISSILE: %03d", g_player.missile_level);
    draw_text(960, 130, 255, 255, 255, "BOMB:    %03d", g_player.bomb);
}

