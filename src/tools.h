#pragma once
#include "type.h"

#define PI (3.1415926)

#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define MIN(x,y) ((x) > (y) ? (y) : (x))

#define DEBUG 1
enum {
    RED = 0,
    GREEN = 1,
    BLUE = 2,
    YELLOW = 3,
};

void get_center(SDL_Rect* r, SDL_Point* p);
/* 旋转这个玩意儿 */
void spin_rect(SDL_Rect * r, SDL_Texture *t, int angle);

/* 计算斜率并生成dx dy */
void calc_slope(int x1, int y1, int x2, int y2, float* dx, float* dy);

/* 两个矩形的碰撞检测函数 */
int collision(SDL_Rect* r1, SDL_Rect* r2);

/* 判断飞船是不是飞了 */
int out_of_screen(SDL_Rect* r);

/* 绘制一个矩形框, 用来debug */
void draw_rect(SDL_Rect* r, int color);

/* 绘制任意形状 */
void blit(SDL_Texture* texture, int x, int y);

/* 绘制任意矩形范围 */
void blitRect(SDL_Texture* texture, SDL_Rect* src, int x, int y);

/* X方向限制飞船位置 */
void clip_ship_x(struct _ship* s);

/* Y方向限制飞船位置 */
void clip_ship_y(struct _ship* s);

/* 屏幕范围内限制飞船位置 */
void clip_ship(struct _ship* s);

/* 载入贴图 */
SDL_Texture* loadTexture(char* filename);

void calculate_circle_point(SDL_Point enter, int r, int d, SDL_Point* p);
void calculate_circle_speed(int s, int d, float *dx, float *dy);
