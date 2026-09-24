#include <gba.h>

#define MAP_W 64
#define MAP_H 32
#define WORLD_W (MAP_W * 8)
#define WORLD_H (MAP_H * 8)

#define SCREEN_W 240
#define SCREEN_H 160

/* ---------- Tiles ---------- */

#define T_GRASS        0
#define T_GRASS2       1
#define T_PATH         2
#define T_PATH_EDGE    3
#define T_WALL         4
#define T_WALL_LINE    5
#define T_ROOF         6
#define T_ROOF_LIGHT   7
#define T_ROOF_EDGE    8
#define T_DOOR         9
#define T_WINDOW       10
#define T_TREE_TOP     11
#define T_TREE_MID     12
#define T_TREE_BOTTOM  13
#define T_BUSH         14
#define T_FLOWER       15
#define T_FENCE        16
#define T_SIGN         17
#define T_STONE        18
#define T_DARK_GRASS   19

#define DIR_DOWN  0
#define DIR_UP    1
#define DIR_LEFT  2
#define DIR_RIGHT 3

/* =========================================================
   TILE DRAWING
   ========================================================= */

static void pixel(u16 *tile, int x, int y, u8 c)
{
    int p = y * 8 + x;
    int word = p >> 2;
    int shift = (p & 3) * 4;

    tile[word] &= ~(0xF << shift);
    tile[word] |= (c & 0xF) << shift;
}

static void fill(u16 *tile, u8 c)
{
    u16 v =
        c |
        (c << 4) |
        (c << 8) |
        (c << 12);

    for (int i = 0; i < 16; i++)
        tile[i] = v;
}

static void makeTiles(void)
{
    u16 *t = (u16 *)CHAR_BASE_ADR(0);

    /* grass */
    fill(&t[T_GRASS * 16], 1);

    fill(&t[T_GRASS2 * 16], 1);
    pixel(&t[T_GRASS2 * 16], 1, 2, 2);
    pixel(&t[T_GRASS2 * 16], 6, 5, 2);
    pixel(&t[T_GRASS2 * 16], 3, 7, 2);

    /* dirt path */
    fill(&t[T_PATH * 16], 3);
    pixel(&t[T_PATH * 16], 1, 1, 4);
    pixel(&t[T_PATH * 16], 6, 3, 4);
    pixel(&t[T_PATH * 16], 3, 6, 4);

    /* path edge */
    fill(&t[T_PATH_EDGE * 16], 3);

    for (int x = 0; x < 8; x++)
    {
        pixel(&t[T_PATH_EDGE * 16], x, 0, 5);

        if ((x & 1) == 0)
            pixel(&t[T_PATH_EDGE * 16], x, 1, 4);
    }

    /* wall */
    fill(&t[T_WALL * 16], 6);

    pixel(&t[T_WALL * 16], 0, 3, 7);
    pixel(&t[T_WALL * 16], 1, 3, 7);
    pixel(&t[T_WALL * 16], 2, 3, 7);
    pixel(&t[T_WALL * 16], 3, 3, 7);
    pixel(&t[T_WALL * 16], 4, 3, 7);
    pixel(&t[T_WALL * 16], 5, 3, 7);
    pixel(&t[T_WALL * 16], 6, 3, 7);
    pixel(&t[T_WALL * 16], 7, 3, 7);

    /* lower wall */
    fill(&t[T_WALL_LINE * 16], 6);

    for (int x = 0; x < 8; x++)
    {
        pixel(&t[T_WALL_LINE * 16], x, 6, 7);
        pixel(&t[T_WALL_LINE * 16], x, 7, 7);
    }

    /* roof */
    fill(&t[T_ROOF * 16], 8);

    for (int y = 1; y < 8; y += 3)
    {
        for (int x = 0; x < 8; x++)
            pixel(&t[T_ROOF * 16], x, y, 9);
    }

    for (int y = 0; y < 8; y += 3)
    {
        pixel(&t[T_ROOF * 16], 3, y, 10);
        pixel(&t[T_ROOF * 16], 7, y, 10);
    }

    /* roof highlight */
    fill(&t[T_ROOF_LIGHT * 16], 9);

    for (int y = 2; y < 8; y += 3)
    {
        for (int x = 0; x < 8; x++)
            pixel(&t[T_ROOF_LIGHT * 16], x, y, 8);
    }

    /* roof lower edge */
    fill(&t[T_ROOF_EDGE * 16], 8);

    for (int x = 0; x < 8; x++)
    {
        pixel(&t[T_ROOF_EDGE * 16], x, 5, 10);
        pixel(&t[T_ROOF_EDGE * 16], x, 6, 10);
        pixel(&t[T_ROOF_EDGE * 16], x, 7, 7);
    }

    /* door */
    fill(&t[T_DOOR * 16], 11);

    for (int y = 0; y < 8; y++)
    {
        pixel(&t[T_DOOR * 16], 0, y, 7);
        pixel(&t[T_DOOR * 16], 7, y, 7);
    }

    pixel(&t[T_DOOR * 16], 5, 4, 15);

    /* window */
    fill(&t[T_WINDOW * 16], 6);

    for (int y = 1; y <= 6; y++)
    {
        for (int x = 1; x <= 6; x++)
            pixel(&t[T_WINDOW * 16], x, y, 12);
    }

    for (int x = 1; x <= 6; x++)
        pixel(&t[T_WINDOW * 16], x, 4, 13);

    for (int y = 1; y <= 6; y++)
        pixel(&t[T_WINDOW * 16], 4, y, 13);

    /* tree top */
    fill(&t[T_TREE_TOP * 16], 1);

    for (int y = 1; y < 8; y++)
    {
        for (int x = 1; x < 7; x++)
        {
            if (!(y == 1 && (x == 1 || x == 6)))
                pixel(
                    &t[T_TREE_TOP * 16],
                    x,
                    y,
                    ((x + y) & 1) ? 14 : 15
                );
        }
    }

    /* tree middle */
    fill(&t[T_TREE_MID * 16], 1);

    for (int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            if (!(y > 5 && (x == 0 || x == 7)))
                pixel(
                    &t[T_TREE_MID * 16],
                    x,
                    y,
                    ((x + y) & 1) ? 14 : 15
                );
        }
    }

    /* tree bottom */
    fill(&t[T_TREE_BOTTOM * 16], 1);

    for (int y = 0; y < 3; y++)
    {
        for (int x = 1; x < 7; x++)
            pixel(&t[T_TREE_BOTTOM * 16], x, y, 14);
    }

    for (int y = 3; y < 8; y++)
    {
        pixel(&t[T_TREE_BOTTOM * 16], 3, y, 11);
        pixel(&t[T_TREE_BOTTOM * 16], 4, y, 11);
    }

    /* bush */
    fill(&t[T_BUSH * 16], 1);

    for (int y = 2; y < 7; y++)
    {
        for (int x = 1; x < 7; x++)
            pixel(
                &t[T_BUSH * 16],
                x,
                y,
                ((x + y) & 1) ? 14 : 15
            );
    }

    /* flowers */
    fill(&t[T_FLOWER * 16], 1);
    pixel(&t[T_FLOWER * 16], 2, 2, 10);
    pixel(&t[T_FLOWER * 16], 3, 2, 13);
    pixel(&t[T_FLOWER * 16], 5, 5, 10);
    pixel(&t[T_FLOWER * 16], 6, 5, 13);

    /* fence */
    fill(&t[T_FENCE * 16], 1);

    for (int x = 0; x < 8; x++)
    {
        pixel(&t[T_FENCE * 16], x, 3, 11);
        pixel(&t[T_FENCE * 16], x, 5, 11);
    }

    pixel(&t[T_FENCE * 16], 1, 1, 11);
    pixel(&t[T_FENCE * 16], 1, 7, 11);
    pixel(&t[T_FENCE * 16], 6, 1, 11);
    pixel(&t[T_FENCE * 16], 6, 7, 11);

    /* sign */
    fill(&t[T_SIGN * 16], 1);

    for (int y = 1; y <= 4; y++)
        for (int x = 1; x <= 6; x++)
            pixel(&t[T_SIGN * 16], x, y, 11);

    pixel(&t[T_SIGN * 16], 3, 5, 11);
    pixel(&t[T_SIGN * 16], 4, 5, 11);
    pixel(&t[T_SIGN * 16], 3, 6, 11);
    pixel(&t[T_SIGN * 16], 4, 6, 11);
    pixel(&t[T_SIGN * 16], 3, 7, 11);
    pixel(&t[T_SIGN * 16], 4, 7, 11);

    /* stone */
    fill(&t[T_STONE * 16], 1);

    for (int y = 3; y <= 6; y++)
        for (int x = 2; x <= 5; x++)
            pixel(&t[T_STONE * 16], x, y, 5);

    /* dark grass border */
    fill(&t[T_DARK_GRASS * 16], 2);
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
        return T_WALL;

    if (x < 32)
    {
        u16 *map = (u16 *)SCREEN_BASE_BLOCK(30);
        return map[y * 32 + x] & 0x3FF;
    }

    u16 *map = (u16 *)SCREEN_BASE_BLOCK(31);

    return map[y * 32 + (x - 32)] & 0x3FF;
}

/* =========================================================
   HOUSE
   ========================================================= */

static void house(int x, int y, int w)
{
    int right = x + w - 1;

    /*
       Roof.
       Wider than facade and visually taller.
    */

    for (int yy = y; yy <= y + 3; yy++)
    {
        for (int xx = x; xx <= right; xx++)
        {
            if (yy == y)
                setMap(xx, yy, T_ROOF_LIGHT);
            else
                setMap(xx, yy, T_ROOF);
        }
    }

    for (int xx = x - 1; xx <= right + 1; xx++)
        setMap(xx, y + 4, T_ROOF_EDGE);

    /*
       Facade.
    */

    for (int yy = y + 5; yy <= y + 8; yy++)
    {
        for (int xx = x; xx <= right; xx++)
            setMap(xx, yy, T_WALL);
    }

    for (int xx = x; xx <= right; xx++)
        setMap(xx, y + 8, T_WALL_LINE);

    /*
       Windows.
    */

    setMap(x + 1, y + 6, T_WINDOW);
    setMap(x + 2, y + 6, T_WINDOW);

    setMap(right - 2, y + 6, T_WINDOW);
    setMap(right - 1, y + 6, T_WINDOW);

    /*
       16 px wide door.
    */

    int d = x + w / 2 - 1;

    setMap(d,     y + 7, T_DOOR);
    setMap(d + 1, y + 7, T_DOOR);

    setMap(d,     y + 8, T_DOOR);
    setMap(d + 1, y + 8, T_DOOR);

    /*
       Short front path.
    */

    setMap(d,     y + 9, T_PATH);
    setMap(d + 1, y + 9, T_PATH);
}

/* =========================================================
   TREE
   ========================================================= */

static void tree(int x, int y)
{
    /*
       24 x 32-ish visual footprint.
    */

    setMap(x,     y, T_TREE_TOP);
    setMap(x + 1, y, T_TREE_TOP);
    setMap(x + 2, y, T_TREE_TOP);

    setMap(x,     y + 1, T_TREE_MID);
    setMap(x + 1, y + 1, T_TREE_MID);
    setMap(x + 2, y + 1, T_TREE_MID);

    setMap(x,     y + 2, T_TREE_MID);
    setMap(x + 1, y + 2, T_TREE_MID);
    setMap(x + 2, y + 2, T_TREE_MID);

    setMap(x,     y + 3, T_GRASS);
    setMap(x + 1, y + 3, T_TREE_BOTTOM);
    setMap(x + 2, y + 3, T_GRASS);
}

/* =========================================================
   WORLD
   ========================================================= */

static void makeWorld(void)
{
    /*
       Base grass.
    */

    for (int y = 0; y < MAP_H; y++)
    {
        for (int x = 0; x < MAP_W; x++)
        {
            if ((x * 5 + y * 11) % 29 == 0)
                setMap(x, y, T_GRASS2);
            else
                setMap(x, y, T_GRASS);
        }
    }

    /*
       Main horizontal path.
       3 tiles / 24 px.
    */

    for (int y = 15; y <= 17; y++)
    {
        for (int x = 0; x < MAP_W; x++)
            setMap(x, y, T_PATH);
    }

    /*
       Vertical path.
    */

    for (int x = 30; x <= 32; x++)
    {
        for (int y = 0; y < MAP_H; y++)
            setMap(x, y, T_PATH);
    }

    /*
       Path borders.
    */

    for (int x = 0; x < MAP_W; x++)
    {
        setMap(x, 14, T_PATH_EDGE);
        setMap(x, 18, T_PATH_EDGE);
    }

    /*
       Houses north of road.
    */

    house(3, 3, 9);
    house(16, 4, 10);
    house(36, 3, 9);
    house(49, 4, 10);

    /*
       Houses south.
       More spacing than before.
    */

    house(5, 21, 10);
    house(20, 22, 8);
    house(36, 21, 10);
    house(51, 22, 9);

    /*
       Trees along outer areas.
    */

    tree(0, 1);
    tree(12, 0);
    tree(27, 1);
    tree(42, 0);
    tree(59, 1);

    tree(0, 20);
    tree(14, 20);
    tree(31, 21);
    tree(46, 20);
    tree(61, 20);

    /*
       Bush groups.
    */

    for (int x = 1; x <= 10; x++)
        setMap(x, 13, T_BUSH);

    for (int x = 38; x <= 45; x++)
        setMap(x, 13, T_BUSH);

    /*
       Breaks in bushes for walking.
    */

    setMap(6, 13, T_GRASS);
    setMap(7, 13, T_GRASS);

    setMap(41, 13, T_GRASS);
    setMap(42, 13, T_GRASS);

    /*
       Flower beds.
    */

    setMap(13, 11, T_FLOWER);
    setMap(14, 11, T_FLOWER);
    setMap(13, 12, T_FLOWER);
    setMap(14, 12, T_FLOWER);

    setMap(46, 11, T_FLOWER);
    setMap(47, 11, T_FLOWER);
    setMap(46, 12, T_FLOWER);
    setMap(47, 12, T_FLOWER);

    /*
       Signs.
    */

    setMap(27, 13, T_SIGN);
    setMap(34, 19, T_SIGN);

    /*
       Fences.
    */

    for (int x = 1; x <= 12; x++)
        setMap(x, 30, T_FENCE);

    for (int x = 36; x <= 47; x++)
        setMap(x, 30, T_FENCE);

    /*
       Small stones / details.
    */

    setMap(14, 19, T_STONE);
    setMap(44, 19, T_STONE);
    setMap(57, 12, T_STONE);
}

/* =========================================================
   COLLISION
   ========================================================= */

static int solid(int tx, int ty)
{
    int t = getMap(tx, ty);

    if (
        t == T_WALL ||
        t == T_WALL_LINE ||
        t == T_ROOF ||
        t == T_ROOF_LIGHT ||
        t == T_ROOF_EDGE ||
        t == T_TREE_TOP ||
        t == T_TREE_MID ||
        t == T_TREE_BOTTOM ||
        t == T_BUSH ||
        t == T_FENCE ||
        t == T_SIGN
    )
        return 1;

    return 0;
}

static int blocked(int x, int y)
{
    /*
       16x32 sprite, but collision only around feet.
    */

    int left   = x + 4;
    int right  = x + 11;

    int top    = y + 24;
    int bottom = y + 28;

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
   PLAYER
   ========================================================= */

static void objPixel(u16 *base, int x, int y, u8 c)
{
    if (x < 0 || x >= 16 || y < 0 || y >= 32)
        return;

    int tileX = x >> 3;
    int tileY = y >> 3;

    int lx = x & 7;
    int ly = y & 7;

    int tile = tileY * 2 + tileX;

    pixel(
        &base[tile * 16],
        lx,
        ly,
        c
    );
}

static void clearPlayer(u16 *base)
{
    for (int i = 0; i < 128; i++)
        base[i] = 0;
}

static void playerFrame(
    u16 *base,
    int dir,
    int frame
)
{
    clearPlayer(base);

    int outline = 1;
    int hair = 2;
    int skin = 3;
    int shirt = 4;
    int pants = 5;
    int shoes = 6;
    int shirtLight = 7;
    int shadow = 8;

    /*
       Shadow.
    */

    for (int x = 4; x <= 11; x++)
        objPixel(base, x, 29, shadow);

    /*
       Head.
    */

    for (int y = 7; y <= 12; y++)
    {
        for (int x = 5; x <= 10; x++)
            objPixel(base, x, y, skin);
    }

    /*
       Hair.
    */

    for (int x = 5; x <= 10; x++)
    {
        objPixel(base, x, 6, hair);
        objPixel(base, x, 7, hair);
    }

    objPixel(base, 4, 8, hair);
    objPixel(base, 11, 8, hair);

    /*
       Face based on direction.
    */

    if (dir == DIR_DOWN)
    {
        objPixel(base, 6, 10, outline);
        objPixel(base, 9, 10, outline);
    }
    else if (dir == DIR_UP)
    {
        for (int x = 5; x <= 10; x++)
            objPixel(base, x, 10, hair);
    }
    else if (dir == DIR_LEFT)
    {
        objPixel(base, 5, 10, outline);
    }
    else
    {
        objPixel(base, 10, 10, outline);
    }

    /*
       Shirt.
    */

    for (int y = 14; y <= 20; y++)
    {
        for (int x = 5; x <= 10; x++)
            objPixel(base, x, y, shirt);
    }

    objPixel(base, 6, 15, shirtLight);
    objPixel(base, 6, 16, shirtLight);

    /*
       Arms.
    */

    int a = frame ? 1 : 0;
    int b = frame ? 0 : 1;

    for (int y = 15; y <= 20; y++)
    {
        objPixel(base, 3, y + a, skin);
        objPixel(base, 4, y + a, skin);

        objPixel(base, 11, y + b, skin);
        objPixel(base, 12, y + b, skin);
    }

    /*
       Pants.
    */

    for (int y = 21; y <= 23; y++)
    {
        for (int x = 5; x <= 10; x++)
            objPixel(base, x, y, pants);
    }

    /*
       Walking legs.
    */

    if (frame == 0)
    {
        for (int y = 24; y <= 27; y++)
        {
            objPixel(base, 5, y, pants);
            objPixel(base, 6, y, pants);

            objPixel(base, 9, y, pants);
            objPixel(base, 10, y, pants);
        }

        objPixel(base, 4, 28, shoes);
        objPixel(base, 5, 28, shoes);
        objPixel(base, 6, 28, shoes);

        objPixel(base, 9, 28, shoes);
        objPixel(base, 10, 28, shoes);
        objPixel(base, 11, 28, shoes);
    }
    else
    {
        for (int y = 24; y <= 27; y++)
        {
            objPixel(base, 4, y, pants);
            objPixel(base, 5, y, pants);

            objPixel(base, 10, y, pants);
            objPixel(base, 11, y, pants);
        }

        objPixel(base, 3, 28, shoes);
        objPixel(base, 4, 28, shoes);
        objPixel(base, 5, 28, shoes);

        objPixel(base, 10, 28, shoes);
        objPixel(base, 11, 28, shoes);
        objPixel(base, 12, 28, shoes);
    }
}

/* =========================================================
   PALETTES
   ========================================================= */

static void makePalettes(void)
{
    BG_PALETTE[0]  = RGB5(0,0,0);

    BG_PALETTE[1]  = RGB5(10,24,12);
    BG_PALETTE[2]  = RGB5(15,28,15);

    BG_PALETTE[3]  = RGB5(25,22,14);
    BG_PALETTE[4]  = RGB5(29,26,18);
    BG_PALETTE[5]  = RGB5(18,17,12);

    BG_PALETTE[6]  = RGB5(28,24,18);
    BG_PALETTE[7]  = RGB5(18,14,10);

    BG_PALETTE[8]  = RGB5(25,7,6);
    BG_PALETTE[9]  = RGB5(30,12,9);
    BG_PALETTE[10] = RGB5(17,4,4);

    BG_PALETTE[11] = RGB5(13,8,4);

    BG_PALETTE[12] = RGB5(12,23,30);
    BG_PALETTE[13] = RGB5(23,29,31);

    BG_PALETTE[14] = RGB5(4,17,6);
    BG_PALETTE[15] = RGB5(8,27,10);

    SPRITE_PALETTE[0] = RGB5(31,0,31);
    SPRITE_PALETTE[1] = RGB5(3,3,4);
    SPRITE_PALETTE[2] = RGB5(9,5,3);
    SPRITE_PALETTE[3] = RGB5(30,22,16);
    SPRITE_PALETTE[4] = RGB5(5,12,28);
    SPRITE_PALETTE[5] = RGB5(5,7,14);
    SPRITE_PALETTE[6] = RGB5(3,3,4);
    SPRITE_PALETTE[7] = RGB5(18,22,31);
    SPRITE_PALETTE[8] = RGB5(4,12,4);
}

/* =========================================================
   HELPERS
   ========================================================= */

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

    makePalettes();

    REG_BG0CNT =
        BG_PRIORITY(1) |
        CHAR_BASE(0) |
        SCREEN_BASE(30) |
        BG_16_COLOR |
        BG_SIZE_1;

    makeTiles();
    makeWorld();

    u16 *playerTiles =
        (u16 *)SPRITE_GFX;

    int playerX = 244;
    int playerY = 92;

    int direction = DIR_DOWN;

    int moving = 0;
    int walkFrame = 0;
    int walkTimer = 0;

    playerFrame(
        playerTiles,
        direction,
        0
    );

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

        nx = clampInt(
            nx,
            0,
            WORLD_W - 16
        );

        ny = clampInt(
            ny,
            0,
            WORLD_H - 32
        );

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

        playerFrame(
            playerTiles,
            direction,
            moving ? walkFrame : 0
        );

        /*
           CAMERA
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

        int screenX =
            playerX - cameraX;

        int screenY =
            playerY - cameraY;

        /*
           16x32 player sprite.
        */

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
