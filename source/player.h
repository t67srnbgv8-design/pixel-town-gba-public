#ifndef PIXELTOWN_PLAYER_H
#define PIXELTOWN_PLAYER_H

typedef struct
{
    int x;
    int y;

    int direction;
    int frame;
    int animationTimer;

} Player;

extern Player player;

void playerInit(void);
void playerUpdate(void);

void playerDraw(
    int cameraX,
    int cameraY
);

#endif
