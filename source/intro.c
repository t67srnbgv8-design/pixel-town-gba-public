#include <gba.h>
#include <string.h>

#include "intro.h"

/*
    PIXEL TOWN
    Developer: BAYA

    Ablauf:

    BAYA PRESENTS
         ->
    PIXEL TOWN
         ->
    PRESS START
         ->
    Game

    WICHTIG:
    Während der sichtbaren Title-Sequenz wird
    der komplette Mode-3-Framebuffer NICHT mehr
    jedes Frame neu gezeichnet.

    Dadurch vermeiden wir das Flackern/Tearing,
    das besonders in Delta sichtbar war.
*/

#define SCREEN_W 240
#define SCREEN_H 160


/* =========================================================
   HARDWARE BLEND DEFINITIONS
   ========================================================= */

#define INTRO_BLD_BG2      (1 << 2)
#define INTRO_BLD_OBJ      (1 << 4)
#define INTRO_BLD_BACKDROP (1 << 5)

#define INTRO_BLD_BLACK    (3 << 6)

#define INTRO_FADE_TARGETS \
    (INTRO_BLD_BG2 | INTRO_BLD_OBJ | INTRO_BLD_BACKDROP)


static u16 *framebuffer = (u16 *)MODE3_FB;


/* =========================================================
   BASIC DRAWING
   ========================================================= */

static void waitFrames(int frames)
{
    int i;

    for (i = 0; i < frames; i++)
    {
        VBlankIntrWait();
    }
}


static void putPixel(
    int x,
    int y,
    u16 color
)
{
    if (x < 0)
        return;

    if (y < 0)
        return;

    if (x >= SCREEN_W)
        return;

    if (y >= SCREEN_H)
        return;

    framebuffer[
        y * SCREEN_W + x
    ] = color;
}


static void fillRect(
    int x,
    int y,
    int width,
    int height,
    u16 color
)
{
    int px;
    int py;

    for (py = y; py < y + height; py++)
    {
        for (px = x; px < x + width; px++)
        {
            putPixel(
                px,
                py,
                color
            );
        }
    }
}


static void clearScreen(u16 color)
{
    int i;

    for (i = 0; i < SCREEN_W * SCREEN_H; i++)
    {
        framebuffer[i] = color;
    }
}


/* =========================================================
   SMALL PIXEL FONT
   ========================================================= */

typedef struct
{
    char character;
    u8 rows[7];

} IntroGlyph;


static const IntroGlyph introFont[] =
{
    {
        'A',
        {
            0x0E,
            0x11,
            0x11,
            0x1F,
            0x11,
            0x11,
            0x11
        }
    },

    {
        'B',
        {
            0x1E,
            0x11,
            0x11,
            0x1E,
            0x11,
            0x11,
            0x1E
        }
    },

    {
        'E',
        {
            0x1F,
            0x10,
            0x10,
            0x1E,
            0x10,
            0x10,
            0x1F
        }
    },

    {
        'I',
        {
            0x1F,
            0x04,
            0x04,
            0x04,
            0x04,
            0x04,
            0x1F
        }
    },

    {
        'L',
        {
            0x10,
            0x10,
            0x10,
            0x10,
            0x10,
            0x10,
            0x1F
        }
    },

    {
        'N',
        {
            0x11,
            0x19,
            0x19,
            0x15,
            0x13,
            0x13,
            0x11
        }
    },

    {
        'O',
        {
            0x0E,
            0x11,
            0x11,
            0x11,
            0x11,
            0x11,
            0x0E
        }
    },

    {
        'P',
        {
            0x1E,
            0x11,
            0x11,
            0x1E,
            0x10,
            0x10,
            0x10
        }
    },

    {
        'R',
        {
            0x1E,
            0x11,
            0x11,
            0x1E,
            0x14,
            0x12,
            0x11
        }
    },

    {
        'S',
        {
            0x0F,
            0x10,
            0x10,
            0x0E,
            0x01,
            0x01,
            0x1E
        }
    },

    {
        'T',
        {
            0x1F,
            0x04,
            0x04,
            0x04,
            0x04,
            0x04,
            0x04
        }
    },

    {
        'W',
        {
            0x11,
            0x11,
            0x11,
            0x15,
            0x15,
            0x1B,
            0x11
        }
    },

    {
        'X',
        {
            0x11,
            0x11,
            0x0A,
            0x04,
            0x0A,
            0x11,
            0x11
        }
    },

    {
        'Y',
        {
            0x11,
            0x11,
            0x0A,
            0x04,
            0x04,
            0x04,
            0x04
        }
    }
};


static const IntroGlyph *findGlyph(
    char character
)
{
    unsigned int i;

    unsigned int count =
        sizeof(introFont) /
        sizeof(introFont[0]);

    for (i = 0; i < count; i++)
    {
        if (
            introFont[i].character ==
            character
        )
        {
            return &introFont[i];
        }
    }

    return 0;
}


static void drawCharacter(
    int x,
    int y,
    char character,
    int scale,
    u16 color
)
{
    const IntroGlyph *glyph;

    int row;
    int column;

    glyph = findGlyph(character);

    if (!glyph)
        return;

    for (row = 0; row < 7; row++)
    {
        for (
            column = 0;
            column < 5;
            column++
        )
        {
            if (
                glyph->rows[row] &
                (1 << (4 - column))
            )
            {
                fillRect(
                    x + column * scale,
                    y + row * scale,
                    scale,
                    scale,
                    color
                );
            }
        }
    }
}


static int getTextWidth(
    const char *text,
    int scale
)
{
    int length;

    length = strlen(text);

    if (length <= 0)
        return 0;

    return
        length * 6 * scale -
        scale;
}


static void drawText(
    int x,
    int y,
    const char *text,
    int scale,
    u16 color
)
{
    int cursorX;

    cursorX = x;

    while (*text)
    {
        if (*text != ' ')
        {
            drawCharacter(
                cursorX,
                y,
                *text,
                scale,
                color
            );
        }

        cursorX +=
            6 * scale;

        text++;
    }
}


static void drawCenteredText(
    int y,
    const char *text,
    int scale,
    u16 color
)
{
    int width;
    int x;

    width =
        getTextWidth(
            text,
            scale
        );

    x =
        (SCREEN_W - width) /
        2;

    drawText(
        x,
        y,
        text,
        scale,
        color
    );
}


/* =========================================================
   HARDWARE FADES
   ========================================================= */

static void fadeIn(void)
{
    int level;

    REG_BLDCNT =
        INTRO_FADE_TARGETS |
        INTRO_BLD_BLACK;

    /*
        Start completely black.
    */

    REG_BLDY = 16;

    for (
        level = 16;
        level >= 0;
        level--
    )
    {
        REG_BLDY = level;

        waitFrames(2);
    }

    REG_BLDCNT = 0;
    REG_BLDY = 0;
}


static void fadeOut(void)
{
    int level;

    REG_BLDCNT =
        INTRO_FADE_TARGETS |
        INTRO_BLD_BLACK;

    for (
        level = 0;
        level <= 16;
        level++
    )
    {
        REG_BLDY = level;

        waitFrames(2);
    }

    /*
        Bildschirm bleibt danach schwarz,
        bis wir den Modus wechseln.
    */
}


/* =========================================================
   BAYA DEVELOPER SCREEN
   ========================================================= */

static void drawBayaLogo(void)
{
    const u16 black =
        RGB5(0,0,0);

    const u16 white =
        RGB5(31,31,31);

    const u16 shadow =
        RGB5(8,10,13);

    const u16 blue =
        RGB5(6,18,31);

    const u16 gray =
        RGB5(19,21,24);


    clearScreen(
        black
    );


    /*
        Logo shadow
    */

    drawCenteredText(
        54,
        "BAYA",
        5,
        shadow
    );


    /*
        Main BAYA logo
    */

    drawCenteredText(
        50,
        "BAYA",
        5,
        white
    );


    /*
        Logo accent
    */

    fillRect(
        82,
        91,
        76,
        3,
        blue
    );


    /*
        PRESENTS
    */

    drawCenteredText(
        105,
        "PRESENTS",
        1,
        gray
    );
}


static void runBayaSequence(void)
{
    int level;

    /*
        Logo wird aufgebaut,
        bevor es sichtbar wird.
    */

    drawBayaLogo();


    REG_BLDCNT =
        INTRO_FADE_TARGETS |
        INTRO_BLD_BLACK;

    REG_BLDY = 16;


    /*
        Fade in
    */

    for (
        level = 16;
        level >= 0;
        level--
    )
    {
        REG_BLDY = level;

        waitFrames(2);
    }


    /*
        Logo stehen lassen.
    */

    waitFrames(70);


    /*
        Fade out
    */

    for (
        level = 0;
        level <= 16;
        level++
    )
    {
        REG_BLDY = level;

        waitFrames(2);
    }


    /*
        Schwarz halten.
    */

    clearScreen(
        RGB5(0,0,0)
    );

    waitFrames(8);
}


/* =========================================================
   TITLE BACKGROUND
   ========================================================= */

static void drawCloud(
    int x,
    int y,
    u16 color
)
{
    fillRect(
        x + 6,
        y,
        22,
        4,
        color
    );

    fillRect(
        x,
        y + 4,
        36,
        5,
        color
    );

    fillRect(
        x + 10,
        y - 3,
        13,
        4,
        color
    );
}


static void drawSmallTree(
    int x,
    int y
)
{
    const u16 trunk =
        RGB5(10,6,3);

    const u16 darkGreen =
        RGB5(3,12,5);

    const u16 green =
        RGB5(7,20,7);

    const u16 lightGreen =
        RGB5(14,27,10);


    fillRect(
        x + 9,
        y + 17,
        5,
        18,
        trunk
    );


    fillRect(
        x + 2,
        y + 5,
        19,
        18,
        darkGreen
    );


    fillRect(
        x + 5,
        y + 1,
        14,
        20,
        green
    );


    fillRect(
        x,
        y + 8,
        24,
        10,
        green
    );


    fillRect(
        x + 7,
        y + 4,
        8,
        4,
        lightGreen
    );
}


static void drawHouse(
    int x,
    int y,
    int width
)
{
    const u16 outline =
        RGB5(6,5,5);

    const u16 roofDark =
        RGB5(13,3,4);

    const u16 roof =
        RGB5(24,7,6);

    const u16 roofLight =
        RGB5(31,12,8);

    const u16 wall =
        RGB5(29,25,18);

    const u16 wallShadow =
        RGB5(20,16,11);

    const u16 window =
        RGB5(7,19,29);

    const u16 windowLight =
        RGB5(18,27,31);

    const u16 door =
        RGB5(11,7,4);


    /*
        Wall shadow
    */

    fillRect(
        x,
        y + 17,
        width,
        35,
        wallShadow
    );


    /*
        Main wall
    */

    fillRect(
        x + 2,
        y + 17,
        width - 4,
        31,
        wall
    );


    /*
        Roof outline
    */

    fillRect(
        x - 5,
        y + 8,
        width + 10,
        12,
        outline
    );


    /*
        Dark roof layer
    */

    fillRect(
        x - 3,
        y + 6,
        width + 6,
        11,
        roofDark
    );


    /*
        Main roof
    */

    fillRect(
        x,
        y + 3,
        width,
        11,
        roof
    );


    /*
        Roof highlight
    */

    fillRect(
        x + 5,
        y + 4,
        width - 10,
        3,
        roofLight
    );


    /*
        Door outline
    */

    fillRect(
        x + width / 2 - 5,
        y + 29,
        11,
        19,
        outline
    );


    /*
        Door
    */

    fillRect(
        x + width / 2 - 3,
        y + 31,
        7,
        17,
        door
    );


    /*
        Left window
    */

    fillRect(
        x + 8,
        y + 27,
        13,
        12,
        outline
    );

    fillRect(
        x + 10,
        y + 29,
        9,
        8,
        window
    );

    fillRect(
        x + 11,
        y + 30,
        4,
        3,
        windowLight
    );


    /*
        Right window
    */

    fillRect(
        x + width - 21,
        y + 27,
        13,
        12,
        outline
    );

    fillRect(
        x + width - 19,
        y + 29,
        9,
        8,
        window
    );

    fillRect(
        x + width - 18,
        y + 30,
        4,
        3,
        windowLight
    );
}


static void drawTitleBackground(
    int cloudOffset
)
{
    const u16 sky =
        RGB5(10,20,30);

    const u16 cloud =
        RGB5(24,29,31);

    const u16 distantGrass =
        RGB5(8,18,8);

    const u16 grass =
        RGB5(12,25,10);

    const u16 grassLight =
        RGB5(16,28,12);

    const u16 road =
        RGB5(24,20,12);

    const u16 roadLight =
        RGB5(29,25,17);


    clearScreen(
        sky
    );


    /*
        Clouds
    */

    drawCloud(
        18 + cloudOffset,
        22,
        cloud
    );

    drawCloud(
        168 + cloudOffset / 2,
        31,
        cloud
    );


    /*
        Background grass
    */

    fillRect(
        0,
        72,
        SCREEN_W,
        15,
        distantGrass
    );


    /*
        Main grass
    */

    fillRect(
        0,
        87,
        SCREEN_W,
        73,
        grass
    );


    fillRect(
        0,
        87,
        SCREEN_W,
        3,
        grassLight
    );


    /*
        Houses
    */

    drawHouse(
        10,
        71,
        61
    );

    drawHouse(
        169,
        74,
        61
    );


    /*
        Trees
    */

    drawSmallTree(
        79,
        78
    );

    drawSmallTree(
        105,
        75
    );

    drawSmallTree(
        132,
        79
    );


    /*
        Foreground road
    */

    fillRect(
        0,
        139,
        SCREEN_W,
        21,
        road
    );

    fillRect(
        0,
        139,
        SCREEN_W,
        3,
        roadLight
    );
}


/* =========================================================
   PIXEL TOWN LOGO
   ========================================================= */

static void drawTitleLogo(
    int yOffset
)
{
    const u16 deepShadow =
        RGB5(2,4,6);

    const u16 border =
        RGB5(8,10,13);

    const u16 cream =
        RGB5(31,30,22);

    const u16 gold =
        RGB5(31,23,6);


    /*
        PIXEL shadow
    */

    drawCenteredText(
        18 + yOffset,
        "PIXEL",
        4,
        deepShadow
    );


    /*
        PIXEL border
    */

    drawCenteredText(
        16 + yOffset,
        "PIXEL",
        4,
        border
    );


    /*
        PIXEL face
    */

    drawCenteredText(
        14 + yOffset,
        "PIXEL",
        4,
        cream
    );


    /*
        TOWN shadow
    */

    drawCenteredText(
        51 + yOffset,
        "TOWN",
        4,
        deepShadow
    );


    /*
        TOWN face
    */

    drawCenteredText(
        48 + yOffset,
        "TOWN",
        4,
        gold
    );
}


/* =========================================================
   PRESS START
   ========================================================= */

static void drawPressStart(
    int visible
)
{
    /*
        Löscht nur den kleinen Bereich des Prompts.
        Kein Fullscreen-Redraw.
    */

    fillRect(
        60,
        121,
        120,
        12,
        RGB5(12,25,10)
    );


    if (visible)
    {
        /*
            Text shadow
        */

        drawCenteredText(
            124,
            "PRESS START",
            1,
            RGB5(3,5,4)
        );


        /*
            Main text
        */

        drawCenteredText(
            122,
            "PRESS START",
            1,
            RGB5(31,31,31)
        );
    }
}


/* =========================================================
   TITLE SEQUENCE
   ========================================================= */

static void runTitleSequence(void)
{
    int blinkTimer;
    int promptVisible;

    u16 keys;


    /*
        -----------------------------------------------------
        FLICKER FIX
        -----------------------------------------------------

        Der Bildschirm bleibt komplett schwarz,
        während die gesamte Titelszene in VRAM
        aufgebaut wird.

        Früher wurde hier der komplette Mode-3-
        Framebuffer 24 Frames lang neu gezeichnet.

        Genau das konnte in Delta sichtbares
        Tearing/Flackern verursachen.
    */

    REG_BLDCNT =
        INTRO_FADE_TARGETS |
        INTRO_BLD_BLACK;

    REG_BLDY = 16;


    /*
        Komplette fertige Szene EINMAL zeichnen.
    */

    drawTitleBackground(0);

    drawTitleLogo(0);

    drawPressStart(1);


    /*
        Jetzt erst wird das fertige Bild sichtbar.
    */

    fadeIn();


    /*
        PRESS START loop
    */

    blinkTimer = 0;
    promptVisible = 1;

    scanKeys();


    while (1)
    {
        VBlankIntrWait();

        scanKeys();

        keys = keysDown();


        /*
            START pressed.
        */

        if (keys & KEY_START)
        {
            break;
        }


        /*
            Nur der kleine Prompt-Bereich
            wird alle 30 Frames geändert.

            Der Hintergrund wird NICHT
            erneut gezeichnet.
        */

        blinkTimer++;

        if (blinkTimer >= 30)
        {
            blinkTimer = 0;

            promptVisible =
                !promptVisible;

            drawPressStart(
                promptVisible
            );
        }
    }


    /*
        Prompt wieder sichtbar machen,
        bevor ausgeblendet wird.
    */

    drawPressStart(1);

    waitFrames(6);


    /*
        Fade to black.
    */

    fadeOut();


    /*
        Sicher komplett schwarz machen.
    */

    clearScreen(
        RGB5(0,0,0)
    );


    /*
        Blend-Hardware zurücksetzen.
    */

    REG_BLDCNT = 0;
    REG_BLDY = 0;
}


/* =========================================================
   PUBLIC INTRO ENTRY
   ========================================================= */

void introRun(void)
{
    /*
        Intro verwendet Mode 3.

        Das eigentliche Spiel stellt danach
        wieder auf Mode 0 um.
    */

    SetMode(
        MODE_3 |
        BG2_ON
    );


    /*
        Clean hardware state.
    */

    REG_BLDCNT = 0;
    REG_BLDY = 0;


    /*
        Boot starts black.
    */

    clearScreen(
        RGB5(0,0,0)
    );

    waitFrames(8);


    /*
        Developer intro.
    */

    runBayaSequence();


    /*
        Pixel Town title screen.
    */

    runTitleSequence();


    /*
        Hinterlasse einen sauberen schwarzen
        Bildschirm für main.c.

        main.c schaltet anschließend wieder
        auf MODE_0 und initialisiert Tiles,
        Maps und Sprites neu.
    */

    REG_BLDCNT = 0;
    REG_BLDY = 0;

    clearScreen(
        RGB5(0,0,0)
    );

    VBlankIntrWait();
}
