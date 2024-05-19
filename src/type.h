#pragma once
#include "list.h"
#include "SDL2/SDL.h"

/* 颜色的描述类型 */
struct _color {
    int r, g, b, a;
};

enum {
    BULLET_TYPE_NORMAL = 0,
    BULLET_TYPE_MISSILE = 1,
    BULLET_TYPE_BOMB = 2,
};

/* 基础子弹的描述类型 */
struct _bullet {
    SDL_Rect r;

    int dx;
    int dy;

    float fdx;
    float fdy;
    float fdx_sum;
    float fdy_sum;

    int type;
    int health;
    int speed;
    struct list_head list;

    SDL_Point target;
    SDL_Texture* texture;
};

typedef struct _bullet Bullet;
list_to_(Bullet);

/* 基础飞船的描述类型 */
struct _ship {
    SDL_Rect r;
    SDL_Rect shield_r;

    int dx;
    int dy;
    int health;
    int speed;
    int bomb;
    int bomb_reload;
    int max_bomb_count;
    int bullet_level;
    int bullet_reload;
    int missile_level;
    int missile_reload;
    int shield_health;
    int id_sound_die;
    struct list_head list;

    SDL_Point center;
    /* bullets原先的设计思路是: 飞船"装载"子弹.
     * 但敌机被摧毁后子弹链表也丢了, 不仅不合理, 还内存泄露.
     * 所以把敌机子弹放在专门的管理链表bullets::enemies_bullets中.
     */
    // struct list_head bullets;

    SDL_Texture* texture;
    SDL_Texture* shield;
};

typedef struct _ship Player;
list_to_(Player);

typedef struct _ship Enemy;
list_to_(Enemy);

/* 爆炸特效的描述类型 */
struct _explosion {
    SDL_Rect r;

    int dx;
    int dy;
    struct _color c;

    struct list_head list;
    SDL_Texture* texture;
};

typedef struct _explosion Explosion;
list_to_(Explosion);

/* 爆炸碎片的描述类型 */
struct _debris {
    int x;
    int y;
    int dx;
    int dy;
    SDL_Rect rect;
    SDL_Texture* texture;
    int health;
    struct list_head list;
};

typedef struct _debris Debris;
list_to_(Debris);

/* 击毁敌人概率产生道具点 */
struct _point {
    SDL_Rect r;

    int type;
    int speed;
    int health;
    int dx;
    int dy;
    struct list_head list;
    SDL_Texture* texture;
};

typedef struct _point Point;
list_to_(Point);

/* 一个背景效果, 星海
 * 背景特效属于theme, effect是爆炸特效 */
struct _star {
    int x;
    int y;
    int speed;
};

typedef struct _star Star;
