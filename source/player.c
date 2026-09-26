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

static void spritePixel(
    u16 *gfx,
    int x,
    int y,
    int color
)
{
    int p;
    int word;
    int shift;

    if (
        x < 0 ||
        x >= 16 ||
        y < 0 ||
        y >= 32
    )
        return;

    p =
        y * 16 + x;

    word =
        p >> 2;

    shift =
        (p & 3) * 4;

    gfx[word] &=
        ~(0xF << shift);

    gfx[word] |=
        (color & 15) << shift;
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
    {
        for (x = x1; x <= x2; x++)
        {
            spritePixel(
                gfx,
                x,
                y,
                color
            );
        }
    }
}

static void clearSprite(
    u16 *gfx
)
{
    int i;

    /*
        16 x 32 @ 4bpp =
        256 bytes =
        128 u16
    */

    for (i = 0; i < 128; i++)
        gfx[i] = 0;
}

static int getHairColor(void)
{
    switch (
        characterConfig.hairColor
    )
    {
        case 0:
            return 2;

        case 1:
            return 3;

        case 2:
            return 4;

        default:
            return 5;
    }
}

static int getShirtColor(void)
{
    switch (
        characterConfig.clothes
    )
    {
        case 0:
            return 6;

        case 1:
            return 7;

        case 2:
            return 8;

        default:
            return 9;
    }
}

static void makePlayerFrame(
    u16 *gfx,
    int direction,
    int frame
)
{
    int hair =
        getHairColor();

    int shirt =
        getShirtColor();

    int legOffset = 0;

    clearSprite(
        gfx
    );

    /*
        Walk animation.
    */

    if (frame == 1)
        legOffset = 1;

    /*
        Shadow
    */

    spriteRect(
        gfx,
        4,
        29,
        11,
        30,
        1
    );

    /*
        Legs
    */

    spriteRect(
        gfx,
        5 - legOffset,
        22,
        7 - legOffset,
        28,
        10
    );

    spriteRect(
        gfx,
        8 + legOffset,
        22,
        10 + legOffset,
        28,
        10
    );

    /*
        Shoes
    */

    spriteRect(
        gfx,
        4 - legOffset,
        28,
        7 - legOffset,
        30,
        1
    );

    spriteRect(
        gfx,
        8 + legOffset,
        28,
        11 + legOffset,
        30,
        1
    );

    /*
        Torso.
    */

    if (
        characterConfig.gender == 0
    )
    {
        spriteRect(
            gfx,
            4,
            14,
            11,
            23,
            shirt
        );

        spriteRect(
            gfx,
            3,
            15,
            4,
            22,
            11
        );

        spriteRect(
            gfx,
            11,
            15,
            12,
            22,
            11
        );
    }
    else
    {
        spriteRect(
            gfx,
            5,
            14,
            10,
            23,
            shirt
        );

        spriteRect(
            gfx,
            3,
            16,
            4,
            22,
            11
        );

        spriteRect(
            gfx,
            11,
            16,
            12,
            22,
            11
        );
    }

    /*
        Neck
    */

    spriteRect(
        gfx,
        7,
        12,
        8,
        15,
        11
    );

    /*
        Head
    */

    spriteRect(
        gfx,
        4,
        5,
        11,
        13,
        11
    );

    /*
        ears
    */

    spriteRect(
        gfx,
        3,
        8,
        4,
        11,
        11
    );

    spriteRect(
        gfx,
        11,
        8,
        12,
        11,
        11
    );

    /*
        Hair.
    */

    spriteRect(
        gfx,
        4,
        3,
        11,
        7,
        hair
    );

    spriteRect(
        gfx,
        3,
        5,
        5,
        9,
        hair
    );

    spriteRect(
        gfx,
        10,
        5,
        12,
        8,
        hair
    );

    /*
        Long hair.
    */

    if (
        characterConfig.hairLength == 1
    )
    {
        spriteRect(
            gfx,
            3,
            8,
            4,
            15,
            hair
        );

        spriteRect(
            gfx,
            11,
            8,
            12,
            15,
            hair
        );
    }

    /*
        Directional face details.
    */

    if (direction == DIR_DOWN)
    {
        spritePixel(
            gfx,
            6,
            9,
            1
        );

        spritePixel(
            gfx,
            9,
            9,
            1
        );

        spritePixel(
            gfx,
            8,
            12,
            3
        );
    }
    else if (
        direction == DIR_LEFT
    )
    {
        spritePixel(
            gfx,
            5,
            9,
            1
        );

        spritePixel(
            gfx,
            4,
            11,
            3
        );
    }
    else if (
        direction == DIR_RIGHT
    )
    {
        spritePixel(
            gfx,
            10,
            9,
            1
        );

        spritePixel(
            gfx,
            11,
            11,
            3
        );
    }

    /*
        Shirt highlight.
    */

    spriteRect(
        gfx,
        5,
        15,
        6,
        18,
        12
    );
}

static int canMoveTo(
    int x,
    int y
)
{
    /*
        Feet-only collision.
        Allows the head/body to overlap visual scenery.
    */

    if (
        worldIsBlocked(
            x + 4,
            y + 26
        )
    )
        return 0;

    if (
        worldIsBlocked(
            x + 11,
            y + 26
        )
    )
        return 0;

    if (
        worldIsBlocked(
            x + 4,
            y + 29
        )
    )
        return 0;

    if (
        worldIsBlocked(
            x + 11,
            y + 29
        )
    )
        return 0;

    return 1;
}

void playerInit(void)
{
    int i;

    player.x = 244;
    player.y = 92;

    player.direction =
        DIR_DOWN;

    player.frame = 0;

    player.animationTimer = 0;

    /*
        OBJ palette.
    */

    SPRITE_PALETTE[0] =
        RGB5(0,0,0);

    SPRITE_PALETTE[1] =
        RGB5(3,3,4);

    /* hair */
    SPRITE_PALETTE[2] =
        RGB5(2,2,3);

    SPRITE_PALETTE[3] =
        RGB5(12,6,3);

    SPRITE_PALETTE[4] =
        RGB5(28,22,10);

    SPRITE_PALETTE[5] =
        RGB5(21,7,3);

    /* clothes */
    SPRITE_PALETTE[6] =
        RGB5(5,14,29);

    SPRITE_PALETTE[7] =
        RGB5(26,5,5);

    SPRITE_PALETTE[8] =
        RGB5(6,23,9);

    SPRITE_PALETTE[9] =
        RGB5(18,7,25);

    /* trousers */
    SPRITE_PALETTE[10] =
        RGB5(8,9,12);

    /* skin */
    SPRITE_PALETTE[11] =
        RGB5(27,18,13);

    /* highlight */
    SPRITE_PALETTE[12] =
        RGB5(31,31,31);

    for (i = 0; i < 128; i++)
        ((u16 *)SPRITE_GFX)[i] = 0;

    /*
        Hide all OAM objects first.
    */

    for (i = 0; i < 128; i++)
    {
        OAM[i].attr0 =
            ATTR0_HIDE;

        OAM[i].attr1 = 0;
        OAM[i].attr2 = 0;
    }

    makePlayerFrame(
        (u16 *)SPRITE_GFX,
        player.direction,
        player.frame
    );
}

void playerUpdate(void)
{
    int newX =
        player.x;

    int newY =
        player.y;

    int moving = 0;

    scanKeys();

    u16 held =
        keysHeld();

    /*
        No diagonal movement.
    */

    if (held & KEY_UP)
    {
        newY--;

        player.direction =
            DIR_UP;

        moving = 1;
    }
    else if (
        held & KEY_DOWN
    )
    {
        newY++;

        player.direction =
            DIR_DOWN;

        moving = 1;
    }
    else if (
        held & KEY_LEFT
    )
    {
        newX--;

        player.direction =
            DIR_LEFT;

        moving = 1;
    }
    else if (
        held & KEY_RIGHT
    )
    {
        newX++;

        player.direction =
            DIR_RIGHT;

        moving = 1;
    }

    if (
        moving &&
        canMoveTo(
            newX,
            newY
        )
    )
    {
        player.x =
            newX;

        player.y =
            newY;

        player.animationTimer++;

        if (
            player.animationTimer >= 8
        )
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

void playerDraw(
    int cameraX,
    int cameraY
)
{
    int screenX =
        player.x -
        cameraX;

    int screenY =
        player.y -
        cameraY;

    OAM[0].attr0 =
        ATTR0_SQUARE |
        ATTR0_4BPP |
        (screenY & 255);

    /*
        16 x 32 sprite uses wide shape.
    */

    OAM[0].attr0 =
        ATTR0_TALL |
        ATTR0_4BPP |
        (screenY & 255);

    OAM[0].attr1 =
        ATTR1_SIZE_1 |
        (screenX & 511);

    OAM[0].attr2 =
        0 |
        ATTR2_PRIORITY(1);

    CpuFastSet(
        OAM,
        OAM_MEM,
        COPY32 |
        (sizeof(OAM) / 4)
    );
}
