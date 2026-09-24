#include <gba.h>

#define MAP_W 64
#define MAP_H 32
#define WORLD_W (MAP_W * 8)
#define WORLD_H (MAP_H * 8)

#define SCREEN_W 240
#define SCREEN_H 160

#define GRASS       0
#define GRASS2      1
#define ROAD        2
#define PATH        3
#define WALL        4
#define WALL_DARK   5
#define ROOF        6
#define ROOF_EDGE   7
#define DOOR        8
#define WINDOW      9
#define TREE_TOP    10
#define TREE_BOTTOM 11
#define FLOWER      12
#define FENCE       13
#define WATER       14

#define DIR_DOWN  0
#define DIR_UP    1
#define DIR_LEFT  2
#define DIR_RIGHT 3

/* =========================================================
   TILE HELPERS
   ========================================================= */

static void tilePixel(u16 *tile, int x, int y, u8 color)
{
    int p = y * 8 + x;
    int word = p >> 2;
    int shift = (p & 3) * 4;

    tile[word] &= ~(0xF << shift);
    tile[word] |= (color & 0xF) << shift;
}

static void fillTile(u16 *tile, u8 color)
{
    u16 v =
        color |
        (color << 4) |
        (color << 8) |
        (color << 12);

    for (int i = 0; i < 16; i++)
        tile[i] = v;
}

/* =========================================================
   BACKGROUND GRAPHICS
   ========================================================= */

static void makeTiles(void)
{
    u16 *t = (u16 *)CHAR_BASE_ADR(0);

    /* Grass */
    fillTile(&t[GRASS * 16], 1);

    fillTile(&t[GRASS2 * 16], 1);
    tilePixel(&t[GRASS2 * 16], 1, 2, 2);
    tilePixel(&t[GRASS2 * 16], 6, 5, 2);

    /* Road */
    fillTile(&t[ROAD * 16], 3);

    tilePixel(&t[ROAD * 16], 1, 1, 4);
    tilePixel(&t[ROAD * 16], 6, 5, 4);

    /* Footpath */
    fillTile(&t[PATH * 16], 5);

    /* Wall */
    fillTile(&t[WALL * 16], 6);

    for (int x = 0; x < 8; x++)
        tilePixel(&t[WALL * 16], x, 7, 7);

    /* Dark wall */
    fillTile(&t[WALL_DARK * 16], 7);

    /* Roof */
    fillTile(&t[ROOF * 16], 8);

    for (int y = 1; y < 8; y += 3)
    {
        for (int x = 0; x < 8; x++)
            tilePixel(&t[ROOF * 16], x, y, 9);
    }

    /* Roof edge */
    fillTile(&t[ROOF_EDGE * 16], 8);

    for (int x = 0; x < 8; x++)
    {
        tilePixel(&t[ROOF_EDGE * 16], x, 6, 9);
        tilePixel(&t[ROOF_EDGE * 16], x, 7, 9);
    }

    /* Door */
    fillTile(&t[DOOR * 16], 10);

    for (int y = 1; y < 8; y++)
    {
        tilePixel(&t[DOOR * 16], 1, y, 7);
        tilePixel(&t[DOOR * 16], 6, y, 7);
    }

    tilePixel(&t[DOOR * 16], 5, 4, 15);

    /* Window */
    fillTile(&t[WINDOW * 16], 6);

    for (int y = 1; y <= 6; y++)
    {
        for (int x = 1; x <= 6; x++)
            tilePixel(&t[WINDOW * 16], x, y, 11);
    }

    for (int x = 1; x <= 6; x++)
        tilePixel(&t[WINDOW * 16], x, 4, 12);

    tilePixel(&t[WINDOW * 16], 4, 2, 12);
    tilePixel(&t[WINDOW * 16], 4, 3, 12);
    tilePixel(&t[WINDOW * 16], 4, 4, 12);
    tilePixel(&t[WINDOW * 16], 4, 5, 12);
    tilePixel(&t[WINDOW * 16], 4, 6, 12);

    /* Tree crown */
    fillTile(&t[TREE_TOP * 16], 1);

    for (int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            int dx = x - 3;
            int dy = y - 4;

            if (dx * dx + dy * dy < 18)
            {
                tilePixel(
                    &t[TREE_TOP * 16],
                    x,
                    y,
                    ((x + y) & 1) ? 13 : 14
                );
            }
        }
    }

    /* Tree trunk/lower crown */
    fillTile(&t[TREE_BOTTOM * 16], 1);

    for (int y = 0; y < 4; y++)
    {
        for (int x = 1; x < 7; x++)
            tilePixel(&t[TREE_BOTTOM * 16], x, y, 13);
    }

    for (int y = 4; y < 8; y++)
    {
        tilePixel(&t[TREE_BOTTOM * 16], 3, y, 10);
        tilePixel(&t[TREE_BOTTOM * 16], 4, y, 10);
    }

    /* Flowers */
    fillTile(&t[FLOWER * 16], 1);
    tilePixel(&t[FLOWER * 16], 2, 3, 15);
    tilePixel(&t[FLOWER * 16], 3, 3, 12);
    tilePixel(&t[FLOWER * 16], 6, 6, 15);

    /* Fence */
    fillTile(&t[FENCE * 16], 1);

    for (int x = 0; x < 8; x++)
    {
        tilePixel(&t[FENCE * 16], x, 3, 10);
        tilePixel(&t[FENCE * 16], x, 5, 10);
    }

    tilePixel(&t[FENCE * 16], 1, 2, 10);
    tilePixel(&t[FENCE * 16], 1, 6, 10);
    tilePixel(&t[FENCE * 16], 6, 2, 10);
    tilePixel(&t[FENCE * 16], 6, 6, 10);

    /* Water */
    fillTile(&t[WATER * 16], 11);

    for (int x = 0; x < 8; x += 3)
        tilePixel(&t[WATER * 16], x, 4, 12);
}

/* =========================================================
   MAP
   ========================================================= */

static void setMap(int x, int y, int tile)
{
    if (x < 0 || y < 0 || x >= MAP_W || y >= MAP_H)
        return;

    if (x < 32)
    {
        u16 *map = (u16 *)SCREEN_BASE_BLOCK(30);
        map[y * 32 + x] = tile;
    }
    else
    {
        u16 *map = (u16 *)SCREEN_BASE_BLOCK(31);
        map[y * 32 + (x - 32)] = tile;
    }
}

static int getMap(int x, int y)
{
    if (x < 0 || y < 0 || x >= MAP_W || y >= MAP_H)
        return WALL;

    if (x < 32)
    {
        u16 *map = (u16 *)SCREEN_BASE_BLOCK(30);
        return map[y * 32 + x] & 0x3FF;
    }

    u16 *map = (u16 *)SCREEN_BASE_BLOCK(31);
    return map[y * 32 + (x - 32)] & 0x3FF;
}

/* =========================================================
   BIGGER HOUSES
   ========================================================= */

static void makeHouse(int x, int y, int width)
{
    /*
       Häuser sind jetzt ungefähr
       80-104 Pixel breit.
    */

    int right = x + width - 1;

    /* Dach - 4 Tiles hoch */
    for (int yy = y; yy < y + 3; yy++)
    {
        for (int xx = x + 1; xx < right; xx++)
            setMap(xx, yy, ROOF);
    }

    /* leichte Dachüberstände */
    for (int xx = x; xx <= right; xx++)
        setMap(xx, y + 3, ROOF_EDGE);

    /* Hausfassade - 5 Tiles hoch */
    for (int yy = y + 4; yy <= y + 8; yy++)
    {
        for (int xx = x + 1; xx < right; xx++)
            setMap(xx, yy, WALL);
    }

    /* dunkle Seiten */
    for (int yy = y + 4; yy <= y + 8; yy++)
    {
        setMap(x, yy, WALL_DARK);
        setMap(right, yy, WALL_DARK);
    }

    /* Fenster */
    setMap(x + 2, y + 5, WINDOW);
    setMap(x + 3, y + 5, WINDOW);

    setMap(right - 3, y + 5, WINDOW);
    setMap(right - 2, y + 5, WINDOW);

    /* Tür 2 Tiles breit */
    int doorX = x + width / 2 - 1;

    setMap(doorX, y + 7, DOOR);
    setMap(doorX + 1, y + 7, DOOR);

    setMap(doorX, y + 8, DOOR);
    setMap(doorX + 1, y + 8, DOOR);
}

/* =========================================================
   BIG TREES
   ========================================================= */

static void makeTree(int x, int y)
{
    /*
       16 x 24 Pixel großer Baum.
    */

    setMap(x, y, TREE_TOP);
    setMap(x + 1, y, TREE_TOP);

    setMap(x, y + 1, TREE_TOP);
    setMap(x + 1, y + 1, TREE_TOP);

    setMap(x, y + 2, TREE_BOTTOM);
    setMap(x + 1, y + 2, TREE_BOTTOM);
}

/* =========================================================
   WORLD
   ========================================================= */

static void makeWorld(void)
{
    /* Grass */
    for (int y = 0; y < MAP_H; y++)
    {
        for (int x = 0; x < MAP_W; x++)
        {
            if ((x * 7 + y * 5) % 23 == 0)
                setMap(x, y, GRASS2);
            else
                setMap(x, y, GRASS);
        }
    }

    /*
       Straßen jetzt schmaler:
       3 Tiles = 24 Pixel.
    */

    for (int y = 14; y <= 16; y++)
    {
        for (int x = 0; x < MAP_W; x++)
            setMap(x, y, ROAD);
    }

    for (int x = 30; x <= 32; x++)
    {
        for (int y = 0; y < MAP_H; y++)
            setMap(x, y, ROAD);
    }

    /*
       Große Häuser.
    */

    makeHouse(2, 3, 11);
    makeHouse(16, 3, 11);

    makeHouse(35, 3, 12);
    makeHouse(50, 3, 12);

    makeHouse(4, 20, 12);
    makeHouse(19, 20, 10);

    makeHouse(35, 20, 11);
    makeHouse(49, 20, 12);

    /*
       Wege zu Haustüren.
    */

    for (int y = 12; y <= 13; y++)
    {
        setMap(6, y, PATH);
        setMap(7, y, PATH);

        setMap(20, y, PATH);
        setMap(21, y, PATH);

        setMap(40, y, PATH);
        setMap(41, y, PATH);

        setMap(55, y, PATH);
        setMap(56, y, PATH);
    }

    /*
       Größere Bäume.
    */

    makeTree(1, 17);
    makeTree(14, 18);
    makeTree(27, 18);

    makeTree(34, 17);
    makeTree(46, 18);
    makeTree(61, 17);

    /*
       Blumen.
    */

    for (int x = 14; x <= 18; x++)
        setMap(x, 18, FLOWER);

    for (int x = 42; x <= 46; x++)
        setMap(x, 18, FLOWER);

    /*
       Zäune.
    */

    for (int x = 1; x <= 12; x++)
        setMap(x, 30, FENCE);

    for (int x = 35; x <= 46; x++)
        setMap(x, 30, FENCE);
}

/* =========================================================
   COLLISION
   ========================================================= */

static int solid(int tx, int ty)
{
    int tile = getMap(tx, ty);

    if (tile == WALL ||
        tile == WALL_DARK ||
        tile == ROOF ||
        tile == ROOF_EDGE ||
        tile == TREE_TOP ||
        tile == TREE_BOTTOM ||
        tile == FENCE ||
        tile == WATER)
        return 1;

    return 0;
}

static int blocked(int x, int y)
{
    /*
       Spieler ist 16x24 sichtbar.
       Füße sind unten.
    */

    int left = x + 4;
    int right = x + 11;

    int top = y + 19;
    int bottom = y + 23;

    if (solid(left / 8, top / 8))
        return 1;

    if (solid(right / 8, top / 8))
        return 1;

    if (solid(left / 8, bottom / 8))
        return 1;

    if (solid(right / 8, bottom / 8))
        return 1;

    return 0;
}

/* =========================================================
   PLAYER - 16x32 HARDWARE SPRITE
   sichtbare Figur etwa 16x24
   ========================================================= */

static void objPixel(u16 *base, int x, int y, u8 color)
{
    if (x < 0 || x >= 16 || y < 0 || y >= 32)
        return;

    int tileX = x >> 3;
    int tileY = y >> 3;

    int localX = x & 7;
    int localY = y & 7;

    int tileNumber = tileY * 2 + tileX;

    tilePixel(
        &base[tileNumber * 16],
        localX,
        localY,
        color
    );
}

static void clearPlayer(u16 *base)
{
    /*
       16x32 = 8 Tiles.
    */
    for (int i = 0; i < 128; i++)
        base[i] = 0;
}

static void drawPlayer(
    u16 *base,
    int direction,
    int frame
)
{
    clearPlayer(base);

    int outline = 1;
    int hair = 2;
    int skin = 3;
    int shirt = 4;
    int pants = 5;
    int shoe = 6;
    int highlight = 7;
    int shadow = 8;

    /*
       Sprite ist 32 hoch,
       Figur beginnt erst bei y=5.
    */

    /* Schatten */
    for (int x = 4; x <= 11; x++)
        objPixel(base, x, 29, shadow);

    /* Haare / Kopf */
    for (int y = 6; y <= 11; y++)
    {
        for (int x = 5; x <= 10; x++)
            objPixel(base, x, y, skin);
    }

    for (int x = 5; x <= 10; x++)
        objPixel(base, x, 5, hair);

    objPixel(base, 4, 7, hair);
    objPixel(base, 11, 7, hair);

    if (direction == DIR_DOWN)
    {
        objPixel(base, 6, 9, outline);
        objPixel(base, 9, 9, outline);
    }
    else if (direction == DIR_UP)
    {
        for (int x = 5; x <= 10; x++)
            objPixel(base, x, 8, hair);
    }
    else if (direction == DIR_LEFT)
    {
        objPixel(base, 5, 9, outline);
    }
    else
    {
        objPixel(base, 10, 9, outline);
    }

    /* Körper */
    for (int y = 13; y <= 19; y++)
    {
        for (int x = 5; x <= 10; x++)
            objPixel(base, x, y, shirt);
    }

    objPixel(base, 6, 14, highlight);
    objPixel(base, 6, 15, highlight);

    /* Arme */
    int swingA = frame ? 1 : 0;
    int swingB = frame ? 0 : 1;

    for (int y = 14; y <= 19; y++)
    {
        objPixel(base, 3, y + swingA, skin);
        objPixel(base, 4, y + swingA, skin);

        objPixel(base, 11, y + swingB, skin);
        objPixel(base, 12, y + swingB, skin);
    }

    /* Hose */
    for (int y = 20; y <= 22; y++)
    {
        for (int x = 5; x <= 10; x++)
            objPixel(base, x, y, pants);
    }

    /* Beine */
    if (frame == 0)
    {
        for (int y = 23; y <= 27; y++)
        {
            objPixel(base, 5, y, pants);
            objPixel(base, 6, y, pants);

            objPixel(base, 9, y, pants);
            objPixel(base, 10, y, pants);
        }

        objPixel(base, 4, 28, shoe);
        objPixel(base, 5, 28, shoe);
        objPixel(base, 6, 28, shoe);

        objPixel(base, 9, 28, shoe);
        objPixel(base, 10, 28, shoe);
        objPixel(base, 11, 28, shoe);
    }
    else
    {
        for (int y = 23; y <= 27; y++)
        {
            objPixel(base, 4, y, pants);
            objPixel(base, 5, y, pants);

            objPixel(base, 10, y, pants);
            objPixel(base, 11, y, pants);
        }

        objPixel(base, 3, 28, shoe);
        objPixel(base, 4, 28, shoe);
        objPixel(base, 5, 28, shoe);

        objPixel(base, 10, 28, shoe);
        objPixel(base, 11, 28, shoe);
        objPixel(base, 12, 28, shoe);
    }
}

/* =========================================================
   PALETTE
   ========================================================= */

static void palettes(void)
{
    BG_PALETTE[0] = RGB5(0,0,0);

    BG_PALETTE[1] = RGB5(8,22,8);
    BG_PALETTE[2] = RGB5(12,27,11);

    BG_PALETTE[3] = RGB5(21,18,12);
    BG_PALETTE[4] = RGB5(26,23,16);
    BG_PALETTE[5] = RGB5(23,20,14);

    BG_PALETTE[6] = RGB5(26,21,14);
    BG_PALETTE[7] = RGB5(18,13,8);

    BG_PALETTE[8] = RGB5(23,5,5);
    BG_PALETTE[9] = RGB5(29,9,7);

    BG_PALETTE[10] = RGB5(12,7,3);

    BG_PALETTE[11] = RGB5(8,19,29);
    BG_PALETTE[12] = RGB5(19,27,31);

    BG_PALETTE[13] = RGB5(3,16,5);
    BG_PALETTE[14] = RGB5(7,25,8);

    BG_PALETTE[15] = RGB5(31,27,8);

    SPRITE_PALETTE[0] = RGB5(31,0,31);
    SPRITE_PALETTE[1] = RGB5(3,3,4);
    SPRITE_PALETTE[2] = RGB5(8,4,2);
    SPRITE_PALETTE[3] = RGB5(30,21,15);
    SPRITE_PALETTE[4] = RGB5(4,11,27);
    SPRITE_PALETTE[5] = RGB5(5,7,14);
    SPRITE_PALETTE[6] = RGB5(3,3,4);
    SPRITE_PALETTE[7] = RGB5(20,22,31);
    SPRITE_PALETTE[8] = RGB5(4,12,4);
}

static int clampInt(int v, int min, int max)
{
    if (v < min)
        return min;

    if (v > max)
        return max;

    return v;
}

/* =========================================================
   MAIN
   ========================================================= */

int main(void)
{
    irqInit();
    irqEnable(IRQ_VBLANK);

    SetMode(
        MODE_0 |
        BG0_ON |
        OBJ_ON |
        OBJ_1D_MAP
    );

    palettes();

    REG_BG0CNT =
        BG_PRIORITY(1) |
        CHAR_BASE(0) |
        SCREEN_BASE(30) |
        BG_16_COLOR |
        BG_SIZE_1;

    makeTiles();
    makeWorld();

    u16 *playerTiles = (u16 *)SPRITE_GFX;

    /*
       Start mitten auf der Straße.
    */

    int playerX = 244;
    int playerY = 100;

    int direction = DIR_DOWN;
    int moving = 0;

    int walkFrame = 0;
    int walkTimer = 0;

    drawPlayer(
        playerTiles,
        direction,
        0
    );

    /*
       16x32 Sprite:
       tall + size 32.
    */

    OAM[0].attr0 =
        ATTR0_COLOR_16 |
        ATTR0_TALL;

    OAM[0].attr1 =
        ATTR1_SIZE_32;

    OAM[0].attr2 =
        ATTR2_PRIORITY(0);

    for (int i = 1; i < 128; i++)
    {
        OAM[i].attr0 = ATTR0_DISABLED;
        OAM[i].attr1 = 0;
        OAM[i].attr2 = 0;
    }

    while (1)
    {
        VBlankIntrWait();
        scanKeys();

        u16 keys = keysHeld();

        int nx = playerX;
        int ny = playerY;

        moving = 0;

        if (keys & KEY_LEFT)
        {
            nx--;
            direction = DIR_LEFT;
            moving = 1;
        }
        else if (keys & KEY_RIGHT)
        {
            nx++;
            direction = DIR_RIGHT;
            moving = 1;
        }
        else if (keys & KEY_UP)
        {
            ny--;
            direction = DIR_UP;
            moving = 1;
        }
        else if (keys & KEY_DOWN)
        {
            ny++;
            direction = DIR_DOWN;
            moving = 1;
        }

        nx = clampInt(nx, 0, WORLD_W - 16);
        ny = clampInt(ny, 0, WORLD_H - 32);

        if (!blocked(nx, playerY))
            playerX = nx;

        if (!blocked(playerX, ny))
            playerY = ny;

        if (moving)
        {
            walkTimer++;

            if (walkTimer >= 10)
            {
                walkTimer = 0;
                walkFrame ^= 1;
            }
        }
        else
        {
            walkTimer = 0;
            walkFrame = 0;
        }

        drawPlayer(
            playerTiles,
            direction,
            moving ? walkFrame : 0
        );

        /*
           Kamera
        */

        int cameraX =
            playerX + 8 - SCREEN_W / 2;

        int cameraY =
            playerY + 16 - SCREEN_H / 2;

        cameraX = clampInt(
            cameraX,
            0,
            WORLD_W - SCREEN_W
        );

        cameraY = clampInt(
            cameraY,
            0,
            WORLD_H - SCREEN_H
        );

        REG_BG0HOFS = cameraX;
        REG_BG0VOFS = cameraY;

        int screenX = playerX - cameraX;
        int screenY = playerY - cameraY;

        OAM[0].attr0 =
            ATTR0_COLOR_16 |
            ATTR0_TALL |
            (screenY & 0xFF);

        OAM[0].attr1 =
            ATTR1_SIZE_32 |
            (screenX & 0x1FF);

        OAM[0].attr2 =
            ATTR2_PRIORITY(0);
    }

    return 0;
}
