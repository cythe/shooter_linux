#include <stdlib.h>
#include <string.h>
#include "type.h"
#include "player.h"
#include "stage.h"
#include "theme.h"
#include "sound.h"
#include "tools.h"

LIST_HEAD(points);
SDL_Texture* pointsTexture;

void gen_point_pod(int x, int y)
{
    Point* e;

    e = malloc(sizeof(Point));
    memset(e, 0, sizeof(Point));
    list_add(&e->list, &points);

    e->r.x = x;
    e->r.y = y;
    e->dx = -(rand() % 5);
    e->dy = (rand() % 5) - (rand() % 5);
    e->health = 60 * 10;
    e->texture = pointsTexture;

    SDL_QueryTexture(e->texture, NULL, NULL, &e->r.w, &e->r.h);

    e->r.x -= e->r.w / 2;
    e->r.y -= e->r.h / 2;
}

void init_point_texture(void)
{
    pointsTexture = loadTexture("resource/points.png");
    if (!pointsTexture)
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
		"Couldn't load point texture: %s\n", SDL_GetError());
}

void logic_points(void)
{
    struct list_head* pos;
    Point* e;
    Player* player = get_player();

    list_for_each(pos, &points)
    {
	e = list_to_Point(pos);

	if (e->r.x < 0) {
	    e->r.x = 0;
	    e->dx = -e->dx;
	}

	if (e->r.x + e->r.w > SCREEN_WIDTH) {
	    e->r.x = SCREEN_WIDTH - e->r.w;
	    e->dx = -e->dx;
	}

	if (e->r.y < 0) {
	    e->r.y = 0;
	    e->dy = -e->dy;
	}

	if (e->r.y + e->r.h > SCREEN_HEIGHT) {
	    e->r.y = SCREEN_HEIGHT - e->r.h;
	    e->dy = -e->dy;
	}

	e->r.x += e->dx;
	e->r.y += e->dy;

	if (player != NULL && collision(&e->r, &player->r)) {
	    e->health = 0;

	    g_player.bullet_level++;

	    highscore = MAX(score, highscore);

	    play_sound(SND_POINTS, CH_POINTS);
	}

	if (--e->health <= 0) {
	    pos = list_del_update_pos(pos);
	    free(e);
	}
    }
}

void draw_point_pods(void)
{
    struct list_head* pos;

    list_for_each(pos, &points)
    {
	Point* d = list_to_Point(pos);
	blit(d->texture, d->r.x, d->r.y);
    }
}
