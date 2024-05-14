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

void accept_event(void)
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type) {
		case SDL_QUIT:
			exit(0);
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