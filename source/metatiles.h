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

    /*
        HOUSE
    */

    META_ROOF_LEFT,
    META_ROOF_MIDDLE,
    META_ROOF_RIGHT,

    META_ROOF_EAVE_LEFT,
    META_ROOF_EAVE_MIDDLE,
    META_ROOF_EAVE_RIGHT,

    META_WALL,
    META_WALL_DETAIL,

    META_WINDOW,

    META_DOOR_TOP,
    META_DOOR_BOTTOM,

    META_SIGN,
    META_FLOWER_BOX,

    /*
        NATURE
    */

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
