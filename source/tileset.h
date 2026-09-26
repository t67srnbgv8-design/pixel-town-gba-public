#ifndef PIXELTOWN_TILESET_H
#define PIXELTOWN_TILESET_H

#include <gba.h>

enum
{
    TILE_EMPTY = 0,

    TILE_GRASS,
    TILE_GRASS_DETAIL,
    TILE_FLOWER,

    TILE_PATH,
    TILE_PATH_EDGE_L,
    TILE_PATH_EDGE_R,
    TILE_PATH_EDGE_T,
    TILE_PATH_EDGE_B,

    /* roof */
    TILE_ROOF_SLOPE_L,
    TILE_ROOF_TOP,
    TILE_ROOF_SLOPE_R,

    TILE_ROOF_RED,
    TILE_ROOF_RED_LIGHT,
    TILE_ROOF_RED_DARK,

    TILE_ROOF_EAVE_L,
    TILE_ROOF_EAVE_M,
    TILE_ROOF_EAVE_R,

    /* facade */
    TILE_WALL,
    TILE_WALL_DETAIL,
    TILE_WALL_BASE,

    /* windows */
    TILE_WINDOW_TL,
    TILE_WINDOW_TR,
    TILE_WINDOW_BL,
    TILE_WINDOW_BR,

    /* door */
    TILE_DOOR_TL,
    TILE_DOOR_TR,
    TILE_DOOR_BL,
    TILE_DOOR_BR,

    /* decoration */
    TILE_SIGN,
    TILE_FLOWER_BOX,

    /* trees */
    TILE_TREE_00,
    TILE_TREE_01,
    TILE_TREE_02,
    TILE_TREE_03,

    TILE_TREE_10,
    TILE_TREE_11,
    TILE_TREE_12,
    TILE_TREE_13,

    TILE_TREE_20,
    TILE_TREE_21,
    TILE_TREE_22,
    TILE_TREE_23,

    TILE_TREE_30,
    TILE_TREE_31,
    TILE_TREE_32,
    TILE_TREE_33,

    TILE_BUSH,
    TILE_WATER,

    TILE_COUNT
};

void tilesetInit(void);

#endif
