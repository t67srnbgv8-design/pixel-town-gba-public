#include <gba.h>

#include "tileset.h"
#include "world.h"
#include "player.h"

#define SCREEN_W 240
#define SCREEN_H 160

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

int main(void)
{
    irqInit();
    irqEnable(IRQ_VBLANK);

    /*
       Mode 0:
       BG0 = ground
       BG1 = foreground
       OBJ = player
    */
    SetMode(
        MODE_0 |
        BG0_ON |
        BG1_ON |
        OBJ_ON |
        OBJ_1D_MAP
    );

    /*
       64x32 ground map:
       screen blocks 28 + 29
    */
    REG_BG0CNT =
        BG_PRIORITY(2) |
        CHAR_BASE(0) |
        SCREEN_BASE(28) |
        BG_16_COLOR |
        BG_SIZE_1;

    /*
       64x32 foreground map:
       screen blocks 30 + 31

       Priority 0 means this can appear in front
       of the player.
    */
    REG_BG1CNT =
        BG_PRIORITY(0) |
        CHAR_BASE(0) |
        SCREEN_BASE(30) |
        BG_16_COLOR |
        BG_SIZE_1;

    tilesetInit();

    worldInit();
    worldDraw();

    playerInit();

    while (1)
    {
        VBlankIntrWait();

        playerUpdate();

        /*
           Camera follows player.
        */
        int cameraX =
            player.x + 8 -
            SCREEN_W / 2;

        int cameraY =
            player.y + 16 -
            SCREEN_H / 2;

        cameraX = clampValue(
            cameraX,
            0,
            WORLD_W - SCREEN_W
        );

        cameraY = clampValue(
            cameraY,
            0,
            WORLD_H - SCREEN_H
        );

        /*
           Both world layers move together.
        */
        REG_BG0HOFS = cameraX;
        REG_BG0VOFS = cameraY;

        REG_BG1HOFS = cameraX;
        REG_BG1VOFS = cameraY;

        playerDraw(
            cameraX,
            cameraY
        );
    }

    return 0;
}
