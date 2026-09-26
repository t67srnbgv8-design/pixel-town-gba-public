#include <gba.h>

#include "character_select.h"

#define SCREEN_W 240
#define SCREEN_H 160

#define COLOR_BG       RGB5(5,10,18)
#define COLOR_PANEL    RGB5(10,16,24)
#define COLOR_WHITE    RGB5(31,31,31)
#define COLOR_GREY     RGB5(18,20,22)
#define COLOR_BLUE     RGB5(7,17,31)
#define COLOR_GOLD     RGB5(31,23,7)
#define COLOR_SKIN     RGB5(27,18,13)
#define COLOR_DARKSKIN RGB5(18,11,7)
#define COLOR_BLACK    RGB5(3,3,4)
#define COLOR_BROWN    RGB5(12,6,3)
#define COLOR_BLONDE   RGB5(28,22,10)
#define COLOR_REDHAIR  RGB5(21,7,3)
#define COLOR_RED      RGB5(26,5,5)
#define COLOR_GREEN    RGB5(6,23,9)
#define COLOR_PURPLE   RGB5(18,7,25)

CharacterConfig characterConfig =
{
    0,
    1,
    0,
    0
};

static volatile u16 *video =
    (volatile u16 *)MODE3_FB;

static void pixel(
    int x,
    int y,
    u16 color
)
{
    if (
        x < 0 ||
        y < 0 ||
        x >= SCREEN_W ||
        y >= SCREEN_H
    )
        return;

    video[y * SCREEN_W + x] = color;
}

static void rect(
    int x,
    int y,
    int w,
    int h,
    u16 color
)
{
    int xx;
    int yy;

    for (yy = 0; yy < h; yy++)
    {
        for (xx = 0; xx < w; xx++)
        {
            pixel(
                x + xx,
                y + yy,
                color
            );
        }
    }
}

static void clearScreen(u16 color)
{
    rect(
        0,
        0,
        SCREEN_W,
        SCREEN_H,
        color
    );
}

/* =========================================================
   SMALL PIXEL FONT

   Only characters required by this menu.
   ========================================================= */

static const unsigned char font[][7] =
{
    {14,17,17,31,17,17,17}, /* A */
    {30,17,17,30,17,17,30}, /* B */
    {14,17,16,16,16,17,14}, /* C */
    {30,17,17,17,17,17,30}, /* D */
    {31,16,16,30,16,16,31}, /* E */
    {31,16,16,30,16,16,16}, /* F */
    {14,17,16,23,17,17,14}, /* G */
    {17,17,17,31,17,17,17}, /* H */
    {14,4,4,4,4,4,14},      /* I */
    {7,2,2,2,18,18,12},     /* J */
    {17,18,20,24,20,18,17}, /* K */
    {16,16,16,16,16,16,31}, /* L */
    {17,27,21,21,17,17,17}, /* M */
    {17,25,21,19,17,17,17}, /* N */
    {14,17,17,17,17,17,14}, /* O */
    {30,17,17,30,16,16,16}, /* P */
    {14,17,17,17,21,18,13}, /* Q */
    {30,17,17,30,20,18,17}, /* R */
    {15,16,16,14,1,1,30},   /* S */
    {31,4,4,4,4,4,4},       /* T */
    {17,17,17,17,17,17,14}, /* U */
    {17,17,17,17,17,10,4},  /* V */
    {17,17,17,21,21,21,10}, /* W */
    {17,17,10,4,10,17,17},  /* X */
    {17,17,10,4,4,4,4},     /* Y */
    {31,1,2,4,8,16,31}      /* Z */
};

static void drawChar(
    int x,
    int y,
    char c,
    u16 color
)
{
    int row;
    int col;
    int index;

    if (c < 'A' || c > 'Z')
        return;

    index =
        c - 'A';

    for (row = 0; row < 7; row++)
    {
        for (col = 0; col < 5; col++)
        {
            if (
                font[index][row] &
                (1 << (4 - col))
            )
            {
                pixel(
                    x + col,
                    y + row,
                    color
                );
            }
        }
    }
}

static void text(
    int x,
    int y,
    const char *str,
    u16 color
)
{
    while (*str)
    {
        if (*str == ' ')
        {
            x += 6;
        }
        else
        {
            drawChar(
                x,
                y,
                *str,
                color
            );

            x += 6;
        }

        str++;
    }
}

/* =========================================================
   CHARACTER PREVIEW
   ========================================================= */

static u16 hairColor(void)
{
    switch (characterConfig.hairColor)
    {
        case 0:
            return COLOR_BLACK;

        case 1:
            return COLOR_BROWN;

        case 2:
            return COLOR_BLONDE;

        default:
            return COLOR_REDHAIR;
    }
}

static u16 shirtColor(void)
{
    switch (characterConfig.clothes)
    {
        case 0:
            return COLOR_BLUE;

        case 1:
            return COLOR_RED;

        case 2:
            return COLOR_GREEN;

        default:
            return COLOR_PURPLE;
    }
}

static void drawPreview(void)
{
    int x = 178;
    int y = 45;

    u16 hair =
        hairColor();

    u16 shirt =
        shirtColor();

    /*
        Shadow
    */

    rect(
        x - 10,
        y + 69,
        20,
        3,
        COLOR_BLACK
    );

    /*
        Legs
    */

    rect(
        x - 7,
        y + 49,
        5,
        18,
        COLOR_GREY
    );

    rect(
        x + 2,
        y + 49,
        5,
        18,
        COLOR_GREY
    );

    /*
        Shoes
    */

    rect(
        x - 8,
        y + 65,
        7,
        4,
        COLOR_BLACK
    );

    rect(
        x + 1,
        y + 65,
        7,
        4,
        COLOR_BLACK
    );

    /*
        Body

        Female option has slightly narrower shoulders.
    */

    if (characterConfig.gender == 0)
    {
        rect(
            x - 9,
            y + 29,
            18,
            22,
            shirt
        );

        rect(
            x - 13,
            y + 31,
            4,
            17,
            COLOR_SKIN
        );

        rect(
            x + 9,
            y + 31,
            4,
            17,
            COLOR_SKIN
        );
    }
    else
    {
        rect(
            x - 8,
            y + 29,
            16,
            22,
            shirt
        );

        rect(
            x - 12,
            y + 32,
            4,
            15,
            COLOR_SKIN
        );

        rect(
            x + 8,
            y + 32,
            4,
            15,
            COLOR_SKIN
        );
    }

    /*
        Neck
    */

    rect(
        x - 3,
        y + 25,
        6,
        6,
        COLOR_SKIN
    );

    /*
        Head
    */

    rect(
        x - 9,
        y + 8,
        18,
        18,
        COLOR_SKIN
    );

    /*
        ears
    */

    rect(
        x - 11,
        y + 14,
        2,
        7,
        COLOR_SKIN
    );

    rect(
        x + 9,
        y + 14,
        2,
        7,
        COLOR_SKIN
    );

    /*
        Hair top
    */

    rect(
        x - 10,
        y + 5,
        20,
        7,
        hair
    );

    rect(
        x - 9,
        y + 10,
        4,
        7,
        hair
    );

    rect(
        x + 5,
        y + 10,
        4,
        5,
        hair
    );

    /*
        Hair length
    */

    if (characterConfig.hairLength == 1)
    {
        rect(
            x - 10,
            y + 12,
            4,
            18,
            hair
        );

        rect(
            x + 6,
            y + 12,
            4,
            18,
            hair
        );
    }

    /*
        eyes
    */

    pixel(
        x - 4,
        y + 17,
        COLOR_BLACK
    );

    pixel(
        x + 4,
        y + 17,
        COLOR_BLACK
    );

    /*
        shirt highlight
    */

    rect(
        x - 4,
        y + 32,
        3,
        10,
        COLOR_WHITE
    );
}

/* =========================================================
   MENU
   ========================================================= */

static void drawArrow(
    int x,
    int y
)
{
    pixel(x, y, COLOR_GOLD);

    rect(
        x + 1,
        y - 1,
        2,
        3,
        COLOR_GOLD
    );

    rect(
        x + 3,
        y - 2,
        2,
        5,
        COLOR_GOLD
    );
}

static void drawMenu(
    int selected
)
{
    clearScreen(
        COLOR_BG
    );

    /*
        Header
    */

    rect(
        8,
        7,
        224,
        18,
        COLOR_PANEL
    );

    text(
        56,
        12,
        "CHARACTER",
        COLOR_WHITE
    );

    /*
        Menu panel
    */

    rect(
        12,
        34,
        133,
        94,
        COLOR_PANEL
    );

    text(
        28,
        45,
        "GENDER",
        COLOR_WHITE
    );

    text(
        28,
        65,
        "HAIR COLOR",
        COLOR_WHITE
    );

    text(
        28,
        85,
        "HAIR LENGTH",
        COLOR_WHITE
    );

    text(
        28,
        105,
        "CLOTHES",
        COLOR_WHITE
    );

    drawArrow(
        18,
        48 + selected * 20
    );

    /*
        Preview panel
    */

    rect(
        153,
        34,
        74,
        94,
        COLOR_PANEL
    );

    drawPreview();

    /*
        Bottom
    */

    text(
        22,
        141,
        "LEFT RIGHT CHANGE",
        COLOR_GREY
    );

    text(
        153,
        141,
        "START",
        COLOR_GOLD
    );
}

/* =========================================================
   CHARACTER SELECT LOOP
   ========================================================= */

void characterSelectRun(void)
{
    int selected = 0;
    int dirty = 1;

    SetMode(
        MODE_3 |
        BG2_ON
    );

    while (1)
    {
        VBlankIntrWait();

        scanKeys();

        u16 down =
            keysDown();

        if (down & KEY_UP)
        {
            selected--;

            if (selected < 0)
                selected = 3;

            dirty = 1;
        }

        if (down & KEY_DOWN)
        {
            selected++;

            if (selected > 3)
                selected = 0;

            dirty = 1;
        }

        if (down & KEY_LEFT)
        {
            if (selected == 0)
            {
                characterConfig.gender ^= 1;
            }
            else if (selected == 1)
            {
                characterConfig.hairColor--;

                if (
                    characterConfig.hairColor < 0
                )
                {
                    characterConfig.hairColor = 3;
                }
            }
            else if (selected == 2)
            {
                characterConfig.hairLength ^= 1;
            }
            else
            {
                characterConfig.clothes--;

                if (
                    characterConfig.clothes < 0
                )
                {
                    characterConfig.clothes = 3;
                }
            }

            dirty = 1;
        }

        if (down & KEY_RIGHT)
        {
            if (selected == 0)
            {
                characterConfig.gender ^= 1;
            }
            else if (selected == 1)
            {
                characterConfig.hairColor++;

                if (
                    characterConfig.hairColor > 3
                )
                {
                    characterConfig.hairColor = 0;
                }
            }
            else if (selected == 2)
            {
                characterConfig.hairLength ^= 1;
            }
            else
            {
                characterConfig.clothes++;

                if (
                    characterConfig.clothes > 3
                )
                {
                    characterConfig.clothes = 0;
                }
            }

            dirty = 1;
        }

        if (dirty)
        {
            drawMenu(
                selected
            );

            dirty = 0;
        }

        if (down & KEY_START)
            break;
    }

    /*
        Wait until START is released.
    */

    do
    {
        VBlankIntrWait();
        scanKeys();

    } while (keysHeld() & KEY_START);
}
