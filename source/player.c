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

/*
    Raw GBA OAM bits.

    attr0:
    bits 8-9  = object mode
    bit  13   = 256-color when set; clear = 16-color
    bits 14-15 = shape

    attr1:
    bits 14-15 = size

    For a 16x32 sprite:
    shape = TALL
    size  = 1
*/

#define OBJ_Y_MASK          0x00FF
#define OBJ_X_MASK          0x01FF

#define OBJ_MODE_HIDE       0x0200

#define OBJ_SHAPE_TALL      0x8000
#define OBJ_SIZE_16X32      0x4000

#define OBJ_PRIORITY_1      0x0400

/* =========================================================
   SPRITE DRAWING
   ========================================================= */

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
    {
        return;
    }

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
        16 x 32 pixels
        4 bits per pixel
        = 256 bytes
        = 128 u16
    */

    for (i = 0; i < 128; i++)
        gfx[i] = 0;
}

/* =========================================================
   APPEARANCE
   ========================================================= */

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

/* =========================================================
   PLAYER GRAPHICS
   ========================================================= */

static void makePlayerFrame(
    u16 *gfx,
    int direction,
    int frame
)
{
    int hair;
    int shirt;

    int leftLegOffset = 0;
    int rightLegOffset = 0;

    hair =
        getHairColor();

    shirt =
        getShirtColor();

    clearSprite(
        gfx
    );

    /*
        Walking animation.

        The feet alternate slightly.
    */

    if (frame == 1)
    {
        leftLegOffset = -1;
        rightLegOffset = 1;
    }

    /* =====================================================
       SHADOW
       ===================================================== */

    spriteRect(
        gfx,
        4,
        29,
        11,
        30,
        1
    );

    /* =====================================================
       LEGS
       ===================================================== */

    spriteRect(
        gfx,
        5 + leftLegOffset,
        22,
        7 + leftLegOffset,
        28,
        10
    );

    spriteRect(
        gfx,
        8 + rightLegOffset,
        22,
        10 + rightLegOffset,
        28,
        10
    );

    /* =====================================================
       SHOES
       ===================================================== */

    spriteRect(
        gfx,
        4 + leftLegOffset,
        28,
        7 + leftLegOffset,
        30,
        1
    );

    spriteRect(
        gfx,
        8 + rightLegOffset,
        28,
        11 + rightLegOffset,
        30,
        1
    );

    /* =====================================================
       BODY
       ===================================================== */

    if (
        characterConfig.gender == 0
    )
    {
        /*
            Male body
        */

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
        /*
            Female body
        */

        spriteRect(
            gfx,
            5,
            14,
            10,
            21,
            shirt
        );

        /*
            Lower clothing shape.
        */

        spriteRect(
            gfx,
            4,
            20,
            11,
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

    /* =====================================================
       NECK
       ===================================================== */

    spriteRect(
        gfx,
        7,
        12,
        8,
        15,
        11
    );

    /* =====================================================
       HEAD
       ===================================================== */

    spriteRect(
        gfx,
        4,
        5,
        11,
        13,
        11
    );

    /*
        Slightly rounder face.
    */

    spriteRect(
        gfx,
        5,
        4,
        10,
        14,
        11
    );

    /* ears */

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

    /* =====================================================
       HAIR
       ===================================================== */

    spriteRect(
        gfx,
        4,
        3,
        11,
        6,
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
        Hair crown shape.
    */

    spritePixel(
        gfx,
        5,
        2,
        hair
    );

    spriteRect(
        gfx,
        6,
        2,
        9,
        2,
        hair
    );

    spritePixel(
        gfx,
        10,
        2,
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
            16,
            hair
        );

        spriteRect(
            gfx,
            11,
            8,
            12,
            16,
            hair
        );

        spritePixel(
            gfx,
            4,
            16,
            hair
        );

        spritePixel(
            gfx,
            11,
            16,
            hair
        );
    }

    /* =====================================================
       FACE / DIRECTION
       ===================================================== */

    if (
        direction == DIR_DOWN
    )
    {
        /*
            Front-facing eyes.
        */

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

        /*
            tiny face shadow
        */

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
    else
    {
        /*
            Back-facing hair.
        */

        spriteRect(
            gfx,
            5,
            7,
            10,
            11,
            hair
        );
    }

    /* =====================================================
       CLOTHING HIGHLIGHT
       ===================================================== */

    if (
        direction != DIR_UP
    )
    {
        spriteRect(
            gfx,
            5,
            15,
            6,
            18,
            12
        );
    }
}

/* =========================================================
   COLLISION
   ========================================================= */

static int canMoveTo(
    int x,
    int y
)
{
    /*
        Collision only around feet.

        This makes the player feel like a top-down RPG
        character rather than a rectangular object.
    */

    if (
        worldIsBlocked(
            x + 4,
            y + 26
        )
    )
    {
        return 0;
    }

    if (
        worldIsBlocked(
            x + 11,
            y + 26
        )
    )
    {
        return 0;
    }

    if (
        worldIsBlocked(
            x + 4,
            y + 29
        )
    )
    {
        return 0;
    }

    if (
        worldIsBlocked(
            x + 11,
            y + 29
        )
    )
    {
        return 0;
    }

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

    player.direction =
        DIR_DOWN;

    player.frame = 0;

    player.animationTimer = 0;

    /* =====================================================
       OBJ PALETTE
       ===================================================== */

    /*
        Transparent
    */

    SPRITE_PALETTE[0] =
        RGB5(0,0,0);

    /*
        Outline / shoes
    */

    SPRITE_PALETTE[1] =
        RGB5(3,3,4);

    /*
        Hair colours
    */

    SPRITE_PALETTE[2] =
        RGB5(2,2,3);

    SPRITE_PALETTE[3] =
        RGB5(12,6,3);

    SPRITE_PALETTE[4] =
        RGB5(28,22,10);

    SPRITE_PALETTE[5] =
        RGB5(21,7,3);

    /*
        Clothes
    */

    SPRITE_PALETTE[6] =
        RGB5(5,14,29);

    SPRITE_PALETTE[7] =
        RGB5(26,5,5);

    SPRITE_PALETTE[8] =
        RGB5(6,23,9);

    SPRITE_PALETTE[9] =
        RGB5(18,7,25);

    /*
        Trousers
    */

    SPRITE_PALETTE[10] =
        RGB5(8,9,12);

    /*
        Skin
    */

    SPRITE_PALETTE[11] =
        RGB5(27,18,13);

    /*
        Highlight
    */

    SPRITE_PALETTE[12] =
        RGB5(31,31,31);

    /* =====================================================
       CLEAR OBJ VRAM
       ===================================================== */

    for (i = 0; i < 128; i++)
    {
        ((u16 *)SPRITE_GFX)[i] = 0;
    }

    /* =====================================================
       HIDE ALL SPRITES

       attr0 object mode = 2 means disabled.
       ===================================================== */

    for (i = 0; i < 128; i++)
    {
        OAM[i].attr0 =
            OBJ_MODE_HIDE;

        OAM[i].attr1 = 0;
        OAM[i].attr2 = 0;
    }

    makePlayerFrame(
        (u16 *)SPRITE_GFX,
        player.direction,
        player.frame
    );
}

/* =========================================================
   UPDATE
   ========================================================= */

void playerUpdate(void)
{
    int newX;
    int newY;
    int moving;

    u16 held;

    newX =
        player.x;

    newY =
        player.y;

    moving = 0;

    scanKeys();

    held =
        keysHeld();

    /*
        No diagonal movement yet.
    */

    if (
        held & KEY_UP
    )
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

    /*
        Rebuild current 16x32 frame.
    */

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
    int screenX;
    int screenY;

    screenX =
        player.x -
        cameraX;

    screenY =
        player.y -
        cameraY;

    /*
        ATTR0

        bits 0-7   Y
        bits 14-15 shape

        TALL + 4bpp.
        4bpp is the default, so no special flag required.
    */

    OAM[0].attr0 =
        (screenY & OBJ_Y_MASK) |
        OBJ_SHAPE_TALL;

    /*
        ATTR1

        bits 0-8   X
        bits 14-15 size

        Tall + size 1 = 16x32.
    */

    OAM[0].attr1 =
        (screenX & OBJ_X_MASK) |
        OBJ_SIZE_16X32;

    /*
        ATTR2

        tile 0
        priority 1
        palette bank 0
    */

    OAM[0].attr2 =
        OBJ_PRIORITY_1;

    /*
        Copy shadow OAM to hardware OAM.
    */

    CpuFastSet(
        OAM,
        (void *)0x07000000,
        COPY32 |
        (sizeof(OAM) / 4)
    );
}
