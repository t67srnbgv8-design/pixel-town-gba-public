#ifndef PIXELTOWN_WORLD_H
#define PIXELTOWN_WORLD_H

#include <gba.h>

/*
    World size in 16x16 metatiles.

    32 x 16 metatiles
    = 512 x 256 pixels
*/

#define META_MAP_W 32
#define META_MAP_H 16

#define WORLD_W (META_MAP_W * 16)
#define WORLD_H (META_MAP_H * 16)

/*
    World metatile map.
*/

extern u16 worldMap[META_MAP_H][META_MAP_W];

/*
    Creates the town layout:
    grass, roads, houses,
    trees, bushes and water.
*/

void worldInit(void);

/*
    Converts the 16x16 metatile map
    into the GBA BG0/BG1 tilemaps.
*/

void worldDraw(void);

/*
    Collision check.

    pixelX / pixelY are world coordinates.

    Returns:
    0 = walkable
    1 = blocked
*/

int worldIsBlocked(
    int pixelX,
    int pixelY
);

#endif
