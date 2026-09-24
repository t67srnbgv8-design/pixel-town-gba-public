#include <gba.h>

#define MAP_W 32
#define MAP_H 32

#define TILE_GRASS      0
#define TILE_GRASS_DOT  1
#define TILE_ROAD       2
#define TILE_WALL       3
#define TILE_ROOF       4
#define TILE_DOOR       5
#define TILE_TREE       6
#define TILE_FLOWER     7

#define DIR_DOWN  0
#define DIR_UP    1
#define DIR_LEFT  2
#define DIR_RIGHT 3

/* =========================================================
   4BPP TILE HELPERS
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
    u16 v =
        color |
        (color << 4) |
        (color << 8) |
        (color << 12);

    for (int i = 0; i < 16; i++)
        tile[i] = v;
}

/* =========================================================
   BACKGROUND
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

    /* Hauswand */
    fillTile(&tiles[TILE_WALL * 16], 4);

    for (int x = 0; x < 8; x++)
        setTilePixel(&tiles[TILE_WALL * 16], x, 7, 5);

    /* Dach */
    fillTile(&tiles[TILE_ROOF * 16], 6);

    for (int x = 0; x < 8; x += 2)
        setTilePixel(&tiles[TILE_ROOF * 16], x, 2, 7);

    /* Tür */
    fillTile(&tiles[TILE_DOOR * 16], 8);
    setTilePixel(&tiles[TILE_DOOR * 16], 6, 4, 9);

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
                ((x + y) & 1) ? 10 : 11
            );
        }
    }

    setTilePixel(&tiles[TILE_TREE * 16], 3, 6, 12);
    setTilePixel(&tiles[TILE_TREE * 16], 4, 6, 12);
    setTilePixel(&tiles[TILE_TREE * 16], 3, 7, 12);
    setTilePixel(&tiles[TILE_TREE * 16], 4, 7, 12);

    /* Blumen */
    fillTile(&tiles[TILE_FLOWER * 16], 1);
    setTilePixel(&tiles[TILE_FLOWER * 16], 2, 3, 13);
    setTilePixel(&tiles[TILE_FLOWER * 16], 3, 3, 14);
    setTilePixel(&tiles[TILE_FLOWER * 16], 6, 6, 13);
}

static void setMapTile(int x, int y, int tile)
{
    if (x < 0 || y < 0 || x >= MAP_W || y >= MAP_H)
        return;

    u16 *map = (u16 *)SCREEN_BASE_BLOCK(31);
    map[y * MAP_W + x] = tile;
}

static void createWorld(void)
{
    /* Gras */

    for (int y = 0; y < MAP_H; y++)
    {
        for (int x = 0; x < MAP_W; x++)
        {
            if ((x + y * 3) % 11 == 0)
                setMapTile(x, y, TILE_GRASS_DOT);
            else
                setMapTile(x, y, TILE_GRASS);
        }
    }

    /* Horizontale Straße */

    for (int y = 9; y <= 13; y++)
    {
        for (int x = 0; x < MAP_W; x++)
            setMapTile(x, y, TILE_ROAD);
    }

    /* Vertikale Straße */

    for (int x = 13; x <= 16; x++)
    {
        for (int y = 0; y < MAP_H; y++)
            setMapTile(x, y, TILE_ROAD);
    }

    /* Haus links */

    for (int x = 3; x <= 8; x++)
    {
        setMapTile(x, 2, TILE_ROOF);
        setMapTile(x, 3, TILE_ROOF);
    }

    for (int y = 4; y <= 7; y++)
    {
        for (int x = 3; x <= 8; x++)
            setMapTile(x, y, TILE_WALL);
    }

    setMapTile(5, 7, TILE_DOOR);

    /* Haus rechts */

    for (int x = 21; x <= 26; x++)
    {
        setMapTile(x, 2, TILE_ROOF);
        setMapTile(x, 3, TILE_ROOF);
    }

    for (int y = 4; y <= 7; y++)
    {
        for (int x = 21; x <= 26; x++)
            setMapTile(x, y, TILE_WALL);
    }

    setMapTile(24, 7, TILE_DOOR);

    /* Bäume */

    setMapTile(2, 15, TILE_TREE);
    setMapTile(4, 17, TILE_TREE);

    setMapTile(26, 15, TILE_TREE);
    setMapTile(28, 17, TILE_TREE);

    setMapTile(1, 19, TILE_TREE);
    setMapTile(29, 20, TILE_TREE);

    /* Blumen */

    for (int x = 7; x <= 11; x++)
        setMapTile(x, 18, TILE_FLOWER);

    for (int x = 19; x <= 23; x++)
        setMapTile(x, 18, TILE_FLOWER);
}

/* =========================================================
   COLLISION
   ========================================================= */

static int solidTile(int tx, int ty)
{
    if (tx < 0 || ty < 0 || tx >= MAP_W || ty >= MAP_H)
        return 1;

    u16 *map = (u16 *)SCREEN_BASE_BLOCK(31);

    int tile =
        map[ty * MAP_W + tx] & 0x3FF;

    if (tile == TILE_WALL)
        return 1;

    if (tile == TILE_ROOF)
        return 1;

    if (tile == TILE_TREE)
        return 1;

    return 0;
}

static int blocked(int x, int y)
{
    /*
       Figur selbst ist 16x24 Pixel groß,
       steckt aber in einem 32x32 Sprite.

       Kollidiert wird nur bei den Füßen.
    */

    int left   = x + 10;
    int right  = x + 21;

    int top    = y + 22;
    int bottom = y + 29;

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
   32x32 OBJ PIXEL

   Bei OBJ_1D_MAP liegt ein 32x32 4bpp Sprite
   als 4 Tiles pro Reihe und 4 Reihen im Speicher.
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
    /* 16 Tiles * 16 u16 pro Tile */

    for (int i = 0; i < 256; i++)
        base[i] = 0;
}

/* =========================================================
   PLAYER DRAWING
   ========================================================= */

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

    /*
       Figur wird ungefähr mittig in den
       32x32 Sprite gezeichnet.
    */

    int cx = 16;

    /* Schatten */

    for (int x = 10; x <= 21; x++)
    {
        objPixel(
            base,
            x,
            30,
            8
        );
    }

    /* Kopf */

    for (int y = 4; y <= 11; y++)
    {
        for (int x = 12; x <= 19; x++)
        {
            objPixel(
                base,
                x,
                y,
                skin
            );
        }
    }

    /* Haare oben */

    for (int x = 12; x <= 19; x++)
    {
        objPixel(
            base,
            x,
            3,
            hair
        );

        objPixel(
            base,
            x,
            4,
            hair
        );
    }

    objPixel(base, 11, 5, hair);
    objPixel(base, 20, 5, hair);

    /*
       Gesicht / Blickrichtung
    */

    if (direction == DIR_DOWN)
    {
        objPixel(base, 14, 8, outline);
        objPixel(base, 18, 8, outline);

        objPixel(base, 16, 10, light);
    }
    else if (direction == DIR_UP)
    {
        for (int x = 12; x <= 19; x++)
            objPixel(base, x, 7, hair);
    }
    else if (direction == DIR_LEFT)
    {
        objPixel(base, 13, 8, outline);
        objPixel(base, 12, 9, light);
    }
    else
    {
        objPixel(base, 18, 8, outline);
        objPixel(base, 19, 9, light);
    }

    /* Hals */

    objPixel(base, 15, 12, skin);
    objPixel(base, 16, 12, skin);

    /* Körper */

    for (int y = 13; y <= 21; y++)
    {
        for (int x = 12; x <= 19; x++)
        {
            objPixel(
                base,
                x,
                y,
                shirt
            );
        }
    }

    /* Shirt Highlight */

    for (int y = 14; y <= 19; y++)
        objPixel(base, 13, y, light);

    /*
       Arme:
       Beim Gehen bewegen sie sich leicht gegengleich.
    */

    int leftArmOffset  = frame ? 1 : 0;
    int rightArmOffset = frame ? 0 : 1;

    for (int y = 14; y <= 20; y++)
    {
        objPixel(
            base,
            10,
            y + leftArmOffset,
            skin
        );

        objPixel(
            base,
            11,
            y + leftArmOffset,
            skin
        );

        objPixel(
            base,
            20,
            y + rightArmOffset,
            skin
        );

        objPixel(
            base,
            21,
            y + rightArmOffset,
            skin
        );
    }

    /* Hose */

    for (int y = 22; y <= 24; y++)
    {
        for (int x = 12; x <= 19; x++)
        {
            objPixel(
                base,
                x,
                y,
                pants
            );
        }
    }

    /*
       Beine
    */

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
        /* Linkes Bein etwas nach vorne */

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

    (void)cx;
}

/* =========================================================
   PALETTES
   ========================================================= */

static void setupPalettes(void)
{
    /* BG */

    BG_PALETTE[0]  = RGB5(0, 0, 0);

    BG_PALETTE[1]  = RGB5(8, 22, 8);
    BG_PALETTE[2]  = RGB5(12, 27, 11);

    BG_PALETTE[3]  = RGB5(22, 19, 12);

    BG_PALETTE[4]  = RGB5(26, 21, 14);
    BG_PALETTE[5]  = RGB5(20, 15, 9);

    BG_PALETTE[6]  = RGB5(24, 6, 5);
    BG_PALETTE[7]  = RGB5(30, 11, 8);

    BG_PALETTE[8]  = RGB5(12, 7, 3);
    BG_PALETTE[9]  = RGB5(30, 25, 8);

    BG_PALETTE[10] = RGB5(4, 17, 5);
    BG_PALETTE[11] = RGB5(7, 24, 8);
    BG_PALETTE[12] = RGB5(15, 9, 4);

    BG_PALETTE[13] = RGB5(31, 8, 11);
    BG_PALETTE[14] = RGB5(31, 28, 6);


    /* Sprite */

    SPRITE_PALETTE[0] =
        RGB5(31, 0, 31);

    SPRITE_PALETTE[1] =
        RGB5(3, 3, 4);

    SPRITE_PALETTE[2] =
        RGB5(8, 4, 2);

    SPRITE_PALETTE[3] =
        RGB5(30, 21, 15);

    SPRITE_PALETTE[4] =
        RGB5(4, 11, 27);

    SPRITE_PALETTE[5] =
        RGB5(5, 7, 14);

    SPRITE_PALETTE[6] =
        RGB5(3, 3, 4);

    SPRITE_PALETTE[7] =
        RGB5(20, 22, 31);

    SPRITE_PALETTE[8] =
        RGB5(4, 12, 4);
}

/* =========================================================
   MAIN
   ========================================================= */

int main(void)
{
    irqInit();
    irqEnable(IRQ_VBLANK);

    /*
       Mode 0:
       Tilemap + Hardware OBJ.
    */

    SetMode(
        MODE_0 |
        BG0_ON |
        OBJ_ON |
        OBJ_1D_MAP
    );

    setupPalettes();

    REG_BG0CNT =
        BG_PRIORITY(1) |
        CHAR_BASE(0) |
        SCREEN_BASE(31) |
        BG_16_COLOR |
        BG_SIZE_0;

    createBackgroundTiles();
    createWorld();

    /*
       Spieler-Sprite beginnt bei OBJ Tile 0.
    */

    u16 *playerTiles =
        (u16 *)SPRITE_GFX;

    int px = 104;
    int py = 104;

    int direction = DIR_DOWN;

    int moving = 0;
    int walkFrame = 0;
    int walkTimer = 0;

    makePlayerFrame(
        playerTiles,
        direction,
        0
    );

    /*
       32x32 quadratisches Sprite.
    */

    OAM[0].attr0 =
        ATTR0_COLOR_16 |
        ATTR0_SQUARE |
        (py & 0xFF);

    OAM[0].attr1 =
        ATTR1_SIZE_32 |
        (px & 0x1FF);

    OAM[0].attr2 =
        ATTR2_PRIORITY(0);

    /*
       Alle anderen Sprites aus.
    */

    for (int i = 1; i < 128; i++)
    {
        OAM[i].attr0 =
            ATTR0_DISABLED;

        OAM[i].attr1 = 0;
        OAM[i].attr2 = 0;
    }

    while (1)
    {
        VBlankIntrWait();

        scanKeys();

        u16 keys =
            keysHeld();

        int nx = px;
        int ny = py;

        moving = 0;

        /*
           1 Pixel pro Frame.
           Deutlich kontrollierter als vorher.
        */

        if (keys & KEY_LEFT)
        {
            nx -= 1;
            direction = DIR_LEFT;
            moving = 1;
        }
        else if (keys & KEY_RIGHT)
        {
            nx += 1;
            direction = DIR_RIGHT;
            moving = 1;
        }
        else if (keys & KEY_UP)
        {
            ny -= 1;
            direction = DIR_UP;
            moving = 1;
        }
        else if (keys & KEY_DOWN)
        {
            ny += 1;
            direction = DIR_DOWN;
            moving = 1;
        }

        /*
           Kollision
        */

        if (!blocked(nx, py))
            px = nx;

        if (!blocked(px, ny))
            py = ny;

        /*
           Grenzen
        */

        if (px < -8)
            px = -8;

        if (px > 216)
            px = 216;

        if (py < -4)
            py = -4;

        if (py > 128)
            py = 128;

        /*
           Animation
        */

        if (moving)
        {
            walkTimer++;

            /*
               Langsamere Animation als zuvor.
            */

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

        /*
           Figur neu zeichnen.
        */

        makePlayerFrame(
            playerTiles,
            direction,
            moving ? walkFrame : 0
        );

        /*
           Hardware-Sprite verschieben.
        */

        OAM[0].attr0 =
            ATTR0_COLOR_16 |
            ATTR0_SQUARE |
            (py & 0xFF);

        OAM[0].attr1 =
            ATTR1_SIZE_32 |
            (px & 0x1FF);

        OAM[0].attr2 =
            ATTR2_PRIORITY(0);
    }

    return 0;
}
