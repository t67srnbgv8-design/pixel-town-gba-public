#ifndef PIXELTOWN_WORLD_H
#define PIXELTOWN_WORLD_H

#include <gba.h>

#define META_MAP_W 32
#define META_MAP_H 16

#define WORLD_W (META_MAP_W * 16)
#define WORLD_H (META_MAP_H * 16)

extern u16 worldMap[META_MAP_H][META_MAP_W];

void worldInit(void);
void worldDraw(void);

int worldIsBlocked(
    int pixelX,
    int pixelY
);

#endif
