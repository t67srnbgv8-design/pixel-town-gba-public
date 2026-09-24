#include <gba.h>
#include "player.h"
#include "world.h"

Player player;

static void objPixel(
    u16 *base,
    int x,
    int y,
    u8 colour
)
{
    if (x < 0 || x >= 16 || y < 0 || y >= 32)
        return;

    int tileX = x >> 3;
    int tileY = y >> 3;

    int localX = x & 7;
    int localY = y & 7;

    int tile = tileY * 2 + tileX;

    int p = localY * 8 + localX;
    int word = p >> 2;
    int shift = (p & 3) * 4;

    u16 *ptr = &base[tile * 16];

    ptr[word] &= ~(0xF << shift);
    ptr[word] |= (colour & 0xF) << shift;
}

static void clearFrame(u16 *base)
{
    for (int i = 0; i < 128; i++)
        base[i] = 0;
}

static void makeFrame(
    u16 *base,
    int direction,
    int frame
)
{
    clearFrame(base);

    const int outline = 1;
    const int hair    = 2;
    const int skin    = 3;
    const int shirt   = 4;
    const int pants   = 5;
    const int shoes   = 6;
    const int light   = 7;
    const int shadow  = 8;

    /* ground shadow */
    for (int x = 4; x <= 11; x++)
        objPixel(base,x,29,shadow);

    /* head */
    for (int y = 7; y <= 12; y++)
        for (int x = 5; x <= 10; x++)
            objPixel(base,x,y,skin);

    /* hair */
    for (int x = 5; x <= 10; x++) {
        objPixel(base,x,6,hair);
        objPixel(base,x,7,hair);
    }

    objPixel(base,4,8,hair);
    objPixel(base,11,8,hair);

    /* facing */
    if (direction == DIR_DOWN) {
        objPixel(base,6,10,outline);
        objPixel(base,9,10,outline);
    }
    else if (direction == DIR_UP) {
        for (int x = 5; x <= 10; x++)
            objPixel(base,x,10,hair);
    }
    else if (direction == DIR_LEFT) {
        objPixel(base,5,10,outline);
    }
    else {
        objPixel(base,10,10,outline);
    }

    /* body */
    for (int y = 14; y <= 20; y++)
        for (int x = 5; x <= 10; x++)
            objPixel(base,x,y,shirt);

    objPixel(base,6,15,light);
    objPixel(base,6,16,light);

    /* arms */
    int leftOffset  = frame ? 1 : 0;
    int rightOffset = frame ? 0 : 1;

    for (int y = 15; y <= 20; y++) {
        objPixel(base,3,y + leftOffset,skin);
        objPixel(base,4,y + leftOffset,skin);

        objPixel(base,11,y + rightOffset,skin);
        objPixel(base,12,y + rightOffset,skin);
    }

    /* trousers */
    for (int y = 21; y <= 23; y++)
        for (int x = 5; x <= 10; x++)
            objPixel(base,x,y,pants);

    if (frame == 0) {
        for (int y = 24; y <= 27; y++) {
            objPixel(base,5,y,pants);
            objPixel(base,6,y,pants);

            objPixel(base,9,y,pants);
            objPixel(base,10,y,pants);
        }

        for (int x = 4; x <= 6; x++)
            objPixel(base,x,28,shoes);

        for (int x = 9; x <= 11; x++)
            objPixel(base,x,28,shoes);
    }
    else {
        for (int y = 24; y <= 27; y++) {
            objPixel(base,4,y,pants);
            objPixel(base,5,y,pants);

            objPixel(base,10,y,pants);
            objPixel(base,11,y,pants);
        }

        for (int x = 3; x <= 5; x++)
            objPixel(base,x,28,shoes);

        for (int x = 10; x <= 12; x++)
            objPixel(base,x,28,shoes);
    }
}

static int blocked(int x, int y)
{
    /*
       Only the feet collide with the map.
       This allows the body/head to overlap foreground.
    */

    int left   = x + 4;
    int right  = x + 11;
    int top    = y + 24;
    int bottom = y + 28;

    if (worldIsBlocked(left,top))
        return 1;

    if (worldIsBlocked(right,top))
        return 1;

    if (worldIsBlocked(left,bottom))
        return 1;

    if (worldIsBlocked(right,bottom))
        return 1;

    return 0;
}

static int clampValue(
    int value,
    int minimum,
    int maximum
)
{
    if (value < minimum)
        return minimum;

    if (value > maximum)
        return maximum;

    return value;
}

void playerInit(void)
{
    player.x = 244;
    player.y = 92;

    player.direction = DIR_DOWN;
    player.moving = 0;
    player.frame = 0;
    player.timer = 0;

    /*
       Sprite palette
    */
    SPRITE_PALETTE[0] = RGB5(31,0,31);
    SPRITE_PALETTE[1] = RGB5(3,3,4);
    SPRITE_PALETTE[2] = RGB5(9,5,3);
    SPRITE_PALETTE[3] = RGB5(30,22,16);
    SPRITE_PALETTE[4] = RGB5(5,12,28);
    SPRITE_PALETTE[5] = RGB5(5,7,14);
    SPRITE_PALETTE[6] = RGB5(3,3,4);
    SPRITE_PALETTE[7] = RGB5(18,22,31);
    SPRITE_PALETTE[8] = RGB5(4,12,4);

    makeFrame(
        (u16 *)SPRITE_GFX,
        player.direction,
        0
    );

    for (int i = 1; i < 128; i++) {
        OAM[i].attr0 = ATTR0_DISABLED;
        OAM[i].attr1 = 0;
        OAM[i].attr2 = 0;
    }
}

void playerUpdate(void)
{
    scanKeys();

    u16 keys = keysHeld();

    int nx = player.x;
    int ny = player.y;

    player.moving = 0;

    if (keys & KEY_LEFT) {
        nx--;
        player.direction = DIR_LEFT;
        player.moving = 1;
    }
    else if (keys & KEY_RIGHT) {
        nx++;
        player.direction = DIR_RIGHT;
        player.moving = 1;
    }
    else if (keys & KEY_UP) {
        ny--;
        player.direction = DIR_UP;
        player.moving = 1;
    }
    else if (keys & KEY_DOWN) {
        ny++;
        player.direction = DIR_DOWN;
        player.moving = 1;
    }

    nx = clampValue(
        nx,
        0,
        WORLD_W - 16
    );

    ny = clampValue(
        ny,
        0,
        WORLD_H - 32
    );

    /*
       X and Y checked separately so movement doesn't
       get stuck as easily against corners.
    */

    if (!blocked(nx,player.y))
        player.x = nx;

    if (!blocked(player.x,ny))
        player.y = ny;

    if (player.moving) {
        player.timer++;

        if (player.timer >= 10) {
            player.timer = 0;
            player.frame ^= 1;
        }
    }
    else {
        player.timer = 0;
        player.frame = 0;
    }

    makeFrame(
        (u16 *)SPRITE_GFX,
        player.direction,
        player.moving ? player.frame : 0
    );
}

void playerDraw(
    int cameraX,
    int cameraY
)
{
    int screenX =
        player.x - cameraX;

    int screenY =
        player.y - cameraY;

    OAM[0].attr0 =
        ATTR0_COLOR_16 |
        ATTR0_TALL |
        (screenY & 0xFF);

    OAM[0].attr1 =
        ATTR1_SIZE_32 |
        (screenX & 0x1FF);

    /*
       BG1 priority 0
       Player priority 1
       BG0 priority 2

       Therefore:
       foreground > player > ground
    */
    OAM[0].attr2 =
        ATTR2_PRIORITY(1);
}
