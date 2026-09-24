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
    META_PATH_EDGE,

    META_ROOF_LEFT,
    META_ROOF_MIDDLE,
    META_ROOF_RIGHT,

    META_ROOF_LOW_LEFT,
    META_ROOF_LOW_MIDDLE,
    META_ROOF_LOW_RIGHT,

    META_WALL,
    META_WINDOW,
    META_DOOR,

    META_TREE_TOP_LEFT,
    META_TREE_TOP_RIGHT,
    META_TREE_LOW_LEFT,
    META_TREE_LOW_RIGHT,

    META_BUSH,

    META_FENCE,

    META_WATER,
    META_WATER_EDGE,

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
