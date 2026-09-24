#include <gba.h>

#define SCREEN_W 240
#define SCREEN_H 160

typedef struct {
    int x, y, w, h;
} Rect;

/* Direkter Zugriff auf den GBA Mode-3-Videospeicher */
#define VRAM ((volatile u16*)0x06000000)

static inline u16 rgb(int r, int g, int b) {
    return RGB5(r, g, b);
}

static void pixel(int x, int y, u16 color) {
    if (x >= 0 && x < SCREEN_W && y >= 0 && y < SCREEN_H) {
        VRAM[y * SCREEN_W + x] = color;
    }
}

static void rectFill(int x, int y, int w, int h, u16 color) {
    for (int yy = y; yy < y + h; yy++) {
        if (yy < 0 || yy >= SCREEN_H)
            continue;

        for (int xx = x; xx < x + w; xx++) {
            if (xx < 0 || xx >= SCREEN_W)
                continue;

            VRAM[yy * SCREEN_W + xx] = color;
        }
    }
}

static void tree(int x, int y) {
    u16 darkGreen = rgb(3, 13, 5);
    u16 green     = rgb(5, 20, 7);
    u16 light     = rgb(8, 25, 10);
    u16 trunk     = rgb(15, 8, 3);

    rectFill(x + 6, y + 15, 4, 8, trunk);

    rectFill(x + 3, y + 4, 10, 14, darkGreen);
    rectFill(x, y + 8, 16, 8, darkGreen);

    rectFill(x + 4, y + 3, 8, 13, green);
    rectFill(x + 2, y + 8, 12, 6, green);

    rectFill(x + 5, y + 5, 4, 4, light);
}

static void house(int x, int y, u16 wall, u16 roof) {
    u16 roofDark = rgb(12, 4, 3);
    u16 door     = rgb(12, 7, 3);
    u16 window   = rgb(10, 24, 29);
    u16 frame    = rgb(25, 24, 19);

    rectFill(x + 3, y + 15, 42, 29, wall);

    rectFill(x, y + 10, 48, 8, roofDark);
    rectFill(x + 4, y + 6, 40, 8, roof);
    rectFill(x + 9, y + 2, 30, 7, roof);

    rectFill(x + 7, y + 23, 10, 10, frame);
    rectFill(x + 9, y + 25, 6, 6, window);

    rectFill(x + 31, y + 23, 10, 10, frame);
    rectFill(x + 33, y + 25, 6, 6, window);

    rectFill(x + 20, y + 29, 9, 15, door);
    pixel(x + 27, y + 36, rgb(29, 23, 5));
}

static void player(int x, int y, int step) {
    u16 outline = rgb(4, 4, 6);
    u16 hair    = rgb(9, 5, 2);
    u16 skin    = rgb(30, 21, 15);
    u16 shirt   = rgb(4, 12, 26);
    u16 jeans   = rgb(5, 7, 14);
    u16 shoes   = rgb(3, 3, 4);

    /* Kopf */
    rectFill(x + 4, y, 8, 3, hair);
    rectFill(x + 2, y + 3, 12, 7, outline);
    rectFill(x + 4, y + 3, 8, 7, skin);
    rectFill(x + 4, y + 3, 8, 2, hair);

    /* Körper */
    rectFill(x + 2, y + 10, 12, 10, outline);
    rectFill(x + 4, y + 10, 8, 9, shirt);

    /* Arme */
    rectFill(x, y + 11, 4, 8, skin);
    rectFill(x + 12, y + 11, 4, 8, skin);

    /* Hose */
    rectFill(x + 3, y + 19, 10, 4, jeans);

    /* Laufanimation */
    if (step) {
        rectFill(x + 2, y + 22, 4, 7, jeans);
        rectFill(x + 10, y + 22, 4, 5, jeans);

        rectFill(x + 1, y + 28, 5, 3, shoes);
        rectFill(x + 10, y + 26, 5, 3, shoes);
    } else {
        rectFill(x + 3, y + 22, 4, 7, jeans);
        rectFill(x + 9, y + 22, 4, 7, jeans);

        rectFill(x + 2, y + 28, 5, 3, shoes);
        rectFill(x + 9, y + 28, 5, 3, shoes);
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

    if (overlap(p, house1))
        return 1;

    if (overlap(p, house2))
        return 1;

    if (overlap(p, tree1))
        return 1;

    if (overlap(p, tree2))
        return 1;

    if (overlap(p, tree3))
        return 1;

    if (overlap(p, tree4))
        return 1;

    return 0;
}

static void drawWorld(void) {
    u16 grass     = rgb(8, 22, 8);
    u16 grass2    = rgb(10, 25, 10);
    u16 road      = rgb(23, 20, 13);
    u16 roadLight = rgb(27, 24, 17);
    u16 fence     = rgb(20, 14, 7);

    /* Wiese */
    rectFill(0, 0, SCREEN_W, SCREEN_H, grass);

    for (int y = 4; y < SCREEN_H; y += 16) {
        for (int x = 5; x < SCREEN_W; x += 19) {
            rectFill(x, y, 2, 2, grass2);
        }
    }

    /* Horizontale Straße */
    rectFill(0, 68, SCREEN_W, 39, road);
    rectFill(0, 71, SCREEN_W, 2, roadLight);
    rectFill(0, 102, SCREEN_W, 2, roadLight);

    /* Vertikaler Weg */
    rectFill(103, 0, 35, SCREEN_H, road);
    rectFill(106, 0, 2, SCREEN_H, roadLight);
    rectFill(133, 0, 2, SCREEN_H, roadLight);

    /* Dorfplatz */
    rectFill(88, 59, 65, 57, road);
    rectFill(94, 65, 53, 45, roadLight);

    /* Häuser */
    house(
        18,
        18,
        rgb(27, 21, 13),
        rgb(25, 7, 5)
    );

    house(
        171,
        17,
        rgb(24, 20, 15),
        rgb(8, 12, 24)
    );

    /* Bäume */
    tree(7, 92);
    tree(30, 112);
    tree(199, 91);
    tree(216, 112);

    /* Zäune */
    for (int x = 52; x < 94; x += 8) {
        rectFill(x, 126, 3, 13, fence);
        rectFill(x, 130, 8, 3, fence);
    }

    for (int x = 151; x < 193; x += 8) {
        rectFill(x, 126, 3, 13, fence);
        rectFill(x, 130, 8, 3, fence);
    }

    /* Blumen links */
    for (int x = 62; x < 90; x += 9) {
        pixel(x, 148, rgb(31, 9, 12));
        pixel(x + 1, 148, rgb(31, 28, 7));
    }

    /* Blumen rechts */
    for (int x = 154; x < 188; x += 9) {
        pixel(x, 148, rgb(20, 10, 31));
        pixel(x + 1, 148, rgb(31, 28, 7));
    }
}

int main(void) {
    irqInit();
    irqEnable(IRQ_VBLANK);

    SetMode(MODE_3 | BG2_ON);

    int px = 112;
    int py = 118;

    int animation = 0;
    int animationTimer = 0;

    while (1) {
        VBlankIntrWait();
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

        /*
         * X und Y getrennt prüfen.
         * Dadurch kann die Figur sauber an Hindernissen
         * entlanglaufen.
         */
        if (!blocked(nx, py))
            px = nx;

        if (!blocked(px, ny))
            py = ny;

        if (moving) {
            animationTimer++;

            if (animationTimer >= 10) {
                animationTimer = 0;
                animation = !animation;
            }
        } else {
            animation = 0;
            animationTimer = 0;
        }

        drawWorld();
        player(px, py, animation);
    }

    return 0;
}
