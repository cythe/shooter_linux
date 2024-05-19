#include "type.h"
#include <string.h>
#include "SDL2/SDL_mixer.h"
#include "sound.h"

Mix_Chunk* sounds[SND_MAX] = { 0 };
Mix_Music* music = NULL;

void load_music(char* filename)
{
    if (music != NULL) {
	Mix_HaltMusic();
	Mix_FreeMusic(music);
	music = NULL;
    }

    music = Mix_LoadMUS(filename);
}

void play_bgm(int loop)
{
    Mix_PlayMusic(music, (loop) ? -1 : 0);
}

void play_sound(int id, int channel)
{
    Mix_PlayChannel(channel, sounds[id], 0);
}

static void load_sounds(void)
{
    sounds[SND_PLAYER_FIRE] = Mix_LoadWAV("resource/334227__jradcoolness__laser.ogg");
    sounds[SND_ALIEN_FIRE] = Mix_LoadWAV("resource/196914__dpoggioli__laser-gun.ogg");
    sounds[SND_PLAYER_DIE] = Mix_LoadWAV("resource/245372__quaker540__hq-explosion.ogg");
    sounds[SND_SHIELD_SHATTER] = Mix_LoadWAV("resource/shield_shatter.ogg");
    sounds[SND_ALIEN_DIE] = Mix_LoadWAV("resource/10 Guage Shotgun-SoundBible.com-74120584.ogg");
    sounds[SND_POINTS] = Mix_LoadWAV("resource/342749__rhodesmas__notification-01.ogg");
}

void init_sounds(void)
{
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
		"Couldn't initialize SDL Mixer\n");
	exit(1);
    }

    Mix_AllocateChannels(MAX_SND_CHANNELS);

    memset(sounds, 0, sizeof(Mix_Chunk*) * SND_MAX);

    music = NULL;

    load_sounds();

    load_music("resource/Mercury.ogg");

    play_bgm(1);
}
