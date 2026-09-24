#ifndef PIXELTOWN_METATILES_H
#define PIXELTOWN_METATILES_H

#include <gba.h>

typedef struct
{
    u16 bottom[4];
    u16 top[4];

    u8 collision;
    u8 behavior;
    u8 elevation;
} Metatile;

enum
{
    META_GRASS = 0,
    META_GRASS_DETAIL,
    META_FLOWER,

    META_PATH,
    META_PATH_V,
    META_PATH_TOP,
    META_PATH_BOTTOM,

    META_ROOF_L,
    META_ROOF_M,
    META_ROOF_R,

    META_WALL,
    META_WINDOW_TOP,
    META_WINDOW_BOTTOM,

    META_DOOR_TOP,
    META_DOOR_BOTTOM,

    META_TREE_TL,
    META_TREE_TR,
    META_TREE_BL,
    META_TREE_BR,

    META_BUSH,
    META_WATER,

    META_COUNT
};

enum
{
    BEHAVIOR_NORMAL = 0,
    BEHAVIOR_WATER,
    BEHAVIOR_DOOR
};

extern Metatile metatiles[META_COUNT];

void metatilesInit(void);

#endif
