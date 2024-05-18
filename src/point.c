#include <stdlib.h>
#include <string.h>
#include "type.h"
#include "player.h"
#include "stage.h"
#include "theme.h"
#include "sound.h"
#include "tools.h"
#include "point.h"

LIST_HEAD(points);
SDL_Texture* pointsTexture[POINT_TYPE_MAX];

static void __gen_point_pod(int x, int y, int t)
{
    Point* p;

    p = malloc(sizeof(Point));
    memset(p, 0, sizeof(Point));
    list_add(&p->list, &points);

    p->r.x = x;
    p->r.y = y;
    p->dx = -(rand() % 5);
    p->dy = (rand() % 5) - (rand() % 5);
    p->health = 60 * 10;
    p->type = t;
    p->texture = pointsTexture[t];

    SDL_QueryTexture(p->texture, NULL, NULL, &p->r.w, &p->r.h);

    p->r.x -= p->r.w / 2;
    p->r.y -= p->r.h / 2;
}

void gen_point_pod(int x, int y)
{
    int r;
    int t;

    r = rand() % 100;
    if (r < 80) {   // 80%的概率生成奖励点
	if ( r / 10 == 1 || r / 10 == 2 )
	    t = POINT_TYPE_P;
	else if ( r / 10 == 3 || r / 10 == 4 )
	    t = POINT_TYPE_M;
	else if ( r / 10 == 5 )
	    t = POINT_TYPE_S;
	else if ( r / 10 == 6 )
	    t = POINT_TYPE_B;
	else if ( r / 10 == 7 )
	    t = POINT_TYPE_U;
	else
	    t = POINT_TYPE_P;

	__gen_point_pod(x, y, t);
    }
}

void init_point_texture(void)
{
    pointsTexture[POINT_TYPE_P] = loadTexture("resource/point_p.png");
    if (!pointsTexture[POINT_TYPE_P])
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
		"Couldn't load point power texture: %s\n", SDL_GetError());

    pointsTexture[POINT_TYPE_M] = loadTexture("resource/point_m.png");
    if (!pointsTexture[POINT_TYPE_M])
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
		"Couldn't load point missile texture: %s\n", SDL_GetError());

    pointsTexture[POINT_TYPE_S] = loadTexture("resource/point_s.png");
    if (!pointsTexture[POINT_TYPE_S])
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
		"Couldn't load point shield texture: %s\n", SDL_GetError());

    pointsTexture[POINT_TYPE_B] = loadTexture("resource/point_b.png");
    if (!pointsTexture[POINT_TYPE_B])
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
		"Couldn't load point bomb texture: %s\n", SDL_GetError());

    pointsTexture[POINT_TYPE_U] = loadTexture("resource/point_u.png");
    if (!pointsTexture[POINT_TYPE_U])
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
		"Couldn't load point 1up texture: %s\n", SDL_GetError());
}

void logic_points(void)
{
    struct list_head* pos;
    Point* p;
    Player* player = get_player();

    list_for_each(pos, &points)
    {
	p = list_to_Point(pos);

	if (p->r.x < 0) {
	    p->r.x = 0;
	    p->dx = -p->dx;
	}

	if (p->r.x + p->r.w > SCREEN_WIDTH) {
	    p->r.x = SCREEN_WIDTH - p->r.w;
	    p->dx = -p->dx;
	}

	if (p->r.y < 0) {
	    p->r.y = 0;
	    p->dy = -p->dy;
	}

	if (p->r.y + p->r.h > SCREEN_HEIGHT) {
	    p->r.y = SCREEN_HEIGHT - p->r.h;
	    p->dy = -p->dy;
	}

	p->r.x += p->dx;
	p->r.y += p->dy;

	if (player != NULL && collision(&p->r, &player->r)) {
	    p->health = 0;

	    if(p->type == POINT_TYPE_P && g_player.bullet_level < PLAYER_BULLET_LEVEL_MAX) {
		g_player.bullet_level++;
	    }

	    if(p->type == POINT_TYPE_M && g_player.missile_level < PLAYER_MISSILE_LEVEL_MAX) {
		g_player.missile_level++;
	    }

	    if(p->type == POINT_TYPE_B && g_player.bomb < PLAYER_BOMB_MAX_BOMB_CNT) {
		g_player.bomb++;
	    }

	    if(p->type == POINT_TYPE_U && g_player.health < PLAYER_MAX_LIFE) {
		g_player.health++;
	    }

	    highscore = MAX(score, highscore);

	    play_sound(SND_POINTS, CH_POINTS);
	}

	if (--p->health <= 0) {
	    pos = list_del_update_pos(pos);
	    free(p);
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
#if DEBUG
	draw_rect(&d->r, YELLOW);
#endif
    }
}
