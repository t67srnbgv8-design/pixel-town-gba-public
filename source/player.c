#include <gba.h>

#include "player.h"
#include "world.h"
#include "character_select.h"

Player player;

enum
{
    DIR_DOWN = 0,
    DIR_UP,
    DIR_LEFT,
    DIR_RIGHT
};

#define OBJ_Y_MASK       0x00FF
#define OBJ_X_MASK       0x01FF
#define OBJ_HIDE         0x0200
#define OBJ_SHAPE_TALL   0x8000
#define OBJ_SIZE_16X32   0x4000
#define OBJ_PRIORITY_1   0x0400

/* =========================================================
   CORRECT GBA 4BPP OBJ TILE WRITER

   16x32 object in 1D mapping:

       tile 0  tile 1
       tile 2  tile 3
       tile 4  tile 5
       tile 6  tile 7

   Each 8x8 4bpp tile = 32 bytes = 16 u16.
   ========================================================= */

static void spritePixel(
    u16 *gfx,
    int x,
    int y,
    int color
)
{
    int tileX;
    int tileY;
    int tile;
    int localX;
    int localY;
    int pixelInTile;
    int word;
    int shift;

    if (
        x < 0 ||
        x >= 16 ||
        y < 0 ||
        y >= 32
    )
        return;

    tileX = x >> 3;
    tileY = y >> 3;

    tile =
        tileY * 2 +
        tileX;

    localX =
        x & 7;

    localY =
        y & 7;

    pixelInTile =
        localY * 8 +
        localX;

    word =
        tile * 16 +
        (pixelInTile >> 2);

    shift =
        (pixelInTile & 3) * 4;

    gfx[word] =
        (gfx[word] & ~(0xF << shift)) |
        ((color & 15) << shift);
}

static void spriteRect(
    u16 *gfx,
    int x1,
    int y1,
    int x2,
    int y2,
    int color
)
{
    int x;
    int y;

    for (y = y1; y <= y2; y++)
        for (x = x1; x <= x2; x++)
            spritePixel(gfx, x, y, color);
}

static void clearSprite(u16 *gfx)
{
    int i;

    /*
        8 OBJ tiles * 16 u16 per tile.
    */

    for (i = 0; i < 128; i++)
        gfx[i] = 0;
}

static int hairColorIndex(void)
{
    switch (characterConfig.hairColor)
    {
        case 0: return 2;
        case 1: return 3;
        case 2: return 4;
        default: return 5;
    }
}

static int clothesColorIndex(void)
{
    switch (characterConfig.clothes)
    {
        case 0: return 6;
        case 1: return 7;
        case 2: return 8;
        default: return 9;
    }
}

/* =========================================================
   ORIGINAL GBA-RPG STYLE CHARACTER
   ========================================================= */

static void makePlayerFrame(
    u16 *gfx,
    int direction,
    int frame
)
{
    int hair;
    int clothes;

    hair =
        hairColorIndex();

    clothes =
        clothesColorIndex();

    clearSprite(gfx);

    /* shadow */
    spriteRect(gfx, 4, 29, 11, 30, 1);

    /* legs */
    if (frame == 0)
    {
        spriteRect(gfx, 5, 22, 7, 28, 10);
        spriteRect(gfx, 8, 22, 10, 28, 10);

        spriteRect(gfx, 4, 27, 7, 29, 1);
        spriteRect(gfx, 8, 27, 11, 29, 1);
    }
    else
    {
        spriteRect(gfx, 4, 22, 6, 28, 10);
        spriteRect(gfx, 9, 22, 11, 28, 10);

        spriteRect(gfx, 3, 27, 6, 29, 1);
        spriteRect(gfx, 9, 27, 12, 29, 1);
    }

    /* =====================================================
       BODY
       ===================================================== */

    if (characterConfig.gender == 0)
    {
        /* broader male torso */
        spriteRect(gfx, 4, 14, 11, 22, clothes);
        spriteRect(gfx, 3, 16, 4, 21, 11);
        spriteRect(gfx, 11, 16, 12, 21, 11);
    }
    else
    {
        /* narrower upper body + wider lower silhouette */
        spriteRect(gfx, 5, 14, 10, 20, clothes);
        spriteRect(gfx, 4, 19, 11, 23, clothes);

        spriteRect(gfx, 3, 16, 4, 21, 11);
        spriteRect(gfx, 11, 16, 12, 21, 11);
    }

    /* =====================================================
       CLOTHING TYPES
       ===================================================== */

    if (characterConfig.clothes == 0)
    {
        /* T-shirt */
        spriteRect(gfx, 3, 15, 5, 18, clothes);
        spriteRect(gfx, 10, 15, 12, 18, clothes);

        spritePixel(gfx, 6, 15, 12);
    }
    else if (characterConfig.clothes == 1)
    {
        /* hoodie */
        spriteRect(gfx, 3, 14, 12, 18, clothes);
        spriteRect(gfx, 4, 13, 11, 15, clothes);

        spritePixel(gfx, 7, 16, 12);
        spritePixel(gfx, 8, 16, 12);

        spriteRect(gfx, 6, 20, 9, 21, 1);
    }
    else if (characterConfig.clothes == 2)
    {
        /* jacket */
        spriteRect(gfx, 3, 15, 12, 22, clothes);

        spriteRect(gfx, 7, 15, 8, 22, 1);

        spritePixel(gfx, 5, 17, 12);
        spritePixel(gfx, 10, 17, 12);
    }
    else
    {
        /* sweater */
        spriteRect(gfx, 3, 15, 12, 22, clothes);
        spriteRect(gfx, 3, 18, 4, 22, clothes);
        spriteRect(gfx, 11, 18, 12, 22, clothes);

        spriteRect(gfx, 5, 21, 10, 22, 1);
    }

    /* neck */
    spriteRect(gfx, 7, 12, 8, 15, 11);

    /* =====================================================
       HEAD
       ===================================================== */

    spriteRect(gfx, 5, 4, 10, 13, 11);
    spriteRect(gfx, 4, 6, 11, 12, 11);

    /* ears */
    spriteRect(gfx, 3, 8, 4, 11, 11);
    spriteRect(gfx, 11, 8, 12, 11, 11);

    /* =====================================================
       HAIR
       ===================================================== */

    spriteRect(gfx, 5, 2, 10, 4, hair);
    spriteRect(gfx, 4, 3, 11, 7, hair);

    spritePixel(gfx, 3, 5, hair);
    spritePixel(gfx, 12, 5, hair);

    spriteRect(gfx, 3, 6, 5, 9, hair);
    spriteRect(gfx, 10, 6, 12, 8, hair);

    if (characterConfig.hairLength)
    {
        spriteRect(gfx, 3, 8, 4, 16, hair);
        spriteRect(gfx, 11, 8, 12, 16, hair);

        if (characterConfig.gender == 1)
        {
            spritePixel(gfx, 4, 16, hair);
            spritePixel(gfx, 11, 16, hair);
        }
    }

    /* =====================================================
       DIRECTION
       ===================================================== */

    if (direction == DIR_DOWN)
    {
        spritePixel(gfx, 6, 9, 1);
        spritePixel(gfx, 9, 9, 1);

        spritePixel(gfx, 7, 12, 3);
    }
    else if (direction == DIR_UP)
    {
        /* back of head */
        spriteRect(gfx, 5, 6, 10, 11, hair);
        spriteRect(gfx, 4, 8, 11, 10, hair);
    }
    else if (direction == DIR_LEFT)
    {
        spritePixel(gfx, 5, 9, 1);
        spritePixel(gfx, 4, 11, 3);
    }
    else
    {
        spritePixel(gfx, 10, 9, 1);
        spritePixel(gfx, 11, 11, 3);
    }
}

/* =========================================================
   COLLISION
   ========================================================= */

static int canMoveTo(int x, int y)
{
    if (worldIsBlocked(x + 4, y + 26))
        return 0;

    if (worldIsBlocked(x + 11, y + 26))
        return 0;

    if (worldIsBlocked(x + 4, y + 29))
        return 0;

    if (worldIsBlocked(x + 11, y + 29))
        return 0;

    return 1;
}

/* =========================================================
   INIT
   ========================================================= */

void playerInit(void)
{
    int i;

    player.x = 244;
    player.y = 92;

    player.direction = DIR_DOWN;
    player.frame = 0;
    player.animationTimer = 0;

    /* transparent */
    SPRITE_PALETTE[0] = RGB5(0,0,0);

    /* outline */
    SPRITE_PALETTE[1] = RGB5(3,3,4);

    /* hair */
    SPRITE_PALETTE[2] = RGB5(2,2,3);
    SPRITE_PALETTE[3] = RGB5(12,6,3);
    SPRITE_PALETTE[4] = RGB5(28,22,10);
    SPRITE_PALETTE[5] = RGB5(21,7,3);

    /* clothing */
    SPRITE_PALETTE[6] = RGB5(5,14,29);
    SPRITE_PALETTE[7] = RGB5(26,5,5);
    SPRITE_PALETTE[8] = RGB5(6,23,9);
    SPRITE_PALETTE[9] = RGB5(18,7,25);

    /* trousers */
    SPRITE_PALETTE[10] = RGB5(8,9,12);

    /* skin */
    SPRITE_PALETTE[11] = RGB5(27,18,13);

    /* highlight */
    SPRITE_PALETTE[12] = RGB5(31,31,31);

    /*
        Clear enough OBJ VRAM for our eight tiles.
    */

    for (i = 0; i < 128; i++)
        ((u16 *)SPRITE_GFX)[i] = 0;

    /*
        Hide all objects.
    */

    for (i = 0; i < 128; i++)
    {
        OAM[i].attr0 = OBJ_HIDE;
        OAM[i].attr1 = 0;
        OAM[i].attr2 = 0;
    }

    makePlayerFrame(
        (u16 *)SPRITE_GFX,
        player.direction,
        player.frame
    );

    /*
        Character menu left forced blank enabled.
        Everything is ready now, so turn LCD back on.
    */

    REG_DISPCNT &= ~0x0080;
}

/* =========================================================
   UPDATE
   ========================================================= */

void playerUpdate(void)
{
    int newX = player.x;
    int newY = player.y;
    int moving = 0;

    u16 held;

    scanKeys();
    held = keysHeld();

    if (held & KEY_UP)
    {
        newY--;
        player.direction = DIR_UP;
        moving = 1;
    }
    else if (held & KEY_DOWN)
    {
        newY++;
        player.direction = DIR_DOWN;
        moving = 1;
    }
    else if (held & KEY_LEFT)
    {
        newX--;
        player.direction = DIR_LEFT;
        moving = 1;
    }
    else if (held & KEY_RIGHT)
    {
        newX++;
        player.direction = DIR_RIGHT;
        moving = 1;
    }

    if (
        moving &&
        canMoveTo(newX, newY)
    )
    {
        player.x = newX;
        player.y = newY;

        player.animationTimer++;

        if (player.animationTimer >= 8)
        {
            player.animationTimer = 0;
            player.frame ^= 1;
        }
    }
    else
    {
        player.animationTimer = 0;
        player.frame = 0;
    }

    makePlayerFrame(
        (u16 *)SPRITE_GFX,
        player.direction,
        player.frame
    );
}

/* =========================================================
   DRAW
   ========================================================= */

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
        (screenY & OBJ_Y_MASK) |
        OBJ_SHAPE_TALL;

    OAM[0].attr1 =
        (screenX & OBJ_X_MASK) |
        OBJ_SIZE_16X32;

    OAM[0].attr2 =
        OBJ_PRIORITY_1;

    CpuFastSet(
        OAM,
        (void *)0x07000000,
        COPY32 |
        (sizeof(OAM) / 4)
    );
}
