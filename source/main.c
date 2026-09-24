#include <gba.h>

#define SCREEN_W 240
#define SCREEN_H 160

typedef struct {
    int x, y, w, h;
} Rect;

/* Mode 4: zwei Framebuffer-Seiten */
static volatile u16 *backBuffer = (volatile u16 *)0x0600A000;
static int showingPage = 0;

static inline u8 rgb8(int r, int g, int b) {
    /* 8-Bit Palettenindex wird separat vergeben */
    return 0;
}

static void putPixel(int x, int y, u8 color) {
    if (x < 0 || x >= SCREEN_W || y < 0 || y >= SCREEN_H)
        return;

    /*
     * Mode 4 speichert zwei 8-Bit Pixel in einem u16.
     */
    int index = y * SCREEN_W + x;
    int halfword = index >> 1;

    u16 old = backBuffer[halfword];

    if (x & 1)
        old = (old & 0x00FF) | ((u16)color << 8);
    else
        old = (old & 0xFF00) | color;

    backBuffer[halfword] = old;
}

static void rectFill(int x, int y, int w, int h, u8 color) {
    for (int yy = y; yy < y + h; yy++) {
        if (yy < 0 || yy >= SCREEN_H)
            continue;

        for (int xx = x; xx < x + w; xx++) {
            if (xx < 0 || xx >= SCREEN_W)
                continue;

            putPixel(xx, yy, color);
        }
    }
}

static void clearScreen(u8 color) {
    u16 packed = color | ((u16)color << 8);

    for (int i = 0; i < (SCREEN_W * SCREEN_H) / 2; i++)
        backBuffer[i] = packed;
}

static void tree(int x, int y) {
    rectFill(x + 6, y + 15, 4, 8, 6);

    rectFill(x + 3, y + 4, 10, 14, 3);
    rectFill(x, y + 8, 16, 8, 3);

    rectFill(x + 4, y + 3, 8, 13, 4);
    rectFill(x + 2, y + 8, 12, 6, 4);

    rectFill(x + 5, y + 5, 4, 4, 5);
}

static void house(int x, int y, int style) {
    u8 wall = style ? 15 : 12;
    u8 roof = style ? 14 : 10;

    rectFill(x + 3, y + 15, 42, 29, wall);

    rectFill(x, y + 10, 48, 8, 9);
    rectFill(x + 4, y + 6, 40, 8, roof);
    rectFill(x + 9, y + 2, 30, 7, roof);

    /* Fenster */
    rectFill(x + 7, y + 23, 10, 10, 16);
    rectFill(x + 9, y + 25, 6, 6, 17);

    rectFill(x + 31, y + 23, 10, 10, 16);
    rectFill(x + 33, y + 25, 6, 6, 17);

    /* Tür */
    rectFill(x + 20, y + 29, 9, 15, 11);
    putPixel(x + 27, y + 36, 18);
}

static void player(int x, int y, int step) {
    /* Haare */
    rectFill(x + 4, y, 8, 3, 20);

    /* Kopf-Rand */
    rectFill(x + 2, y + 3, 12, 7, 19);

    /* Gesicht */
    rectFill(x + 4, y + 3, 8, 7, 21);
    rectFill(x + 4, y + 3, 8, 2, 20);

    /* Körper */
    rectFill(x + 2, y + 10, 12, 10, 19);
    rectFill(x + 4, y + 10, 8, 9, 22);

    /* Arme */
    rectFill(x, y + 11, 4, 8, 21);
    rectFill(x + 12, y + 11, 4, 8, 21);

    /* Hose */
    rectFill(x + 3, y + 19, 10, 4, 23);

    if (step) {
        rectFill(x + 2, y + 22, 4, 7, 23);
        rectFill(x + 10, y + 22, 4, 5, 23);

        rectFill(x + 1, y + 28, 5, 3, 19);
        rectFill(x + 10, y + 26, 5, 3, 19);
    } else {
        rectFill(x + 3, y + 22, 4, 7, 23);
        rectFill(x + 9, y + 22, 4, 7, 23);

        rectFill(x + 2, y + 28, 5, 3, 19);
        rectFill(x + 9, y + 28, 5, 3, 19);
    }
}

static int overlap(Rect a, Rect b) {
    return
        a.x < b.x + b.w &&
        a.x + a.w > b.x &&
        a.y < b.y + b.h &&
        a.y + a.h > b.y;
}

static int blocked(int x, int y) {
    Rect p = {x + 2, y + 16, 12, 15};

    Rect house1 = {18, 18, 48, 44};
    Rect house2 = {171, 17, 48, 44};

    Rect tree1 = {7, 92, 16, 23};
    Rect tree2 = {30, 112, 16, 23};
    Rect tree3 = {199, 91, 16, 23};
    Rect tree4 = {216, 112, 16, 23};

    if (x < 0 || x > SCREEN_W - 16)
        return 1;

    if (y < 0 || y > SCREEN_H - 31)
        return 1;

    if (overlap(p, house1)) return 1;
    if (overlap(p, house2)) return 1;

    if (overlap(p, tree1)) return 1;
    if (overlap(p, tree2)) return 1;
    if (overlap(p, tree3)) return 1;
    if (overlap(p, tree4)) return 1;

    return 0;
}

static void drawWorld(void) {
    clearScreen(1);

    /* Gras-Struktur */
    for (int y = 4; y < SCREEN_H; y += 16) {
        for (int x = 5; x < SCREEN_W; x += 19) {
            rectFill(x, y, 2, 2, 2);
        }
    }

    /* Horizontale Straße */
    rectFill(0, 68, SCREEN_W, 39, 7);
    rectFill(0, 71, SCREEN_W, 2, 8);
    rectFill(0, 102, SCREEN_W, 2, 8);

    /* Vertikale Straße */
    rectFill(103, 0, 35, SCREEN_H, 7);
    rectFill(106, 0, 2, SCREEN_H, 8);
    rectFill(133, 0, 2, SCREEN_H, 8);

    /* Dorfplatz */
    rectFill(88, 59, 65, 57, 7);
    rectFill(94, 65, 53, 45, 8);

    /* Häuser */
    house(18, 18, 0);
    house(171, 17, 1);

    /* Bäume */
    tree(7, 92);
    tree(30, 112);
    tree(199, 91);
    tree(216, 112);

    /* Zäune */
    for (int x = 52; x < 94; x += 8) {
        rectFill(x, 126, 3, 13, 6);
        rectFill(x, 130, 8, 3, 6);
    }

    for (int x = 151; x < 193; x += 8) {
        rectFill(x, 126, 3, 13, 6);
        rectFill(x, 130, 8, 3, 6);
    }

    /* Blumen */
    for (int x = 62; x < 90; x += 9) {
        putPixel(x, 148, 24);
        putPixel(x + 1, 148, 25);
    }

    for (int x = 154; x < 188; x += 9) {
        putPixel(x, 148, 26);
        putPixel(x + 1, 148, 25);
    }
}

static void flip(void) {
    VBlankIntrWait();

    if (showingPage == 0) {
        REG_DISPCNT |= BACKBUFFER;
        showingPage = 1;
        backBuffer = (volatile u16 *)0x06000000;
    } else {
        REG_DISPCNT &= ~BACKBUFFER;
        showingPage = 0;
        backBuffer = (volatile u16 *)0x0600A000;
    }
}

static void setupPalette(void) {
    BG_PALETTE[0]  = RGB5(0, 0, 0);

    /* Landschaft */
    BG_PALETTE[1]  = RGB5(8, 22, 8);
    BG_PALETTE[2]  = RGB5(10, 25, 10);

    /* Bäume */
    BG_PALETTE[3]  = RGB5(3, 13, 5);
    BG_PALETTE[4]  = RGB5(5, 20, 7);
    BG_PALETTE[5]  = RGB5(10, 27, 11);

    /* Holz */
    BG_PALETTE[6]  = RGB5(16, 10, 4);

    /* Wege */
    BG_PALETTE[7]  = RGB5(22, 19, 12);
    BG_PALETTE[8]  = RGB5(27, 24, 17);

    /* Häuser */
    BG_PALETTE[9]  = RGB5(12, 4, 3);
    BG_PALETTE[10] = RGB5(26, 7, 5);
    BG_PALETTE[11] = RGB5(12, 7, 3);
    BG_PALETTE[12] = RGB5(27, 21, 13);

    BG_PALETTE[14] = RGB5(8, 12, 25);
    BG_PALETTE[15] = RGB5(24, 20, 15);

    BG_PALETTE[16] = RGB5(26, 25, 20);
    BG_PALETTE[17] = RGB5(10, 24, 29);
    BG_PALETTE[18] = RGB5(30, 24, 5);

    /* Spieler */
    BG_PALETTE[19] = RGB5(4, 4, 6);
    BG_PALETTE[20] = RGB5(9, 5, 2);
    BG_PALETTE[21] = RGB5(30, 21, 15);
    BG_PALETTE[22] = RGB5(4, 12, 27);
    BG_PALETTE[23] = RGB5(5, 7, 14);

    /* Blumen */
    BG_PALETTE[24] = RGB5(31, 8, 11);
    BG_PALETTE[25] = RGB5(31, 28, 6);
    BG_PALETTE[26] = RGB5(20, 9, 31);
}

int main(void) {
    irqInit();
    irqEnable(IRQ_VBLANK);

    /*
     * Mode 4 = 240x160, 256 Farben, zwei Bildseiten.
     */
    SetMode(MODE_4 | BG2_ON);

    setupPalette();

    int px = 112;
    int py = 118;

    int animation = 0;
    int animationTimer = 0;

    while (1) {
        scanKeys();

        u16 keys = keysHeld();

        int nx = px;
        int ny = py;
        int moving = 0;

        if (keys & KEY_LEFT) {
            nx--;
            moving = 1;
        }

        if (keys & KEY_RIGHT) {
            nx++;
            moving = 1;
        }

        if (keys & KEY_UP) {
            ny--;
            moving = 1;
        }

        if (keys & KEY_DOWN) {
            ny++;
            moving = 1;
        }

        if (!blocked(nx, py))
            px = nx;

        if (!blocked(px, ny))
            py = ny;

        if (moving) {
            animationTimer++;

            if (animationTimer >= 8) {
                animationTimer = 0;
                animation = !animation;
            }
        } else {
            animation = 0;
            animationTimer = 0;
        }

        /*
         * Alles wird zuerst auf die unsichtbare Seite gezeichnet.
         */
        drawWorld();
        player(px, py, animation);

        /*
         * Erst das komplett fertige Bild anzeigen.
         */
        flip();
    }

    return 0;
}
