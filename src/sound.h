#pragma once
#define MAX_SND_CHANNELS 8

/* ----------------------------------------------------------------------*/
/**
 * @brief : 声音通道定义
 */
/* ----------------------------------------------------------------------*/
enum {
    CH_ANY = -1,
    CH_PLAYER,
    CH_ALIEN_FIRE,
    CH_POINTS,
};

/* ----------------------------------------------------------------------*/
/**
 * @brief : 音乐ID定义
 */
/* ----------------------------------------------------------------------*/
enum {
    SND_PLAYER_FIRE,
    SND_ALIEN_FIRE,
    SND_PLAYER_DIE,
    SND_SHIELD_SHATTER,
    SND_ALIEN_DIE,
    SND_POINTS,
    SND_MAX,
};

void play_bgm(int loop);

/* ----------------------------------------------------------------------*/
/**
 * @brief : 播放音效
 *
 * @param : id
 * @param : channel
 */
/* ----------------------------------------------------------------------*/
void play_sound(int id, int channel);
void init_sounds(void);
