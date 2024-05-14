#pragma once
#define MAX_SND_CHANNELS 8

enum {
	CH_ANY = -1,
	CH_PLAYER,
	CH_ALIEN_FIRE,
	CH_POINTS,
};

enum {
	SND_PLAYER_FIRE,
	SND_ALIEN_FIRE,
	SND_PLAYER_DIE,
	SND_ALIEN_DIE,
	SND_POINTS,
	SND_MAX,
};

void load_music(char* filename);
void play_bgm(int loop);
void play_sound(int id, int channel);
void init_sounds(void);