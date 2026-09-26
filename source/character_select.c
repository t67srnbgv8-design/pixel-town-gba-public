#include <gba.h>
#include "character_select.h"

#define W 240
#define H 160

#define C_BG      RGB5(5,10,18)
#define C_PANEL   RGB5(10,16,24)
#define C_WHITE   RGB5(31,31,31)
#define C_GREY    RGB5(17,19,22)
#define C_GOLD    RGB5(31,23,7)
#define C_SKIN    RGB5(27,18,13)
#define C_BLACK   RGB5(3,3,4)
#define C_BROWN   RGB5(12,6,3)
#define C_BLONDE  RGB5(28,22,10)
#define C_REDHAIR RGB5(21,7,3)
#define C_BLUE    RGB5(5,14,29)
#define C_RED     RGB5(26,5,5)
#define C_GREEN   RGB5(6,22,9)
#define C_PURPLE  RGB5(18,7,25)
#define C_DARK    RGB5(6,7,10)

CharacterConfig characterConfig =
{
    0,  /* gender */
    1,  /* hairColor */
    0,  /* hairLength */
    0   /* clothes */
};

static volatile u16 *fb = (volatile u16 *)MODE3_FB;

/* =========================================================
   DRAW
   ========================================================= */

static void px(int x, int y, u16 c)
{
    if (x < 0 || y < 0 || x >= W || y >= H)
        return;

    fb[y * W + x] = c;
}

static void box(int x, int y, int w, int h, u16 c)
{
    int xx;
    int yy;

    for (yy = 0; yy < h; yy++)
        for (xx = 0; xx < w; xx++)
            px(x + xx, y + yy, c);
}

/* =========================================================
   FONT
   ========================================================= */

static const unsigned char font[][7] =
{
    {14,17,17,31,17,17,17},
    {30,17,17,30,17,17,30},
    {14,17,16,16,16,17,14},
    {30,17,17,17,17,17,30},
    {31,16,16,30,16,16,31},
    {31,16,16,30,16,16,16},
    {14,17,16,23,17,17,14},
    {17,17,17,31,17,17,17},
    {14,4,4,4,4,4,14},
    {7,2,2,2,18,18,12},
    {17,18,20,24,20,18,17},
    {16,16,16,16,16,16,31},
    {17,27,21,21,17,17,17},
    {17,25,21,19,17,17,17},
    {14,17,17,17,17,17,14},
    {30,17,17,30,16,16,16},
    {14,17,17,17,21,18,13},
    {30,17,17,30,20,18,17},
    {15,16,16,14,1,1,30},
    {31,4,4,4,4,4,4},
    {17,17,17,17,17,17,14},
    {17,17,17,17,17,10,4},
    {17,17,17,21,21,21,10},
    {17,17,10,4,10,17,17},
    {17,17,10,4,4,4,4},
    {31,1,2,4,8,16,31}
};

static void chr(int x, int y, char c, u16 color)
{
    int r;
    int col;
    int n;

    if (c < 'A' || c > 'Z')
        return;

    n = c - 'A';

    for (r = 0; r < 7; r++)
    {
        for (col = 0; col < 5; col++)
        {
            if (font[n][r] & (1 << (4 - col)))
                px(x + col, y + r, color);
        }
    }
}

static void txt(int x, int y, const char *s, u16 color)
{
    while (*s)
    {
        if (*s != ' ')
            chr(x, y, *s, color);

        x += 6;
        s++;
    }
}

/* =========================================================
   APPEARANCE
   ========================================================= */

static u16 hair(void)
{
    switch (characterConfig.hairColor)
    {
        case 0: return C_BLACK;
        case 1: return C_BROWN;
        case 2: return C_BLONDE;
        default: return C_REDHAIR;
    }
}

static u16 clothesColor(void)
{
    switch (characterConfig.clothes)
    {
        case 0: return C_BLUE;
        case 1: return C_RED;
        case 2: return C_GREEN;
        default: return C_PURPLE;
    }
}

static const char *genderName(void)
{
    return characterConfig.gender == 0
        ? "MAN"
        : "WOMAN";
}

static const char *hairColorName(void)
{
    switch (characterConfig.hairColor)
    {
        case 0: return "BLACK";
        case 1: return "BROWN";
        case 2: return "BLOND";
        default: return "RED";
    }
}

static const char *hairLengthName(void)
{
    return characterConfig.hairLength == 0
        ? "SHORT"
        : "LONG";
}

static const char *clothesName(void)
{
    switch (characterConfig.clothes)
    {
        case 0: return "TSHIRT";
        case 1: return "HOODIE";
        case 2: return "JACKET";
        default: return "SWEATER";
    }
}

/* =========================================================
   BETTER GBA-RPG PREVIEW
   ========================================================= */

static void preview(void)
{
    int x = 185;
    int y = 48;

    u16 hc = hair();
    u16 shirt = clothesColor();

    /* shadow */
    box(x - 12, y + 67, 24, 4, C_DARK);
    box(x - 9, y + 65, 18, 3, C_DARK);

    /* shoes */
    box(x - 9, y + 59, 8, 7, C_BLACK);
    box(x + 1, y + 59, 8, 7, C_BLACK);

    /* trousers / legs */
    box(x - 7, y + 46, 6, 15, C_DARK);
    box(x + 1, y + 46, 6, 15, C_DARK);

    /* torso silhouette */
    if (characterConfig.gender == 0)
    {
        box(x - 11, y + 27, 22, 21, shirt);
        box(x - 14, y + 30, 4, 15, C_SKIN);
        box(x + 10, y + 30, 4, 15, C_SKIN);
    }
    else
    {
        box(x - 9, y + 27, 18, 19, shirt);
        box(x - 11, y + 42, 22, 6, shirt);
        box(x - 13, y + 30, 4, 14, C_SKIN);
        box(x + 9, y + 30, 4, 14, C_SKIN);
    }

    /* actual clothing silhouettes */
    if (characterConfig.clothes == 1)
    {
        /* hoodie */
        box(x - 12, y + 25, 24, 6, shirt);
        box(x - 8, y + 43, 16, 3, C_DARK);
    }
    else if (characterConfig.clothes == 2)
    {
        /* jacket */
        box(x - 2, y + 27, 4, 21, C_DARK);
        box(x - 9, y + 29, 3, 14, C_WHITE);
        box(x + 6, y + 29, 3, 14, C_WHITE);
    }
    else if (characterConfig.clothes == 3)
    {
        /* sweater */
        box(x - 12, y + 29, 3, 16, shirt);
        box(x + 9, y + 29, 3, 16, shirt);
        box(x - 8, y + 44, 16, 3, C_DARK);
    }
    else
    {
        /* t-shirt sleeves */
        box(x - 13, y + 28, 5, 8, shirt);
        box(x + 8, y + 28, 5, 8, shirt);
    }

    /* neck */
    box(x - 3, y + 23, 6, 6, C_SKIN);

    /* head – larger GBA RPG proportions */
    box(x - 9, y + 7, 18, 17, C_SKIN);
    box(x - 7, y + 5, 14, 21, C_SKIN);

    /* ears */
    box(x - 11, y + 12, 3, 8, C_SKIN);
    box(x + 8, y + 12, 3, 8, C_SKIN);

    /* hair silhouette */
    box(x - 9, y + 3, 18, 7, hc);
    box(x - 11, y + 6, 5, 10, hc);
    box(x + 6, y + 6, 5, 8, hc);

    /* hair crown */
    box(x - 6, y + 1, 12, 3, hc);
    px(x - 7, y + 2, hc);
    px(x + 7, y + 2, hc);

    if (characterConfig.hairLength)
    {
        box(x - 11, y + 12, 4, 17, hc);
        box(x + 7, y + 12, 4, 17, hc);
        box(x - 8, y + 23, 3, 6, hc);
        box(x + 5, y + 23, 3, 6, hc);
    }

    /* eyes */
    box(x - 5, y + 15, 2, 2, C_BLACK);
    box(x + 3, y + 15, 2, 2, C_BLACK);

    /* nose/shadow */
    px(x, y + 19, C_BROWN);

    /* shirt highlight */
    if (characterConfig.clothes != 2)
        box(x - 5, y + 30, 2, 9, C_WHITE);
}

/* =========================================================
   MENU
   ========================================================= */

static void arrow(int x, int y)
{
    px(x, y, C_GOLD);
    box(x + 1, y - 1, 2, 3, C_GOLD);
    box(x + 3, y - 2, 2, 5, C_GOLD);
}

static void drawMenu(int selected)
{
    /*
        IMPORTANT:
        This is only called while the LCD is hidden.
        Therefore Delta never sees the screen halfway drawn.
    */

    box(0, 0, W, H, C_BG);

    box(8, 7, 224, 18, C_PANEL);
    txt(68, 12, "CREATE CHARACTER", C_WHITE);

    box(10, 32, 143, 100, C_PANEL);
    box(158, 32, 72, 100, C_PANEL);

    txt(25, 42, "GENDER", C_WHITE);
    txt(91, 42, genderName(), C_GOLD);

    txt(25, 62, "HAIR", C_WHITE);
    txt(91, 62, hairColorName(), C_GOLD);

    txt(25, 82, "LENGTH", C_WHITE);
    txt(91, 82, hairLengthName(), C_GOLD);

    txt(25, 102, "CLOTHES", C_WHITE);
    txt(91, 102, clothesName(), C_GOLD);

    arrow(16, 45 + selected * 20);

    preview();

    txt(15, 143, "DPAD CHANGE", C_GREY);
    txt(169, 143, "START", C_GOLD);
}

/*
    Blank display while changing the Mode 3 framebuffer.

    Bit 7 of DISPCNT = forced blank.
*/

static void redrawHidden(int selected)
{
    REG_DISPCNT |= 0x0080;

    drawMenu(selected);

    VBlankIntrWait();

    REG_DISPCNT &= ~0x0080;
}

/* =========================================================
   SELECT
   ========================================================= */

void characterSelectRun(void)
{
    int selected = 0;

    SetMode(MODE_3 | BG2_ON);

    redrawHidden(selected);

    while (1)
    {
        u16 down;

        VBlankIntrWait();
        scanKeys();

        down = keysDown();

        if (down & KEY_UP)
        {
            selected--;

            if (selected < 0)
                selected = 3;

            redrawHidden(selected);
        }
        else if (down & KEY_DOWN)
        {
            selected++;

            if (selected > 3)
                selected = 0;

            redrawHidden(selected);
        }
        else if (down & KEY_LEFT)
        {
            if (selected == 0)
            {
                characterConfig.gender ^= 1;
            }
            else if (selected == 1)
            {
                characterConfig.hairColor--;

                if (characterConfig.hairColor < 0)
                    characterConfig.hairColor = 3;
            }
            else if (selected == 2)
            {
                characterConfig.hairLength ^= 1;
            }
            else
            {
                characterConfig.clothes--;

                if (characterConfig.clothes < 0)
                    characterConfig.clothes = 3;
            }

            redrawHidden(selected);
        }
        else if (down & KEY_RIGHT)
        {
            if (selected == 0)
            {
                characterConfig.gender ^= 1;
            }
            else if (selected == 1)
            {
                characterConfig.hairColor++;

                if (characterConfig.hairColor > 3)
                    characterConfig.hairColor = 0;
            }
            else if (selected == 2)
            {
                characterConfig.hairLength ^= 1;
            }
            else
            {
                characterConfig.clothes++;

                if (characterConfig.clothes > 3)
                    characterConfig.clothes = 0;
            }

            redrawHidden(selected);
        }

        if (down & KEY_START)
            break;
    }

    do
    {
        VBlankIntrWait();
        scanKeys();
    }
    while (keysHeld() & KEY_START);

    /*
        Hide LCD while main.c changes from Mode 3 to Mode 0.
    */
    REG_DISPCNT |= 0x0080;
}
