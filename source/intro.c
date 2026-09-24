#include <gba.h>
#include <string.h>

#include "intro.h"

/*
    Pixel Town intro / title screen
    Developer: BAYA

    Mode 3 is used only for the intro.
    The actual game switches back to Mode 0 afterwards.
*/

#define W 240
#define H 160

static u16 *fb = (u16 *)MODE3_FB;

static void waitFrames(int frames)
{
    for (int i = 0; i < frames; i++)
        VBlankIntrWait();
}

static void clearScreen(u16 color)
{
    for (int i = 0; i < W * H; i++)
        fb[i] = color;
}

static void pixel(int x, int y, u16 color)
{
    if (x < 0 || y < 0 || x >= W || y >= H)
        return;

    fb[y * W + x] = color;
}

static void rect(
    int x,
    int y,
    int w,
    int h,
    u16 color
)
{
    for (int py = y; py < y + h; py++)
    {
        for (int px = x; px < x + w; px++)
            pixel(px, py, color);
    }
}

/*
    Tiny custom 5x7 font.
    Only characters needed by the intro are included.
*/

typedef struct
{
    char c;
    u8 row[7];
} Glyph;

static const Glyph font[] =
{
    {'A',{
        0x0E,
        0x11,
        0x11,
        0x1F,
        0x11,
        0x11,
        0x11
    }},

    {'B',{
        0x1E,
        0x11,
        0x11,
        0x1E,
        0x11,
        0x11,
        0x1E
    }},

    {'E',{
        0x1F,
        0x10,
        0x10,
        0x1E,
        0x10,
        0x10,
        0x1F
    }},

    {'I',{
        0x1F,
        0x04,
        0x04,
        0x04,
        0x04,
        0x04,
        0x1F
    }},

    {'L',{
        0x10,
        0x10,
        0x10,
        0x10,
        0x10,
        0x10,
        0x1F
    }},

    {'N',{
        0x11,
        0x19,
        0x19,
        0x15,
        0x13,
        0x13,
        0x11
    }},

    {'O',{
        0x0E,
        0x11,
        0x11,
        0x11,
        0x11,
        0x11,
        0x0E
    }},

    {'P',{
        0x1E,
        0x11,
        0x11,
        0x1E,
        0x10,
        0x10,
        0x10
    }},

    {'R',{
        0x1E,
        0x11,
        0x11,
        0x1E,
        0x14,
        0x12,
        0x11
    }},

    {'S',{
        0x0F,
        0x10,
        0x10,
        0x0E,
        0x01,
        0x01,
        0x1E
    }},

    {'T',{
        0x1F,
        0x04,
        0x04,
        0x04,
        0x04,
        0x04,
        0x04
    }},

    {'W',{
        0x11,
        0x11,
        0x11,
        0x15,
        0x15,
        0x1B,
        0x11
    }},

    {'Y',{
        0x11,
        0x11,
        0x0A,
        0x04,
        0x04,
        0x04,
        0x04
    }}
};

static const Glyph *findGlyph(char c)
{
    unsigned int count =
        sizeof(font) / sizeof(font[0]);

    for (unsigned int i = 0; i < count; i++)
    {
        if (font[i].c == c)
            return &font[i];
    }

    return 0;
}

static void drawChar(
    int x,
    int y,
    char c,
    int scale,
    u16 color
)
{
    const Glyph *g = findGlyph(c);

    if (!g)
        return;

    for (int row = 0; row < 7; row++)
    {
        for (int col = 0; col < 5; col++)
        {
            if (g->row[row] & (1 << (4 - col)))
            {
                rect(
                    x + col * scale,
                    y + row * scale,
                    scale,
                    scale,
                    color
                );
            }
        }
    }
}

static int textWidth(
    const char *text,
    int scale
)
{
    int len = strlen(text);

    if (len == 0)
        return 0;

    return len * 6 * scale - scale;
}

static void drawText(
    int x,
    int y,
    const char *text,
    int scale,
    u16 color
)
{
    int cursor = x;

    while (*text)
    {
        if (*text != ' ')
            drawChar(cursor, y, *text, scale, color);

        cursor += 6 * scale;
        text++;
    }
}

static void centeredText(
    int y,
    const char *text,
    int scale,
    u16 color
)
{
    int w = textWidth(text, scale);

    drawText(
        (W - w) / 2,
        y,
        text,
        scale,
        color
    );
}

static void fadeBlackToScene(void)
{
    /*
        Hardware brightness fade.
    */

    REG_BLDCNT =
        BLD_BLACK |
        BLD_BG2 |
        BLD_OBJ |
        BLD_BACKDROP;

    for (int i = 16; i >= 0; i--)
    {
        REG_BLDY = i;
        waitFrames(2);
    }

    REG_BLDCNT = 0;
    REG_BLDY = 0;
}

static void fadeSceneToBlack(void)
{
    REG_BLDCNT =
        BLD_BLACK |
        BLD_BG2 |
        BLD_OBJ |
        BLD_BACKDROP;

    for (int i = 0; i <= 16; i++)
    {
        REG_BLDY = i;
        waitFrames(2);
    }
}

/*
    BAYA developer card
*/

static void drawBaya(void)
{
    const u16 black =
        RGB5(0, 0, 0);

    const u16 white =
        RGB5(31, 31, 31);

    const u16 gray =
        RGB5(15, 17, 20);

    const u16 blue =
        RGB5(7, 18, 31);

    clearScreen(black);

    /*
        BAYA shadow
    */

    centeredText(
        53,
        "BAYA",
        5,
        gray
    );

    /*
        Main logo slightly above shadow
    */

    centeredText(
        50,
        "BAYA",
        5,
        white
    );

    /*
        Small blue underline
    */

    rect(
        83,
        91,
        74,
        3,
        blue
    );

    centeredText(
        105,
        "PRESENTS",
        1,
        RGB5(20, 22, 25)
    );
}

/*
    Small original city silhouette used behind title.
*/

static void drawTitleBackground(int offset)
{
    const u16 sky =
        RGB5(8, 18, 29);

    const u16 sky2 =
        RGB5(13, 23, 31);

    const u16 grass =
        RGB5(7, 18, 8);

    const u16 grass2 =
        RGB5(12, 24, 10);

    const u16 dark =
        RGB5(3, 7, 10);

    const u16 wall =
        RGB5(25, 21, 15);

    const u16 roof =
        RGB5(22, 6, 5);

    clearScreen(sky);

    /*
        Horizon
    */

    rect(
        0,
        73,
        W,
        87,
        grass
    );

    /*
        distant lighter strip
    */

    rect(
        0,
        73,
        W,
        4,
        grass2
    );

    /*
        clouds
    */

    rect(
        25 + offset / 5,
        23,
        35,
        4,
        sky2
    );

    rect(
        35 + offset / 5,
        19,
        17,
        4,
        sky2
    );

    rect(
        165 + offset / 7,
        31,
        42,
        4,
        sky2
    );

    /*
        Left house
    */

    rect(
        18,
        81,
        52,
        34,
        wall
    );

    rect(
        13,
        75,
        62,
        10,
        roof
    );

    rect(
        28,
        94,
        10,
        21,
        dark
    );

    rect(
        49,
        91,
        11,
        10,
        RGB5(8, 20, 29)
    );

    /*
        Right house
    */

    rect(
        169,
        84,
        54,
        31,
        wall
    );

    rect(
        164,
        77,
        64,
        11,
        roof
    );

    rect(
        193,
        94,
        10,
        21,
        dark
    );

    /*
        distant trees
    */

    for (int x = 80; x < 160; x += 20)
    {
        rect(
            x,
            88,
            5,
            27,
            dark
        );

        rect(
            x - 7,
            76,
            19,
            17,
            RGB5(5, 15, 6)
        );
    }

    /*
        foreground
    */

    rect(
        0,
        116,
        W,
        44,
        grass2
    );

    /*
        road
    */

    rect(
        0,
        139,
        W,
        21,
        RGB5(24, 20, 12)
    );
}

/*
    Draw PIXEL TOWN logo with shadow.
*/

static void drawTitleLogo(void)
{
    const u16 shadow =
        RGB5(4, 6, 8);

    const u16 cream =
        RGB5(31, 29, 20);

    const u16 yellow =
        RGB5(31, 24, 7);

    centeredText(
        36,
        "PIXEL",
        4,
        shadow
    );

    centeredText(
        33,
        "PIXEL",
        4,
        cream
    );

    centeredText(
        69,
        "TOWN",
        4,
        shadow
    );

    centeredText(
        66,
        "TOWN",
        4,
        yellow
    );
}

static void bayaSequence(void)
{
    drawBaya();

    REG_BLDCNT =
        BLD_BLACK |
        BLD_BG2 |
        BLD_BACKDROP;

    REG_BLDY = 16;

    /*
        Fade BAYA in.
    */

    for (int i = 16; i >= 0; i--)
    {
        REG_BLDY = i;
        waitFrames(2);
    }

    waitFrames(75);

    /*
        Fade out.
    */

    for (int i = 0; i <= 16; i++)
    {
        REG_BLDY = i;
        waitFrames(2);
    }

    REG_BLDCNT = 0;
    REG_BLDY = 0;
}

static void titleSequence(void)
{
    /*
        Slight moving background entrance.
    */

    for (int frame = 0; frame < 30; frame++)
    {
        VBlankIntrWait();

        drawTitleBackground(
            30 - frame
        );

        /*
            Logo enters from above.
        */

        if (frame > 10)
            drawTitleLogo();
    }

    drawTitleBackground(0);
    drawTitleLogo();

    fadeBlackToScene();

    int blink = 0;
    int timer = 0;

    scanKeys();

    while (1)
    {
        VBlankIntrWait();

        scanKeys();

        u16 down =
            keysDown();

        if (down & KEY_START)
            break;

        timer++;

        if (timer >= 30)
        {
            timer = 0;
            blink = !blink;
        }

        /*
            redraw only lower title area
        */

        rect(
            55,
            119,
            130,
            15,
            RGB5(12, 24, 10)
        );

        if (!blink)
        {
            centeredText(
                122,
                "PRESS START",
                1,
                RGB5(31, 31, 31)
            );
        }
    }

    fadeSceneToBlack();
}

void introRun(void)
{
    /*
        Intro uses bitmap Mode 3.
    */

    SetMode(
        MODE_3 |
        BG2_ON
    );

    REG_BLDCNT = 0;
    REG_BLDY = 0;

    bayaSequence();

    /*
        Start title from black.
    */

    REG_BLDCNT =
        BLD_BLACK |
        BLD_BG2 |
        BLD_BACKDROP;

    REG_BLDY = 16;

    titleSequence();

    clearScreen(
        RGB5(0, 0, 0)
    );

    REG_BLDCNT = 0;
    REG_BLDY = 0;
}
