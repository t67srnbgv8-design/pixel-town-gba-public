#ifndef PLAYER_H
#define PLAYER_H

#include <gba.h>

#define DIR_DOWN  0
#define DIR_UP    1
#define DIR_LEFT  2
#define DIR_RIGHT 3

typedef struct
{
    int x;
    int y;
    int direction;
    int moving;
    int frame;
    int timer;
} Player;

extern Player player;

void playerInit(void);
void playerUpdate(void);
void playerDraw(int cameraX, int cameraY);

#endif
