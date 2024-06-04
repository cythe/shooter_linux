#pragma once
#include "type.h"

extern struct list_head stage1_enemies;
extern int current_frame;

enum {
    STAGE_1,
};

extern int score;
extern int highscore;
extern SDL_Window* window;
extern SDL_Renderer* render;

int init_stage(void);
int reset_stage(void);

int get_screen_width(void);
int get_screen_height(void);
Player* get_player(void);
