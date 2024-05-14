#include "event.h"

int keyboard[MAX_KEYBOARD_KEYS] = { 0 };

void doKeyUp(SDL_KeyboardEvent* event)
{
    if (event->repeat == 0 && event->keysym.scancode < MAX_KEYBOARD_KEYS) {
	keyboard[event->keysym.scancode] = 0;
    }
}

void doKeyDown(SDL_KeyboardEvent* event)
{
    if (event->repeat == 0 && event->keysym.scancode < MAX_KEYBOARD_KEYS) {
	keyboard[event->keysym.scancode] = 1;
    }
}

int g_game_exit = 0;
void accept_event(void)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
	switch (event.type) {
	    case SDL_QUIT:
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
			"get exit signal: %s\n", SDL_GetError());
		g_game_exit = 1;
		break;

	    case SDL_KEYDOWN:
		doKeyDown(&event.key);
		break;

	    case SDL_KEYUP:
		doKeyUp(&event.key);
		break;

	    default:
		break;
	}
    }
}

int get_keyboard(int k)
{
    return keyboard[k];
}
