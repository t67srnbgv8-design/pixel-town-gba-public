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
    META_GRASS_A,
    META_GRASS_B,

    META_PATH,
    META_PATH_TOP,
    META_PATH_BOTTOM,
    META_PATH_LEFT,
    META_PATH_RIGHT,

    META_ROOF_TL,
    META_ROOF_T,
    META_ROOF_TR,
    META_ROOF_L,
    META_ROOF_M,
    META_ROOF_R,
    META_ROOF_BL,
    META_ROOF_B,
    META_ROOF_BR,

    META_WALL,
    META_WINDOW,
    META_DOOR,

    META_TREE_TOP_L,
    META_TREE_TOP_R,
    META_TREE_BOTTOM_L,
    META_TREE_BOTTOM_R,

    META_BUSH,
    META_FLOWER,

    META_FENCE_H,
    META_FENCE_V,

    META_WATER,
    META_WATER_TOP,
    META_WATER_BOTTOM,
    META_WATER_LEFT,
    META_WATER_RIGHT,

    META_COUNT
};

enum
{
    BEHAVIOR_NORMAL = 0,
    BEHAVIOR_WATER,
    BEHAVIOR_DOOR,
    BEHAVIOR_GRASS
};

extern Metatile metatiles[META_COUNT];

void metatilesInit(void);

#endif
