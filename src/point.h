#pragma once
#include "type.h"

enum {
    POINT_TYPE_P = 0,   // Power
    POINT_TYPE_M = 1,   // Missile
    POINT_TYPE_S = 2,   // Shield
    POINT_TYPE_B = 3,   // Bomb
    POINT_TYPE_U = 4,   // 1 Up
    POINT_TYPE_MAX,
};

extern struct list_head points;

void init_point_texture(void);

/* ----------------------------------------------------------------------*/
/**
 * @brief : 生成一个奖励点
 *
 * @param : x
 * @param : y
 */
/* ----------------------------------------------------------------------*/
void gen_point_pod(int x, int y);

void logic_points(void);
void draw_point_pods(void);
