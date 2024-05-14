#include <stdlib.h>
#include <string.h>
#include "type.h"
#include "effect.h"
#include "stage.h"
#include "tools.h"

LIST_HEAD(explosions);
LIST_HEAD(debrises);

SDL_Texture* explosionTexture;

static void gen_debris(Player* e)
{
	Debris* d;
	int x, y, w, h;

	w = e->r.w / 2;
	h = e->r.h / 2;

	for (y = 0; y <= h; y += h)
	{
		for (x = 0; x <= w; x += w)
		{
			d = malloc(sizeof(Debris));
			memset(d, 0, sizeof(Debris));
			list_add(&d->list, &debrises);

			d->x = e->r.x + e->r.w / 2;
			d->y = e->r.y + e->r.h / 2;
			d->dx = (rand() % 5) - (rand() % 5);
			d->dy = -(5 + (rand() % 12));
			d->health = 60 * 2;
			d->texture = e->texture;

			d->rect.x = x;
			d->rect.y = y;
			d->rect.w = w;
			d->rect.h = h;
		}
	}
}

static void logic_explosions(void)
{
	struct list_head* pos;

	list_for_each(pos, &explosions)
	{
		Explosion* e = list_to_Explosion(pos);

		e->r.x += e->dx;
		e->r.y += e->dy;

		--e->c.a;
		if (--e->c.a <= 0) {
			pos = list_del_update_pos(pos);
			free(e);
		}
	}
}

static void logic_debris(void)
{
	struct list_head* pos;

	list_for_each(pos, &debrises)
	{
		Debris* d = list_to_Debris(pos);

		d->x += d->dx;
		d->y += d->dy;

		d->dy += 1;

		if (--d->health <= 0) {
			pos = list_del_update_pos(pos);
			free(d);
		}
	}
}

static void gen_explosion(int x, int y, int num)
{
	Explosion* e;
	int i;

	for (i = 0; i < num; i++)
	{
		e = malloc(sizeof(Explosion));
		memset(e, 0, sizeof(Explosion));
		list_add(&e->list, &explosions);

		e->r.x = x + (rand() % 32) - (rand() % 32);
		e->r.y = y + (rand() % 32) - (rand() % 32);
		e->dx = (rand() % 10) - (rand() % 10);
		e->dy = (rand() % 10) - (rand() % 10);

		e->dx /= 10;
		e->dy /= 10;

		switch (rand() % 4) {
		case 0:
			e->c.r = 255;
			break;

		case 1:
			e->c.r = 255;
			e->c.g = 128;
			break;

		case 2:
			e->c.r = 255;
			e->c.g = 255;
			break;

		default:
			e->c.r = 255;
			e->c.g = 255;
			e->c.b = 255;
			break;
		}

		e->c.a = rand() % 60 * 3;
	}
}


static void draw_explosions(void)
{
	struct list_head* pos;

	SDL_SetRenderDrawBlendMode(render, SDL_BLENDMODE_ADD);
	SDL_SetTextureBlendMode(explosionTexture, SDL_BLENDMODE_ADD);

	list_for_each(pos, &explosions)
	{
		Explosion* e = list_to_Explosion(pos);
		SDL_SetTextureColorMod(explosionTexture, e->c.r, e->c.g, e->c.b);
		SDL_SetTextureAlphaMod(explosionTexture, e->c.a);

		blit(explosionTexture, e->r.x, e->r.y);
	}

	SDL_SetRenderDrawBlendMode(render, SDL_BLENDMODE_NONE);
}

static void draw_debris(void)
{
	struct list_head* pos;

	list_for_each(pos, &debrises)
	{
		Debris* d = list_to_Debris(pos);
		blitRect(d->texture, &d->rect, d->x, d->y);
	}
}

void init_explosion_texture(void)
{
	explosionTexture = loadTexture("resource/explosion.png");
	if (!explosionTexture)
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Couldn't load explosion texture: %s\n", SDL_GetError());
}

void gen_destroy_effect(Enemy* e)
{
	gen_explosion(e->r.x, e->r.y, 30);
	gen_debris(e);
}

void logic_destroy_effect(void)
{
	logic_explosions();
	logic_debris();
}
void draw_destroy_effect(void)
{
	draw_explosions();
	draw_debris();
}