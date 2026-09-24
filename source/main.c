#include <gba.h>

#define MAP_W 32
#define MAP_H 32

#define TILE_GRASS 0
#define TILE_GRASS_DOT 1
#define TILE_ROAD 2
#define TILE_ROAD_EDGE 3
#define TILE_WATER 4
#define TILE_WALL 5
#define TILE_ROOF 6
#define TILE_DOOR 7
#define TILE_TREE 8
#define TILE_FLOWER 9

#define DIR_DOWN  0
#define DIR_UP    1
#define DIR_LEFT  2
#define DIR_RIGHT 3

typedef struct {
    int x;
    int y;
    int w;
    int h;
} Rect;

/* ---------------------------------------------------------
   Hintergrund-Tiles
   4bpp: 8x8 Pixel = 32 Bytes pro Tile
   --------------------------------------------------------- */

static void setTilePixel(u16 *tile, int x, int y, u8 color) {
    int pixel = y * 8 + x;
    int word = pixel >> 2;
    int shift = (pixel & 3) * 4;

    tile[word] &= ~(0xF << shift);
    tile[word] |= (color & 0xF) << shift;
}

static void fillTile(u16 *tile, u8 color) {
    u16 value =
        color |
        (color << 4) |
        (color << 8) |
        (color << 12);

    for (int i = 0; i < 16; i++)
        tile[i] = value;
}

static void createBackgroundTiles(void) {
    u16 *tiles = (u16 *)CHAR_BASE_ADR(0);

    /* Gras */
    fillTile(&tiles[TILE_GRASS * 16], 1);

    /* Gras mit kleinen Details */
    fillTile(&tiles[TILE_GRASS_DOT * 16], 1);
    setTilePixel(&tiles[TILE_GRASS_DOT * 16], 2, 2, 2);
    setTilePixel(&tiles[TILE_GRASS_DOT * 16], 6, 5, 2);

    /* Straße */
    fillTile(&tiles[TILE_ROAD * 16], 3);

    /* Straßenrand */
    fillTile(&tiles[TILE_ROAD_EDGE * 16], 3);
    for (int x = 0; x < 8; x++)
        setTilePixel(&tiles[TILE_ROAD_EDGE * 16], x, 0, 4);

    /* Wasser */
    fillTile(&tiles[TILE_WATER * 16], 5);
    for (int x = 0; x < 8; x += 3)
        setTilePixel(&tiles[TILE_WATER * 16], x, 3, 6);

    /* Hauswand */
    fillTile(&tiles[TILE_WALL * 16], 7);
    for (int x = 0; x < 8; x++)
        setTilePixel(&tiles[TILE_WALL * 16], x, 7, 8);

    /* Dach */
    fillTile(&tiles[TILE_ROOF * 16], 9);
    for (int x = 0; x < 8; x += 2)
        setTilePixel(&tiles[TILE_ROOF * 16], x, 2, 10);

    /* Tür */
    fillTile(&tiles[TILE_DOOR * 16], 11);
    setTilePixel(&tiles[TILE_DOOR * 16], 6, 4, 12);

    /* Baum */
    fillTile(&tiles[TILE_TREE * 16], 1);

    for (int y = 0; y < 6; y++) {
        for (int x = 1; x < 7; x++) {
            if ((x + y) % 2)
                setTilePixel(&tiles[TILE_TREE * 16], x, y, 13);
            else
                setTilePixel(&tiles[TILE_TREE * 16], x, y, 14);
        }
    }

    setTilePixel(&tiles[TILE_TREE * 16], 3, 6, 15);
    setTilePixel(&tiles[TILE_TREE * 16], 4, 6, 15);
    setTilePixel(&tiles[TILE_TREE * 16], 3, 7, 15);
    setTilePixel(&tiles[TILE_TREE * 16], 4, 7, 15);

    /* Blumen */
    fillTile(&tiles[TILE_FLOWER * 16], 1);

    setTilePixel(&tiles[TILE_FLOWER * 16], 2, 3, 10);
    setTilePixel(&tiles[TILE_FLOWER * 16], 3, 3, 12);
    setTilePixel(&tiles[TILE_FLOWER * 16], 6, 6, 10);
}

/* ---------------------------------------------------------
   Welt
   --------------------------------------------------------- */

static void setMapTile(int x, int y, int tile) {
    if (x < 0 || y < 0 || x >= MAP_W || y >= MAP_H)
        return;

    u16 *map = (u16 *)SCREEN_BASE_BLOCK(31);
    map[y * MAP_W + x] = tile;
}

static void createWorld(void) {
    for (int y = 0; y < MAP_H; y++) {
        for (int x = 0; x < MAP_W; x++) {
            if ((x + y * 3) % 11 == 0)
                setMapTile(x, y, TILE_GRASS_DOT);
            else
                setMapTile(x, y, TILE_GRASS);
        }
    }

    /* Horizontale Straße */
    for (int y = 9; y <= 13; y++) {
        for (int x = 0; x < MAP_W; x++)
            setMapTile(x, y, TILE_ROAD);
    }

    /* Vertikale Straße */
    for (int x = 13; x <= 16; x++) {
        for (int y = 0; y < MAP_H; y++)
            setMapTile(x, y, TILE_ROAD);
    }

    /* Haus links */
    for (int x = 3; x <= 8; x++) {
        setMapTile(x, 2, TILE_ROOF);
        setMapTile(x, 3, TILE_ROOF);
    }

    for (int y = 4; y <= 7; y++) {
        for (int x = 3; x <= 8; x++)
            setMapTile(x, y, TILE_WALL);
    }

    setMapTile(5, 7, TILE_DOOR);

    /* Haus rechts */
    for (int x = 21; x <= 26; x++) {
        setMapTile(x, 2, TILE_ROOF);
        setMapTile(x, 3, TILE_ROOF);
    }

    for (int y = 4; y <= 7; y++) {
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

/* ---------------------------------------------------------
   Kollision
   --------------------------------------------------------- */

static int solidTile(int tx, int ty) {
    if (tx < 0 || ty < 0 || tx >= MAP_W || ty >= MAP_H)
        return 1;

    u16 *map = (u16 *)SCREEN_BASE_BLOCK(31);
    int tile = map[ty * MAP_W + tx] & 0x3FF;

    if (tile == TILE_WALL)
        return 1;

    if (tile == TILE_ROOF)
        return 1;

    if (tile == TILE_TREE)
        return 1;

    return 0;
}

static int blocked(int x, int y) {
    /*
       Kollisionsbox nur bei den Füßen.
       Dadurch kann die Figur optisch vor Objekten stehen.
    */

    int left   = x + 3;
    int right  = x + 12;
    int top    = y + 18;
    int bottom = y + 30;

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

/* ---------------------------------------------------------
   Spieler-Sprite
   16x32 Pixel
   --------------------------------------------------------- */

static void objPixel(u16 *tileBase, int x, int y, u8 color) {
    /*
       16x32 Sprite = 2 Tiles breit, 4 Tiles hoch.
    */

    int tileX = x / 8;
    int tileY = y / 8;

    int localX = x & 7;
    int localY = y & 7;

    int tileNumber = tileY * 2 + tileX;

    u16 *tile = &tileBase[tileNumber * 16];

    setTilePixel(tile, localX, localY, color);
}

static void clearPlayerTiles(u16 *base) {
    for (int i = 0; i < 8 * 16; i++)
        base[i] = 0;
}

static void makePlayerFrame(
    u16 *base,
    int direction,
    int walking
) {
    clearPlayerTiles(base);

    /* Farben */
    const int outline = 1;
    const int hair    = 2;
    const int skin    = 3;
    const int shirt   = 4;
    const int pants   = 5;
    const int shoe    = 6;

    /* Kopf */
    for (int y = 2; y <= 10; y++) {
        for (int x = 4; x <= 11; x++)
            objPixel(base, x, y, skin);
    }

    /* Haare */
    for (int y = 1; y <= 4; y++) {
        for (int x = 4; x <= 11; x++)
            objPixel(base, x, y, hair);
    }

    objPixel(base, 3, 4, hair);
    objPixel(base, 12, 4, hair);

    /* Körper */
    for (int y = 11; y <= 21; y++) {
        for (int x = 4; x <= 11; x++)
            objPixel(base, x, y, shirt);
    }

    /* Arme */
    for (int y = 12; y <= 20; y++) {
        objPixel(base, 2, y, skin);
        objPixel(base, 3, y, skin);
        objPixel(base, 12, y, skin);
        objPixel(base, 13, y, skin);
    }

    /* Richtung sichtbar machen */
    if (direction == DIR_DOWN) {
        objPixel(base, 6, 7, outline);
        objPixel(base, 9, 7, outline);
    }

    if (direction == DIR_UP) {
        for (int x = 4; x <= 11; x++)
            objPixel(base, x, 5, hair);
    }

    if (direction == DIR_LEFT) {
        objPixel(base, 5, 7, outline);
    }

    if (direction == DIR_RIGHT) {
        objPixel(base, 10, 7, outline);
    }

    /* Hose */
    for (int y = 22; y <= 25; y++) {
        for (int x = 4; x <= 11; x++)
            objPixel(base, x, y, pants);
    }

    /* Beine */
    if (walking) {
        for (int y = 26; y <= 29; y++) {
            objPixel(base, 3, y, pants);
            objPixel(base, 4, y, pants);

            objPixel(base, 10, y, pants);
            objPixel(base, 11, y, pants);
        }

        objPixel(base, 2, 30, shoe);
        objPixel(base, 3, 30, shoe);
        objPixel(base, 4, 30, shoe);

        objPixel(base, 10, 29, shoe);
        objPixel(base, 11, 29, shoe);
        objPixel(base, 12, 29, shoe);
    } else {
        for (int y = 26; y <= 29; y++) {
            objPixel(base, 4, y, pants);
            objPixel(base, 5, y, pants);

            objPixel(base, 10, y, pants);
            objPixel(base, 11, y, pants);
        }

        objPixel(base, 3, 30, shoe);
        objPixel(base, 4, 30, shoe);
        objPixel(base, 5, 30, shoe);

        objPixel(base, 10, 30, shoe);
        objPixel(base, 11, 30, shoe);
        objPixel(base, 12, 30, shoe);
    }
}

/* ---------------------------------------------------------
   Farben
   --------------------------------------------------------- */

static void setupPalettes(void) {
    /* BG Palette */

    BG_PALETTE[0]  = RGB5(0, 0, 0);

    BG_PALETTE[1]  = RGB5(8, 22, 8);
    BG_PALETTE[2]  = RGB5(12, 27, 11);

    BG_PALETTE[3]  = RGB5(22, 19, 12);
    BG_PALETTE[4]  = RGB5(28, 25, 18);

    BG_PALETTE[5]  = RGB5(5, 14, 25);
    BG_PALETTE[6]  = RGB5(8, 21, 30);

    BG_PALETTE[7]  = RGB5(26, 21, 14);
    BG_PALETTE[8]  = RGB5(20, 15, 9);

    BG_PALETTE[9]  = RGB5(24, 6, 5);
    BG_PALETTE[10] = RGB5(30, 11, 8);

    BG_PALETTE[11] = RGB5(12, 7, 3);
    BG_PALETTE[12] = RGB5(30, 25, 8);

    BG_PALETTE[13] = RGB5(4, 17, 5);
    BG_PALETTE[14] = RGB5(7, 24, 8);
    BG_PALETTE[15] = RGB5(15, 9, 4);

    /* OBJ Palette */

    OBJ_PALETTE[0] = RGB5(31, 0, 31);
    OBJ_PALETTE[1] = RGB5(3, 3, 4);
    OBJ_PALETTE[2] = RGB5(8, 4, 2);
    OBJ_PALETTE[3] = RGB5(30, 21, 15);
    OBJ_PALETTE[4] = RGB5(4, 11, 27);
    OBJ_PALETTE[5] = RGB5(5, 7, 14);
    OBJ_PALETTE[6] = RGB5(3, 3, 4);
}

/* ---------------------------------------------------------
   Hauptprogramm
   --------------------------------------------------------- */

int main(void) {
    irqInit();
    irqEnable(IRQ_VBLANK);

    /*
       Mode 0:
       BG0 = Tile-Hintergrund
       OBJ = Hardware-Sprites
       OBJ_1D_MAP = einfache Sprite-Tile-Anordnung
    */
    SetMode(MODE_0 | BG0_ON | OBJ_ON | OBJ_1D_MAP);

    setupPalettes();

    /*
       BG0:
       Character Block 0
       Screen Block 31
       4bpp
       32x32 Tilemap
    */
    REG_BG0CNT =
        BG_PRIORITY(1) |
        CHAR_BASE(0) |
        SCREEN_BASE(31) |
        BG_16_COLOR |
        BG_SIZE_0;

    createBackgroundTiles();
    createWorld();

    /*
       Sprite-Daten beginnen bei Tile 0 des OBJ-Speichers.
    */
    u16 *playerTiles = (u16 *)SPRITE_GFX;

    int px = 112;
    int py = 112;

    int direction = DIR_DOWN;
    int walking = 0;
    int walkTimer = 0;
    int walkFrame = 0;

    makePlayerFrame(playerTiles, direction, 0);

    /*
       OAM Eintrag 0:
       16x32 Sprite
    */
    OAM[0].attr0 =
        ATTR0_COLOR_16 |
        ATTR0_TALL |
        (py & 0xFF);

    OAM[0].attr1 =
        ATTR1_SIZE_16 |
        (px & 0x1FF);

    OAM[0].attr2 =
        ATTR2_ID(0) |
        ATTR2_PRIORITY(0);

    /*
       Restliche Sprites verstecken.
    */
    for (int i = 1; i < 128; i++) {
        OAM[i].attr0 = ATTR0_DISABLED;
        OAM[i].attr1 = 0;
        OAM[i].attr2 = 0;
    }

    while (1) {
        VBlankIntrWait();
        scanKeys();

        u16 keys = keysHeld();

        int nx = px;
        int ny = py;

        walking = 0;

        /*
           Immer nur eine Hauptrichtung gleichzeitig.
           Dadurch wirkt das Laufen kontrollierter.
        */
        if (keys & KEY_LEFT) {
            nx -= 2;
            direction = DIR_LEFT;
            walking = 1;
        }
        else if (keys & KEY_RIGHT) {
            nx += 2;
            direction = DIR_RIGHT;
            walking = 1;
        }
        else if (keys & KEY_UP) {
            ny -= 2;
            direction = DIR_UP;
            walking = 1;
        }
        else if (keys & KEY_DOWN) {
            ny += 2;
            direction = DIR_DOWN;
            walking = 1;
        }

        /*
           X/Y getrennt prüfen.
        */
        if (!blocked(nx, py))
            px = nx;

        if (!blocked(px, ny))
            py = ny;

        /*
           Bildschirmgrenzen.
        */
        if (px < 0)
            px = 0;

        if (px > 224)
            px = 224;

        if (py < 0)
            py = 0;

        if (py > 128)
            py = 128;

        /*
           Laufanimation.
        */
        if (walking) {
            walkTimer++;

            if (walkTimer >= 7) {
                walkTimer = 0;
                walkFrame ^= 1;
            }
        } else {
            walkTimer = 0;
            walkFrame = 0;
        }

        /*
           Nur Sprite-Grafik aktualisieren.
           Der Hintergrund wird NICHT neu gezeichnet.
        */
        makePlayerFrame(
            playerTiles,
            direction,
            walking ? walkFrame : 0
        );

        /*
           Hardware-Sprite bewegen.
        */
        OAM[0].attr0 =
            ATTR0_COLOR_16 |
            ATTR0_TALL |
            (py & 0xFF);

        OAM[0].attr1 =
            ATTR1_SIZE_16 |
            (px & 0x1FF);

        OAM[0].attr2 =
            ATTR2_ID(0) |
            ATTR2_PRIORITY(0);
    }

    return 0;
}
