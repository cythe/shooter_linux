#pragma once
#include "SDL2/SDL.h"

#define MAX_KEYBOARD_KEYS 250

extern int g_game_exit;

extern int keyboard[MAX_KEYBOARD_KEYS];

void accept_event(void);
int get_keyboard(int k);
