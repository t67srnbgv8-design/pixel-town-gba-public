#include <gba.h>

#define MAP_W 64
#define MAP_H 32

#define WORLD_W (MAP_W * 8)
#define WORLD_H (MAP_H * 8)

#define SCREEN_W 240
#define SCREEN_H 160

#define TILE_GRASS      0
#define TILE_GRASS_DOT  1
#define TILE_ROAD       2
#define TILE_WALL       3
#define TILE_ROOF       4
#define TILE_DOOR       5
#define TILE_TREE       6
#define TILE_FLOWER     7
#define TILE_WATER      8
#define TILE_PATH       9

#define DIR_DOWN  0
#define DIR_UP    1
#define DIR_LEFT  2
#define DIR_RIGHT 3

/* =========================================================
   TILE HELPERS
   ========================================================= */

static void setTilePixel(u16 *tile, int x, int y, u8 color)
{
    int pixel = y * 8 + x;
    int word = pixel >> 2;
    int shift = (pixel & 3) * 4;

    tile[word] &= ~(0xF << shift);
    tile[word] |= (color & 0xF) << shift;
}

static void fillTile(u16 *tile, u8 color)
{
    u16 value =
        color |
        (color << 4) |
        (color << 8) |
        (color << 12);

    for (int i = 0; i < 16; i++)
        tile[i] = value;
}

/* =========================================================
   BACKGROUND TILES
   ========================================================= */

static void createBackgroundTiles(void)
{
    u16 *tiles = (u16 *)CHAR_BASE_ADR(0);

    /* Gras */
    fillTile(&tiles[TILE_GRASS * 16], 1);

    /* Gras mit Details */
    fillTile(&tiles[TILE_GRASS_DOT * 16], 1);
    setTilePixel(&tiles[TILE_GRASS_DOT * 16], 2, 2, 2);
    setTilePixel(&tiles[TILE_GRASS_DOT * 16], 6, 5, 2);

    /* Straße */
    fillTile(&tiles[TILE_ROAD * 16], 3);

    for (int x = 0; x < 8; x += 4)
        setTilePixel(&tiles[TILE_ROAD * 16], x, 4, 4);

    /* Hauswand */
    fillTile(&tiles[TILE_WALL * 16], 5);

    for (int x = 0; x < 8; x++)
        setTilePixel(&tiles[TILE_WALL * 16], x, 7, 6);

    /* Dach */
    fillTile(&tiles[TILE_ROOF * 16], 7);

    for (int x = 0; x < 8; x += 2)
        setTilePixel(&tiles[TILE_ROOF * 16], x, 2, 8);

    /* Tür */
    fillTile(&tiles[TILE_DOOR * 16], 9);
    setTilePixel(&tiles[TILE_DOOR * 16], 6, 4, 10);

    /* Baum */
    fillTile(&tiles[TILE_TREE * 16], 1);

    for (int y = 0; y < 6; y++)
    {
        for (int x = 1; x < 7; x++)
        {
            setTilePixel(
                &tiles[TILE_TREE * 16],
                x,
                y,
                ((x + y) & 1) ? 11 : 12
            );
        }
    }

    setTilePixel(&tiles[TILE_TREE * 16], 3, 6, 13);
    setTilePixel(&tiles[TILE_TREE * 16], 4, 6, 13);
    setTilePixel(&tiles[TILE_TREE * 16], 3, 7, 13);
    setTilePixel(&tiles[TILE_TREE * 16], 4, 7, 13);

    /* Blumen */
    fillTile(&tiles[TILE_FLOWER * 16], 1);

    setTilePixel(&tiles[TILE_FLOWER * 16], 2, 3, 14);
    setTilePixel(&tiles[TILE_FLOWER * 16], 3, 3, 15);

    /* Wasser */
    fillTile(&tiles[TILE_WATER * 16], 10);

    for (int x = 0; x < 8; x += 3)
        setTilePixel(&tiles[TILE_WATER * 16], x, 3, 15);

    /* kleiner Weg */
    fillTile(&tiles[TILE_PATH * 16], 4);
}

/* =========================================================
   64x32 MAP

   BG_SIZE_1 = 64x32 Tiles.
   Dafür werden zwei Screenblocks benutzt:
   Block 30 = linke Hälfte
   Block 31 = rechte Hälfte.
   ========================================================= */

static void setMapTile(int x, int y, int tile)
{
    if (x < 0 || y < 0 || x >= MAP_W || y >= MAP_H)
        return;

    u16 *map;

    if (x < 32)
    {
        map = (u16 *)SCREEN_BASE_BLOCK(30);
        map[y * 32 + x] = tile;
    }
    else
    {
        map = (u16 *)SCREEN_BASE_BLOCK(31);
        map[y * 32 + (x - 32)] = tile;
    }
}

static int getMapTile(int x, int y)
{
    if (x < 0 || y < 0 || x >= MAP_W || y >= MAP_H)
        return TILE_WALL;

    u16 *map;

    if (x < 32)
    {
        map = (u16 *)SCREEN_BASE_BLOCK(30);
        return map[y * 32 + x] & 0x3FF;
    }

    map = (u16 *)SCREEN_BASE_BLOCK(31);

    return map[y * 32 + (x - 32)] & 0x3FF;
}

/* =========================================================
   WORLD BUILDING
   ========================================================= */

static void house(int x, int y)
{
    for (int xx = x; xx < x + 6; xx++)
    {
        setMapTile(xx, y, TILE_ROOF);
        setMapTile(xx, y + 1, TILE_ROOF);
    }

    for (int yy = y + 2; yy < y + 6; yy++)
    {
        for (int xx = x; xx < x + 6; xx++)
            setMapTile(xx, yy, TILE_WALL);
    }

    setMapTile(x + 2, y + 5, TILE_DOOR);
}

static void createWorld(void)
{
    /* komplette Welt = Gras */

    for (int y = 0; y < MAP_H; y++)
    {
        for (int x = 0; x < MAP_W; x++)
        {
            if ((x * 3 + y * 7) % 17 == 0)
                setMapTile(x, y, TILE_GRASS_DOT);
            else
                setMapTile(x, y, TILE_GRASS);
        }
    }

    /* große Ost-West-Straße */

    for (int y = 13; y <= 17; y++)
    {
        for (int x = 0; x < MAP_W; x++)
            setMapTile(x, y, TILE_ROAD);
    }

    /* Nord-Süd-Straße */

    for (int x = 28; x <= 32; x++)
    {
        for (int y = 0; y < MAP_H; y++)
            setMapTile(x, y, TILE_ROAD);
    }

    /* Nebenstraße */

    for (int x = 44; x <= 47; x++)
    {
        for (int y = 12; y < MAP_H; y++)
            setMapTile(x, y, TILE_PATH);
    }

    /* Häuser */

    house(5, 4);
    house(15, 4);
    house(36, 4);
    house(49, 4);

    house(8, 21);
    house(20, 22);
    house(36, 21);
    house(51, 21);

    /* Park links unten */

    for (int x = 2; x < 7; x++)
    {
        setMapTile(x, 22, TILE_TREE);
        setMapTile(x, 28, TILE_TREE);
    }

    setMapTile(2, 24, TILE_TREE);
    setMapTile(2, 26, TILE_TREE);

    setMapTile(6, 24, TILE_TREE);
    setMapTile(6, 26, TILE_TREE);

    for (int x = 3; x <= 5; x++)
    {
        setMapTile(x, 24, TILE_FLOWER);
        setMapTile(x, 26, TILE_FLOWER);
    }

    /* kleiner See rechts */

    for (int y = 21; y <= 27; y++)
    {
        for (int x = 58; x <= 62; x++)
            setMapTile(x, y, TILE_WATER);
    }

    /* Bäume am oberen Rand */

    for (int x = 1; x < MAP_W - 1; x += 4)
        setMapTile(x, 1, TILE_TREE);

    /* einzelne Bäume */

    setMapTile(12, 9, TILE_TREE);
    setMapTile(24, 8, TILE_TREE);
    setMapTile(40, 9, TILE_TREE);
    setMapTile(56, 9, TILE_TREE);

    setMapTile(13, 19, TILE_TREE);
    setMapTile(33, 20, TILE_TREE);
    setMapTile(42, 25, TILE_TREE);
}

/* =========================================================
   COLLISION
   ========================================================= */

static int solidTile(int tx, int ty)
{
    int tile = getMapTile(tx, ty);

    if (tile == TILE_WALL)
        return 1;

    if (tile == TILE_ROOF)
        return 1;

    if (tile == TILE_TREE)
        return 1;

    if (tile == TILE_WATER)
        return 1;

    return 0;
}

static int blocked(int worldX, int worldY)
{
    /*
       Spielerposition bezeichnet oben links
       vom 32x32 Sprite.

       Die sichtbare Figur sitzt ungefähr
       in der Mitte.

       Nur Füße kollidieren.
    */

    int left   = worldX + 11;
    int right  = worldX + 20;

    int top    = worldY + 25;
    int bottom = worldY + 29;

    if (solidTile(left / 8, top / 8))
        return 1;

    if (solidTile(right / 8, top / 8))
        return 1;

    if (solidTile(left / 8, bottom / 8))
        return 1;

    if (solidTile(right / 8, bottom / 8))
        return 1;

    return 0;
}

/* =========================================================
   PLAYER SPRITE
   ========================================================= */

static void objPixel(
    u16 *base,
    int x,
    int y,
    u8 color
)
{
    if (x < 0 || x >= 32 || y < 0 || y >= 32)
        return;

    int tileX = x >> 3;
    int tileY = y >> 3;

    int localX = x & 7;
    int localY = y & 7;

    int tileNumber =
        tileY * 4 + tileX;

    setTilePixel(
        &base[tileNumber * 16],
        localX,
        localY,
        color
    );
}

static void clearPlayer(u16 *base)
{
    for (int i = 0; i < 256; i++)
        base[i] = 0;
}

static void makePlayerFrame(
    u16 *base,
    int direction,
    int frame
)
{
    clearPlayer(base);

    const int outline = 1;
    const int hair    = 2;
    const int skin    = 3;
    const int shirt   = 4;
    const int pants   = 5;
    const int shoes   = 6;
    const int light   = 7;
    const int shadow  = 8;

    /* Schatten */

    for (int x = 11; x <= 20; x++)
        objPixel(base, x, 30, shadow);

    /* Kopf */

    for (int y = 4; y <= 11; y++)
    {
        for (int x = 12; x <= 19; x++)
            objPixel(base, x, y, skin);
    }

    /* Haare */

    for (int x = 12; x <= 19; x++)
    {
        objPixel(base, x, 3, hair);
        objPixel(base, x, 4, hair);
    }

    objPixel(base, 11, 5, hair);
    objPixel(base, 20, 5, hair);

    /* Gesicht */

    if (direction == DIR_DOWN)
    {
        objPixel(base, 14, 8, outline);
        objPixel(base, 18, 8, outline);
    }
    else if (direction == DIR_UP)
    {
        for (int x = 12; x <= 19; x++)
            objPixel(base, x, 7, hair);
    }
    else if (direction == DIR_LEFT)
    {
        objPixel(base, 13, 8, outline);
    }
    else
    {
        objPixel(base, 18, 8, outline);
    }

    /* Hals */

    objPixel(base, 15, 12, skin);
    objPixel(base, 16, 12, skin);

    /* Oberkörper */

    for (int y = 13; y <= 21; y++)
    {
        for (int x = 12; x <= 19; x++)
            objPixel(base, x, y, shirt);
    }

    objPixel(base, 13, 15, light);
    objPixel(base, 13, 16, light);
    objPixel(base, 13, 17, light);

    /* Arme */

    int armA = frame ? 1 : 0;
    int armB = frame ? 0 : 1;

    for (int y = 14; y <= 20; y++)
    {
        objPixel(base, 10, y + armA, skin);
        objPixel(base, 11, y + armA, skin);

        objPixel(base, 20, y + armB, skin);
        objPixel(base, 21, y + armB, skin);
    }

    /* Hose */

    for (int y = 22; y <= 24; y++)
    {
        for (int x = 12; x <= 19; x++)
            objPixel(base, x, y, pants);
    }

    /* Beine */

    if (frame == 0)
    {
        for (int y = 25; y <= 28; y++)
        {
            objPixel(base, 12, y, pants);
            objPixel(base, 13, y, pants);
            objPixel(base, 14, y, pants);

            objPixel(base, 17, y, pants);
            objPixel(base, 18, y, pants);
            objPixel(base, 19, y, pants);
        }

        objPixel(base, 11, 29, shoes);
        objPixel(base, 12, 29, shoes);
        objPixel(base, 13, 29, shoes);
        objPixel(base, 14, 29, shoes);

        objPixel(base, 17, 29, shoes);
        objPixel(base, 18, 29, shoes);
        objPixel(base, 19, 29, shoes);
        objPixel(base, 20, 29, shoes);
    }
    else
    {
        for (int y = 25; y <= 28; y++)
        {
            objPixel(base, 11, y, pants);
            objPixel(base, 12, y, pants);
            objPixel(base, 13, y, pants);

            objPixel(base, 18, y, pants);
            objPixel(base, 19, y, pants);
            objPixel(base, 20, y, pants);
        }

        objPixel(base, 10, 29, shoes);
        objPixel(base, 11, 29, shoes);
        objPixel(base, 12, 29, shoes);
        objPixel(base, 13, 29, shoes);

        objPixel(base, 18, 29, shoes);
        objPixel(base, 19, 29, shoes);
        objPixel(base, 20, 29, shoes);
        objPixel(base, 21, 29, shoes);
    }
}

/* =========================================================
   PALETTES
   ========================================================= */

static void setupPalettes(void)
{
    BG_PALETTE[0]  = RGB5(0, 0, 0);

    BG_PALETTE[1]  = RGB5(8, 22, 8);
    BG_PALETTE[2]  = RGB5(12, 27, 11);

    BG_PALETTE[3]  = RGB5(22, 19, 12);
    BG_PALETTE[4]  = RGB5(27, 23, 16);

    BG_PALETTE[5]  = RGB5(26, 21, 14);
    BG_PALETTE[6]  = RGB5(20, 15, 9);

    BG_PALETTE[7]  = RGB5(24, 6, 5);
    BG_PALETTE[8]  = RGB5(30, 11, 8);

    BG_PALETTE[9]  = RGB5(12, 7, 3);
    BG_PALETTE[10] = RGB5(5, 15, 26);

    BG_PALETTE[11] = RGB5(4, 17, 5);
    BG_PALETTE[12] = RGB5(7, 24, 8);

    BG_PALETTE[13] = RGB5(15, 9, 4);
    BG_PALETTE[14] = RGB5(31, 8, 11);
    BG_PALETTE[15] = RGB5(12, 23, 31);

    SPRITE_PALETTE[0] = RGB5(31, 0, 31);
    SPRITE_PALETTE[1] = RGB5(3, 3, 4);
    SPRITE_PALETTE[2] = RGB5(8, 4, 2);
    SPRITE_PALETTE[3] = RGB5(30, 21, 15);
    SPRITE_PALETTE[4] = RGB5(4, 11, 27);
    SPRITE_PALETTE[5] = RGB5(5, 7, 14);
    SPRITE_PALETTE[6] = RGB5(3, 3, 4);
    SPRITE_PALETTE[7] = RGB5(20, 22, 31);
    SPRITE_PALETTE[8] = RGB5(4, 12, 4);
}

/* =========================================================
   CAMERA
   ========================================================= */

static int clampInt(int value, int min, int max)
{
    if (value < min)
        return min;

    if (value > max)
        return max;

    return value;
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

    setupPalettes();

    /*
       BG_SIZE_1 = 512x256 Pixel / 64x32 Tiles.
       Screenblock 30 + 31.
    */

    REG_BG0CNT =
        BG_PRIORITY(1) |
        CHAR_BASE(0) |
        SCREEN_BASE(30) |
        BG_16_COLOR |
        BG_SIZE_1;

    createBackgroundTiles();
    createWorld();

    u16 *playerTiles =
        (u16 *)SPRITE_GFX;

    /*
       Spieler-WELTPOSITION.
       Nicht Bildschirmposition.
    */

    int playerX = 240;
    int playerY = 104;

    int direction = DIR_DOWN;

    int moving = 0;
    int walkFrame = 0;
    int walkTimer = 0;

    makePlayerFrame(
        playerTiles,
        direction,
        0
    );

    /* Andere Sprites deaktivieren */

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

        /*
           1 Pixel / Frame
        */

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

        /*
           Weltgrenzen zuerst.
        */

        nx = clampInt(
            nx,
            -8,
            WORLD_W - 24
        );

        ny = clampInt(
            ny,
            -4,
            WORLD_H - 32
        );

        /*
           Kollision
        */

        if (!blocked(nx, playerY))
            playerX = nx;

        if (!blocked(playerX, ny))
            playerY = ny;

        /*
           Animation
        */

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

        makePlayerFrame(
            playerTiles,
            direction,
            moving ? walkFrame : 0
        );

        /*
           Kamera folgt der Figur.

           Spieler ungefähr in Bildschirmmitte.
        */

        int cameraX =
            playerX + 16 - SCREEN_W / 2;

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

        /*
           Hardware-Scrolling.
        */

        REG_BG0HOFS = cameraX;
        REG_BG0VOFS = cameraY;

        /*
           Weltposition -> Bildschirmposition.
        */

        int screenX =
            playerX - cameraX;

        int screenY =
            playerY - cameraY;

        /*
           Hardware-Sprite aktualisieren.
        */

        OAM[0].attr0 =
            ATTR0_COLOR_16 |
            ATTR0_SQUARE |
            (screenY & 0xFF);

        OAM[0].attr1 =
            ATTR1_SIZE_32 |
            (screenX & 0x1FF);

        OAM[0].attr2 =
            ATTR2_PRIORITY(0);
    }

    return 0;
}
