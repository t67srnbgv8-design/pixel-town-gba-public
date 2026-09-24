#include <gba.h>

#include "tileset.h"
#include "metatiles.h"
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

    SetMode(
        MODE_0 |
        BG0_ON |
        BG1_ON |
        OBJ_ON |
        OBJ_1D_MAP
    );

    /*
       BG0 = normal ground/world

       Priority 2 means the player
       sprite can appear above it.
    */

    REG_BG0CNT =
        BG_PRIORITY(2) |
        CHAR_BASE(0) |
        SCREEN_BASE(28) |
        BG_16_COLOR |
        BG_SIZE_1;

    /*
       BG1 = canopy / foreground

       Priority 0 puts it above player.
    */

    REG_BG1CNT =
        BG_PRIORITY(0) |
        CHAR_BASE(0) |
        SCREEN_BASE(30) |
        BG_16_COLOR |
        BG_SIZE_1;

    tilesetInit();

    metatilesInit();

    worldInit();
    worldDraw();

    playerInit();

    while (1)
    {
        VBlankIntrWait();

        playerUpdate();

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

        REG_BG0HOFS =
            cameraX;

        REG_BG0VOFS =
            cameraY;

        REG_BG1HOFS =
            cameraX;

        REG_BG1VOFS =
            cameraY;

        playerDraw(
            cameraX,
            cameraY
        );
    }

    return 0;
}
