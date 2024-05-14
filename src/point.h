#pragma once
#include "type.h"

extern struct list_head points;

void init_point_texture(void);

void gen_point_pod(int x, int y);
void logic_points(void);
void draw_point_pods(void);