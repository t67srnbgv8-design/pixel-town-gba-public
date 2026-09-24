#ifndef WORLD_H
#define WORLD_H

#include <gba.h>

#define MAP_W 64
#define MAP_H 32

#define WORLD_W (MAP_W * 8)
#define WORLD_H (MAP_H * 8)

typedef struct
{
    u16 ground;
    u16 upper;
    u8 collision;
    u8 elevation;
    u8 behavior;
} MapTile;

enum
{
    BEHAVIOR_NORMAL = 0,
    BEHAVIOR_WATER,
    BEHAVIOR_DOOR,
    BEHAVIOR_GRASS,
    BEHAVIOR_SIGN
};

extern MapTile world[MAP_H][MAP_W];

void worldInit(void);
void worldDraw(void);

int worldIsBlocked(
    int pixelX,
    int pixelY
);

#endif
