#include <gba.h>

#include "intro.h"
#include "character_select.h"
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

    /*
        Developer intro + title.
    */

    introRun();

    /*
        NEW:
        Character creation before entering town.
    */

    characterSelectRun();

    /*
        Enter game.
    */

    SetMode(
        MODE_0 |
        BG0_ON |
        BG1_ON |
        OBJ_ON |
        OBJ_1D_MAP
    );

    REG_BG0CNT =
        BG_PRIORITY(2) |
        CHAR_BASE(0) |
        SCREEN_BASE(28) |
        BG_16_COLOR |
        BG_SIZE_1;

    REG_BG1CNT =
        BG_PRIORITY(0) |
        CHAR_BASE(0) |
        SCREEN_BASE(30) |
        BG_16_COLOR |
        BG_SIZE_1;

    REG_BG0HOFS = 0;
    REG_BG0VOFS = 0;

    REG_BG1HOFS = 0;
    REG_BG1VOFS = 0;

    tilesetInit();

    metatilesInit();

    worldInit();

    worldDraw();

    playerInit();

    while (1)
    {
        int cameraX;
        int cameraY;

        VBlankIntrWait();

        playerUpdate();

        cameraX =
            player.x +
            8 -
            SCREEN_W / 2;

        cameraY =
            player.y +
            16 -
            SCREEN_H / 2;

        cameraX =
            clampValue(
                cameraX,
                0,
                WORLD_W - SCREEN_W
            );

        cameraY =
            clampValue(
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
