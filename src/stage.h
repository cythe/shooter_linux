#pragma once
#include "type.h"

extern int score;
extern int highscore;
extern SDL_Window* window;
extern SDL_Renderer* render;

int init_stage(void);
int reset_stage(void);

int get_screen_width(void);
int get_screen_height(void);
Player* get_player(void);