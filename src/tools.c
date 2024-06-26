#include "type.h"
#include "SDL2/SDL_image.h"
#include "stage.h"
#include "theme.h"
#include "tools.h"

void get_center(SDL_Rect* r, SDL_Point* p)
{
    p->x = r->x +r->w/2;
    p->y = r->y +r->h/2;
}

/* 旋转矩形, 暂定用于导弹追踪 */
void spin_rect(SDL_Rect * r, SDL_Texture *t, int angle)
{
    // 设置源和目标矩形
    SDL_Rect src = { 0, 0, r->w, r->h };
    SDL_Rect dest = { r->x, r->y, r->w, r->h };

    // 设置旋转中心
    SDL_Point center = { r->w/2, r->h/2 }; // 旋转中心是图像的中心

    // 使用 SDL_RenderCopyEx 渲染纹理
    SDL_RenderCopyEx(render, t, &src, &dest, (double)angle, &center, SDL_FLIP_NONE);
#if DEBUG
    draw_rect(&dest, 0xffaa0000);
#endif
}

/* 计算敌方子弹的攻击方向 */
void calc_slope(int x1, int y1, int x2, int y2, float* dx, float* dy)
{
    int steps = MAX(abs(x1 - x2), abs(y1 - y2));

    if (steps == 0) {
	*dx = *dy = 0;
	return;
    }

    *dx = (x1 - x2);
    *dx /= steps;

    *dy = (y1 - y2);
    *dy /= steps;
}

/* 两个矩形的碰撞检测函数 */
int collision(SDL_Rect* r1, SDL_Rect* r2)
{
    return (MAX(r1->x, r2->x) < MIN(r1->x + r1->w, r2->x + r2->w)) &&
	(MAX(r1->y, r2->y) < MIN(r1->y + r1->h, r2->y + r2->h));
}

/* 判断飞船是不是飞了 */
int out_of_screen(SDL_Rect* r)
{
    return r->x < 0 || r->x > SCREEN_WIDTH || r->y < 0 || r->y > SCREEN_HEIGHT;
}

/* 绘制任意形状 */
void blit(SDL_Texture* texture, int x, int y)
{
    SDL_Rect dest;

    dest.x = x;
    dest.y = y;
    SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);

    SDL_RenderCopy(render, texture, NULL, &dest);
}

/* 绘制一个矩形框, 用来debug */
void draw_rect(SDL_Rect* rect, int color)
{
    int r, g, b, a;

    a = (color >> 24) & 0xff;
    r = (color >> 16) & 0xff;
    g = (color >> 8) & 0xff;
    b = color & 0xff;

    SDL_SetRenderDrawColor(render, r, g, b, a);

    SDL_RenderDrawRect(render, rect);
}

/* 绘制任意矩形范围 */
void blitRect(SDL_Texture* texture, SDL_Rect* src, int x, int y)
{
    SDL_Rect dest;

    dest.x = x;
    dest.y = y;
    dest.w = src->w;
    dest.h = src->h;

    SDL_RenderCopy(render, texture, src, &dest);
}

/* X方向限制飞船位置 */
void clip_rect_x(SDL_Rect *r)
{
    if (r->x < 0) {
	r->x = 0;
    }

    if (r->x > get_screen_width() - r->w) {
	r->x = get_screen_width() - r->w;
    }
}

/* Y方向限制飞船位置 */
void clip_rect_y(SDL_Rect *r)
{
    if (r->y < 0) {
	r->y = 0;
    }

    if (r->y > get_screen_height() - r->h) {
	r->y = get_screen_height() - r->h;
    }
}

void clip_rect(SDL_Rect *r)
{
    clip_rect_x(r);
    clip_rect_y(r);
}

/* 屏幕范围内限制飞船位置 */
void clip_ship(struct _ship* s)
{
    clip_rect(&s->r);
}

/* 载入贴图 */
SDL_Texture* loadTexture(char* filename)
{
    SDL_Texture* texture;

    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO,
	    "Loading %s", filename);

    texture = IMG_LoadTexture(render, filename);
    if (!texture) {
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
		"%s", SDL_GetError());
	return NULL;
    }

    return texture;
}

/* 圆形轨迹计算
 *       * 
 *     *   *
 *   *   *   * 0(360)
 *     *   * 
 *       *
 *       90
 * enter为中心点坐标, r是半径, d是角度, p是结果.
 */
void calculate_circle_point(SDL_Point enter, int r, int d, SDL_Point* p)
{
    p->x = enter.x + r * cos(d * PI / 180);
    p->y = enter.y + r * sin(d * PI / 180);
}

void calculate_circle_speed(int s, int d, float* dx, float* dy)
{
    *dx = (float)s * (float)cos(d * PI / 180);
    *dy = (float)s * (float)sin(d * PI / 180);
}

void draw_circle(int x, int y, int radius, int color)
{
    // 初始化参数
    int x0 = 0, y0 = radius;
    int d = 3 - 2 * radius;

    int r, g, b, a;

    a = (color >> 24) & 0xff;
    r = (color >> 16) & 0xff;
    g = (color >> 8) & 0xff;
    b = color & 0xff;

    //SDL_SetRenderDrawBlendMode(render, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(render, r, g, b, a);

    while (y0 >= x0) {
	SDL_RenderDrawPoint(render, x + x0, y + y0);
	SDL_RenderDrawPoint(render, x - x0, y + y0);
	SDL_RenderDrawPoint(render, x + x0, y - y0);
	SDL_RenderDrawPoint(render, x - x0, y - y0);
	SDL_RenderDrawPoint(render, x + y0, y + x0);
	SDL_RenderDrawPoint(render, x - y0, y + x0);
	SDL_RenderDrawPoint(render, x + y0, y - x0);
	SDL_RenderDrawPoint(render, x - y0, y - x0);

	if (d <= 0) {
	    d += 4 * x0 + 6;
	} else {
	    d += 4 * (x0 - y0) + 10;
	    y0--;
	}
	x0++;
    }
    //SDL_SetRenderDrawBlendMode(render, SDL_BLENDMODE_NONE);
}
