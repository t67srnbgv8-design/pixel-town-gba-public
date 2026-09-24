#include <gba.h>

#define MAP_W 64
#define MAP_H 32

#define WORLD_W (MAP_W * 8)
#define WORLD_H (MAP_H * 8)

#define SCREEN_W 240
#define SCREEN_H 160

/* =========================================================
   BACKGROUND TILE IDS
   ========================================================= */

#define T_EMPTY         0
#define T_GRASS         1
#define T_GRASS_A       2
#define T_GRASS_B       3

#define T_PATH          4
#define T_PATH_LIGHT    5
#define T_PATH_TOP      6
#define T_PATH_BOTTOM   7

#define T_WALL          8
#define T_WALL_SHADOW   9
#define T_WINDOW        10
#define T_DOOR          11

#define T_ROOF          12
#define T_ROOF_LIGHT    13
#define T_ROOF_DARK     14
#define T_ROOF_EDGE     15

#define T_TREE_LIGHT    16
#define T_TREE_MID      17
#define T_TREE_DARK     18
#define T_TRUNK         19

#define T_BUSH          20
#define T_FLOWER        21

#define T_FENCE_H       22
#define T_FENCE_V       23
#define T_FENCE_POST    24

#define T_WATER         25
#define T_WATER_SHINE   26
#define T_BANK_TOP      27
#define T_BANK_BOTTOM   28

#define T_SOIL          29
#define T_STONE         30
#define T_SIGN          31

#define T_ROOF_CORNER_L 32
#define T_ROOF_CORNER_R 33

#define DIR_DOWN  0
#define DIR_UP    1
#define DIR_LEFT  2
#define DIR_RIGHT 3


/* =========================================================
   COLLISION MAP

   Separate from graphics.
   This is important: decoration no longer automatically
   decides where the player can walk.
   ========================================================= */

static u8 collision[MAP_H][MAP_W];


/* =========================================================
   TILE PIXEL HELPERS
   ========================================================= */

static void px(u16 *tile, int x, int y, u8 colour)
{
    int p = y * 8 + x;
    int word = p >> 2;
    int shift = (p & 3) * 4;

    tile[word] &= ~(0xF << shift);
    tile[word] |= (colour & 0xF) << shift;
}


static void fill(u16 *tile, u8 colour)
{
    u16 v =
        colour |
        (colour << 4) |
        (colour << 8) |
        (colour << 12);

    for (int i = 0; i < 16; i++)
        tile[i] = v;
}


/* =========================================================
   TILE GRAPHICS
   ========================================================= */

static void makeTiles(void)
{
    u16 *t = (u16 *)CHAR_BASE_ADR(0);

    /* transparent tile for BG1 */
    fill(&t[T_EMPTY * 16], 0);


    /* -----------------------------------------------------
       GRASS
       ----------------------------------------------------- */

    fill(&t[T_GRASS * 16], 1);

    fill(&t[T_GRASS_A * 16], 1);

    px(&t[T_GRASS_A * 16], 1, 6, 2);
    px(&t[T_GRASS_A * 16], 2, 5, 2);

    px(&t[T_GRASS_A * 16], 6, 2, 2);
    px(&t[T_GRASS_A * 16], 5, 3, 2);


    fill(&t[T_GRASS_B * 16], 1);

    px(&t[T_GRASS_B * 16], 2, 2, 14);
    px(&t[T_GRASS_B * 16], 3, 1, 14);

    px(&t[T_GRASS_B * 16], 6, 6, 2);


    /* -----------------------------------------------------
       PATH
       ----------------------------------------------------- */

    fill(&t[T_PATH * 16], 3);

    px(&t[T_PATH * 16], 1, 2, 4);
    px(&t[T_PATH * 16], 5, 1, 4);
    px(&t[T_PATH * 16], 3, 6, 4);
    px(&t[T_PATH * 16], 7, 4, 4);


    fill(&t[T_PATH_LIGHT * 16], 3);

    px(&t[T_PATH_LIGHT * 16], 2, 1, 5);
    px(&t[T_PATH_LIGHT * 16], 6, 5, 5);
    px(&t[T_PATH_LIGHT * 16], 4, 7, 4);


    fill(&t[T_PATH_TOP * 16], 1);

    for (int x = 0; x < 8; x++)
    {
        px(&t[T_PATH_TOP * 16], x, 5, 4);
        px(&t[T_PATH_TOP * 16], x, 6, 3);
        px(&t[T_PATH_TOP * 16], x, 7, 3);
    }


    fill(&t[T_PATH_BOTTOM * 16], 1);

    for (int x = 0; x < 8; x++)
    {
        px(&t[T_PATH_BOTTOM * 16], x, 0, 3);
        px(&t[T_PATH_BOTTOM * 16], x, 1, 3);
        px(&t[T_PATH_BOTTOM * 16], x, 2, 4);
    }


    /* -----------------------------------------------------
       HOUSE WALL
       ----------------------------------------------------- */

    fill(&t[T_WALL * 16], 6);

    for (int x = 0; x < 8; x++)
    {
        px(&t[T_WALL * 16], x, 3, 5);
        px(&t[T_WALL * 16], x, 7, 5);
    }

    px(&t[T_WALL * 16], 1, 1, 7);
    px(&t[T_WALL * 16], 5, 5, 7);


    fill(&t[T_WALL_SHADOW * 16], 5);

    for (int x = 0; x < 8; x++)
    {
        px(&t[T_WALL_SHADOW * 16], x, 0, 6);
        px(&t[T_WALL_SHADOW * 16], x, 1, 6);
        px(&t[T_WALL_SHADOW * 16], x, 7, 7);
    }


    /* -----------------------------------------------------
       WINDOW
       ----------------------------------------------------- */

    fill(&t[T_WINDOW * 16], 6);

    for (int y = 1; y <= 6; y++)
    {
        for (int x = 1; x <= 6; x++)
            px(&t[T_WINDOW * 16], x, y, 12);
    }

    for (int x = 1; x <= 6; x++)
    {
        px(&t[T_WINDOW * 16], x, 0, 7);
        px(&t[T_WINDOW * 16], x, 7, 7);
    }

    for (int y = 1; y <= 6; y++)
    {
        px(&t[T_WINDOW * 16], 0, y, 7);
        px(&t[T_WINDOW * 16], 7, y, 7);
    }

    for (int y = 1; y <= 6; y++)
        px(&t[T_WINDOW * 16], 4, y, 13);

    for (int x = 1; x <= 6; x++)
        px(&t[T_WINDOW * 16], x, 4, 13);


    /* -----------------------------------------------------
       DOOR
       ----------------------------------------------------- */

    fill(&t[T_DOOR * 16], 11);

    for (int y = 0; y < 8; y++)
    {
        px(&t[T_DOOR * 16], 0, y, 7);
        px(&t[T_DOOR * 16], 7, y, 7);
    }

    for (int x = 0; x < 8; x++)
        px(&t[T_DOOR * 16], x, 0, 7);

    px(&t[T_DOOR * 16], 5, 4, 15);


    /* -----------------------------------------------------
       ROOF
       ----------------------------------------------------- */

    fill(&t[T_ROOF * 16], 8);

    for (int y = 1; y < 8; y += 3)
    {
        for (int x = 0; x < 8; x++)
            px(&t[T_ROOF * 16], x, y, 9);
    }

    px(&t[T_ROOF * 16], 3, 0, 10);
    px(&t[T_ROOF * 16], 3, 1, 10);

    px(&t[T_ROOF * 16], 6, 4, 10);
    px(&t[T_ROOF * 16], 6, 5, 10);


    fill(&t[T_ROOF_LIGHT * 16], 9);

    for (int x = 0; x < 8; x++)
    {
        px(&t[T_ROOF_LIGHT * 16], x, 6, 8);
        px(&t[T_ROOF_LIGHT * 16], x, 7, 8);
    }


    fill(&t[T_ROOF_DARK * 16], 10);

    for (int x = 0; x < 8; x++)
        px(&t[T_ROOF_DARK * 16], x, 0, 8);


    fill(&t[T_ROOF_EDGE * 16], 8);

    for (int x = 0; x < 8; x++)
    {
        px(&t[T_ROOF_EDGE * 16], x, 3, 10);
        px(&t[T_ROOF_EDGE * 16], x, 4, 10);
        px(&t[T_ROOF_EDGE * 16], x, 5, 7);
        px(&t[T_ROOF_EDGE * 16], x, 6, 7);
        px(&t[T_ROOF_EDGE * 16], x, 7, 7);
    }


    /* roof corners */

    fill(&t[T_ROOF_CORNER_L * 16], 1);

    for (int y = 2; y < 8; y++)
    {
        int start = 7 - y;

        if (start < 0)
            start = 0;

        for (int x = start; x < 8; x++)
            px(&t[T_ROOF_CORNER_L * 16], x, y, 8);
    }

    for (int y = 5; y < 8; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            if (x >= 7 - y)
                px(&t[T_ROOF_CORNER_L * 16], x, y, 7);
        }
    }


    fill(&t[T_ROOF_CORNER_R * 16], 1);

    for (int y = 2; y < 8; y++)
    {
        int end = y;

        if (end > 7)
            end = 7;

        for (int x = 0; x <= end; x++)
            px(&t[T_ROOF_CORNER_R * 16], x, y, 8);
    }

    for (int y = 5; y < 8; y++)
    {
        for (int x = 0; x <= y && x < 8; x++)
            px(&t[T_ROOF_CORNER_R * 16], x, y, 7);
    }


    /* -----------------------------------------------------
       TREES

       These tiles are designed to overlap into a larger
       multi-tile tree instead of looking like a cross.
       ----------------------------------------------------- */

    fill(&t[T_TREE_LIGHT * 16], 0);

    for (int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            int dx = x - 4;
            int dy = y - 4;

            if (dx * dx + dy * dy <= 22)
            {
                u8 c =
                    ((x + y) % 3 == 0)
                    ? 15
                    : 14;

                px(
                    &t[T_TREE_LIGHT * 16],
                    x,
                    y,
                    c
                );
            }
        }
    }

    px(&t[T_TREE_LIGHT * 16], 3, 2, 2);
    px(&t[T_TREE_LIGHT * 16], 5, 4, 2);


    fill(&t[T_TREE_MID * 16], 0);

    for (int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            int dx = x - 3;
            int dy = y - 3;

            if (dx * dx + dy * dy <= 25)
            {
                u8 c =
                    ((x + y) & 1)
                    ? 14
                    : 15;

                px(
                    &t[T_TREE_MID * 16],
                    x,
                    y,
                    c
                );
            }
        }
    }


    fill(&t[T_TREE_DARK * 16], 0);

    for (int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            int dx = x - 4;
            int dy = y - 3;

            if (dx * dx + dy * dy <= 24)
            {
                u8 c =
                    ((x + y) & 1)
                    ? 14
                    : 2;

                px(
                    &t[T_TREE_DARK * 16],
                    x,
                    y,
                    c
                );
            }
        }
    }


    fill(&t[T_TRUNK * 16], 0);

    for (int y = 0; y < 8; y++)
    {
        px(&t[T_TRUNK * 16], 3, y, 11);
        px(&t[T_TRUNK * 16], 4, y, 11);
    }

    px(&t[T_TRUNK * 16], 2, 7, 11);
    px(&t[T_TRUNK * 16], 5, 7, 11);


    /* -----------------------------------------------------
       BUSH
       ----------------------------------------------------- */

    fill(&t[T_BUSH * 16], 0);

    for (int y = 2; y <= 6; y++)
    {
        for (int x = 1; x <= 6; x++)
        {
            if (!(
                y == 2 &&
                (x == 1 || x == 6)
            ))
            {
                px(
                    &t[T_BUSH * 16],
                    x,
                    y,
                    ((x + y) & 1)
                        ? 14
                        : 15
                );
            }
        }
    }


    /* -----------------------------------------------------
       FLOWERS
       ----------------------------------------------------- */

    fill(&t[T_FLOWER * 16], 0);

    px(&t[T_FLOWER * 16], 2, 2, 13);
    px(&t[T_FLOWER * 16], 3, 2, 10);
    px(&t[T_FLOWER * 16], 2, 3, 10);

    px(&t[T_FLOWER * 16], 6, 5, 13);
    px(&t[T_FLOWER * 16], 5, 5, 10);

    px(&t[T_FLOWER * 16], 2, 4, 14);
    px(&t[T_FLOWER * 16], 5, 7, 14);


    /* -----------------------------------------------------
       FENCES
       ----------------------------------------------------- */

    fill(&t[T_FENCE_H * 16], 0);

    for (int x = 0; x < 8; x++)
    {
        px(&t[T_FENCE_H * 16], x, 3, 11);
        px(&t[T_FENCE_H * 16], x, 5, 11);
    }


    fill(&t[T_FENCE_V * 16], 0);

    for (int y = 0; y < 8; y++)
    {
        px(&t[T_FENCE_V * 16], 3, y, 11);
        px(&t[T_FENCE_V * 16], 5, y, 11);
    }


    fill(&t[T_FENCE_POST * 16], 0);

    for (int y = 1; y < 8; y++)
    {
        px(&t[T_FENCE_POST * 16], 2, y, 11);
        px(&t[T_FENCE_POST * 16], 3, y, 11);
        px(&t[T_FENCE_POST * 16], 4, y, 11);
        px(&t[T_FENCE_POST * 16], 5, y, 11);
    }

    for (int x = 1; x <= 6; x++)
        px(&t[T_FENCE_POST * 16], x, 1, 7);


    /* -----------------------------------------------------
       WATER
       ----------------------------------------------------- */

    fill(&t[T_WATER * 16], 12);

    px(&t[T_WATER * 16], 1, 2, 13);
    px(&t[T_WATER * 16], 2, 2, 13);

    px(&t[T_WATER * 16], 5, 6, 13);
    px(&t[T_WATER * 16], 6, 6, 13);


    fill(&t[T_WATER_SHINE * 16], 12);

    for (int x = 1; x <= 5; x++)
        px(&t[T_WATER_SHINE * 16], x, 3, 13);

    for (int x = 3; x <= 7; x++)
        px(&t[T_WATER_SHINE * 16], x, 7, 13);


    fill(&t[T_BANK_TOP * 16], 4);

    for (int x = 0; x < 8; x++)
    {
        px(&t[T_BANK_TOP * 16], x, 5, 3);
        px(&t[T_BANK_TOP * 16], x, 6, 12);
        px(&t[T_BANK_TOP * 16], x, 7, 12);
    }


    fill(&t[T_BANK_BOTTOM * 16], 12);

    for (int x = 0; x < 8; x++)
    {
        px(&t[T_BANK_BOTTOM * 16], x, 5, 4);
        px(&t[T_BANK_BOTTOM * 16], x, 6, 3);
        px(&t[T_BANK_BOTTOM * 16], x, 7, 1);
    }


    /* -----------------------------------------------------
       SOIL
       ----------------------------------------------------- */

    fill(&t[T_SOIL * 16], 4);

    for (int y = 1; y < 8; y += 3)
    {
        for (int x = 0; x < 8; x++)
            px(&t[T_SOIL * 16], x, y, 3);
    }


    /* -----------------------------------------------------
       STONE
       ----------------------------------------------------- */

    fill(&t[T_STONE * 16], 0);

    for (int y = 3; y <= 6; y++)
    {
        for (int x = 2; x <= 5; x++)
            px(&t[T_STONE * 16], x, y, 5);
    }

    px(&t[T_STONE * 16], 3, 3, 6);
    px(&t[T_STONE * 16], 4, 3, 6);


    /* -----------------------------------------------------
       SIGN
       ----------------------------------------------------- */

    fill(&t[T_SIGN * 16], 0);

    for (int y = 1; y <= 4; y++)
    {
        for (int x = 1; x <= 6; x++)
            px(&t[T_SIGN * 16], x, y, 11);
    }

    px(&t[T_SIGN * 16], 2, 2, 7);
    px(&t[T_SIGN * 16], 5, 2, 7);

    px(&t[T_SIGN * 16], 3, 5, 11);
    px(&t[T_SIGN * 16], 4, 5, 11);
    px(&t[T_SIGN * 16], 3, 6, 11);
    px(&t[T_SIGN * 16], 4, 6, 11);
    px(&t[T_SIGN * 16], 3, 7, 11);
    px(&t[T_SIGN * 16], 4, 7, 11);
}


/* =========================================================
   TWO BACKGROUND MAPS

   BG0 = ground / buildings / trunks
   BG1 = foreground canopy / roof overhangs / decoration

   Both are 64x32 maps.
   ========================================================= */

static void setMapBlock(
    int bg,
    int x,
    int y,
    int tile
)
{
    if (
        x < 0 ||
        y < 0 ||
        x >= MAP_W ||
        y >= MAP_H
    )
        return;

    int base;

    if (bg == 0)
        base = 28;
    else
        base = 30;

    if (x < 32)
    {
        u16 *map =
            (u16 *)SCREEN_BASE_BLOCK(base);

        map[y * 32 + x] = tile;
    }
    else
    {
        u16 *map =
            (u16 *)SCREEN_BASE_BLOCK(base + 1);

        map[
            y * 32 +
            (x - 32)
        ] = tile;
    }
}


static void ground(
    int x,
    int y,
    int tile
)
{
    setMapBlock(
        0,
        x,
        y,
        tile
    );
}


static void foreground(
    int x,
    int y,
    int tile
)
{
    setMapBlock(
        1,
        x,
        y,
        tile
    );
}


static void setSolid(
    int x,
    int y,
    int solid
)
{
    if (
        x < 0 ||
        y < 0 ||
        x >= MAP_W ||
        y >= MAP_H
    )
        return;

    collision[y][x] =
        solid ? 1 : 0;
}


/* =========================================================
   HOUSE

   Wider, with roof overhang and independent collision.
   ========================================================= */

static void makeHouse(
    int x,
    int y,
    int width
)
{
    int right =
        x + width - 1;

    /*
       roof top
    */

    foreground(
        x - 1,
        y + 1,
        T_ROOF_CORNER_L
    );

    foreground(
        right + 1,
        y + 1,
        T_ROOF_CORNER_R
    );

    for (
        int xx = x;
        xx <= right;
        xx++
    )
    {
        foreground(
            xx,
            y,
            T_ROOF_LIGHT
        );

        foreground(
            xx,
            y + 1,
            T_ROOF
        );

        foreground(
            xx,
            y + 2,
            T_ROOF
        );

        foreground(
            xx,
            y + 3,
            T_ROOF_DARK
        );
    }

    /*
       roof overhang
    */

    for (
        int xx = x - 1;
        xx <= right + 1;
        xx++
    )
    {
        foreground(
            xx,
            y + 4,
            T_ROOF_EDGE
        );
    }

    /*
       facade
    */

    for (
        int yy = y + 5;
        yy <= y + 8;
        yy++
    )
    {
        for (
            int xx = x;
            xx <= right;
            xx++
        )
        {
            ground(
                xx,
                yy,
                T_WALL
            );

            setSolid(
                xx,
                yy,
                1
            );
        }
    }

    /*
       shadow base
    */

    for (
        int xx = x;
        xx <= right;
        xx++
    )
    {
        ground(
            xx,
            y + 8,
            T_WALL_SHADOW
        );
    }

    /*
       windows
    */

    ground(
        x + 1,
        y + 6,
        T_WINDOW
    );

    ground(
        x + 2,
        y + 6,
        T_WINDOW
    );

    ground(
        right - 2,
        y + 6,
        T_WINDOW
    );

    ground(
        right - 1,
        y + 6,
        T_WINDOW
    );

    /*
       door
    */

    int door =
        x + width / 2 - 1;

    ground(
        door,
        y + 7,
        T_DOOR
    );

    ground(
        door + 1,
        y + 7,
        T_DOOR
    );

    ground(
        door,
        y + 8,
        T_DOOR
    );

    ground(
        door + 1,
        y + 8,
        T_DOOR
    );

    /*
       Door tiles themselves remain blocked for now.
       The path IN FRONT is guaranteed clear.
    */

    for (
        int yy = y + 9;
        yy <= y + 12;
        yy++
    )
    {
        ground(
            door,
            yy,
            T_PATH
        );

        ground(
            door + 1,
            yy,
            T_PATH_LIGHT
        );

        setSolid(
            door,
            yy,
            0
        );

        setSolid(
            door + 1,
            yy,
            0
        );
    }

    /*
       roof collision
    */

    for (
        int yy = y;
        yy <= y + 4;
        yy++
    )
    {
        for (
            int xx = x - 1;
            xx <= right + 1;
            xx++
        )
        {
            setSolid(
                xx,
                yy,
                1
            );
        }
    }
}


/* =========================================================
   TREE

   32x40-ish visual tree.

   Trunk lives on BG0.
   Canopy lives on BG1.

   This means the player can visually pass underneath
   the lower crown instead of every tree being a flat block.
   ========================================================= */

static void makeTree(
    int x,
    int y
)
{
    /*
       trunk
    */

    ground(
        x + 1,
        y + 3,
        T_TRUNK
    );

    ground(
        x + 2,
        y + 3,
        T_TRUNK
    );

    /*
       lower dark foliage
    */

    foreground(
        x,
        y + 2,
        T_TREE_DARK
    );

    foreground(
        x + 1,
        y + 2,
        T_TREE_MID
    );

    foreground(
        x + 2,
        y + 2,
        T_TREE_MID
    );

    foreground(
        x + 3,
        y + 2,
        T_TREE_DARK
    );

    /*
       middle crown
    */

    foreground(
        x,
        y + 1,
        T_TREE_MID
    );

    foreground(
        x + 1,
        y + 1,
        T_TREE_LIGHT
    );

    foreground(
        x + 2,
        y + 1,
        T_TREE_LIGHT
    );

    foreground(
        x + 3,
        y + 1,
        T_TREE_MID
    );

    /*
       upper crown
    */

    foreground(
        x + 1,
        y,
        T_TREE_LIGHT
    );

    foreground(
        x + 2,
        y,
        T_TREE_LIGHT
    );

    /*
       Collision only around trunk/base.
    */

    setSolid(
        x + 1,
        y + 3,
        1
    );

    setSolid(
        x + 2,
        y + 3,
        1
    );
}


/* =========================================================
   BUSH
   ========================================================= */

static void makeBush(
    int x,
    int y
)
{
    foreground(
        x,
        y,
        T_BUSH
    );

    setSolid(
        x,
        y,
        1
    );
}


/* =========================================================
   FENCE

   Gate stays open.
   ========================================================= */

static void makeFenceYard(
    int x,
    int y,
    int w,
    int h,
    int gate
)
{
    /*
       soil / lawn beds
    */

    for (
        int yy = y + 1;
        yy < y + h - 1;
        yy++
    )
    {
        for (
            int xx = x + 1;
            xx < x + w - 1;
            xx++
        )
        {
            if (
                (xx + yy) % 3 == 0
            )
            {
                ground(
                    xx,
                    yy,
                    T_SOIL
                );
            }
        }
    }

    /*
       top
    */

    for (
        int xx = x;
        xx < x + w;
        xx++
    )
    {
        foreground(
            xx,
            y,
            T_FENCE_H
        );

        setSolid(
            xx,
            y,
            1
        );
    }

    /*
       bottom + gate
    */

    for (
        int xx = x;
        xx < x + w;
        xx++
    )
    {
        if (
            xx == gate ||
            xx == gate + 1
        )
            continue;

        foreground(
            xx,
            y + h - 1,
            T_FENCE_H
        );

        setSolid(
            xx,
            y + h - 1,
            1
        );
    }

    /*
       sides
    */

    for (
        int yy = y;
        yy < y + h;
        yy++
    )
    {
        foreground(
            x,
            yy,
            T_FENCE_V
        );

        foreground(
            x + w - 1,
            yy,
            T_FENCE_V
        );

        setSolid(
            x,
            yy,
            1
        );

        setSolid(
            x + w - 1,
            yy,
            1
        );
    }

    /*
       posts
    */

    foreground(
        x,
        y,
        T_FENCE_POST
    );

    foreground(
        x + w - 1,
        y,
        T_FENCE_POST
    );

    foreground(
        x,
        y + h - 1,
        T_FENCE_POST
    );

    foreground(
        x + w - 1,
        y + h - 1,
        T_FENCE_POST
    );

    /*
       gate clear
    */

    foreground(
        gate,
        y + h - 1,
        T_EMPTY
    );

    foreground(
        gate + 1,
        y + h - 1,
        T_EMPTY
    );

    setSolid(
        gate,
        y + h - 1,
        0
    );

    setSolid(
        gate + 1,
        y + h - 1,
        0
    );
}


/* =========================================================
   POND

   Larger irregular pond.
   ========================================================= */

static void makePond(
    int x,
    int y,
    int w,
    int h
)
{
    /*
       water body
    */

    for (
        int yy = y;
        yy < y + h;
        yy++
    )
    {
        for (
            int xx = x;
            xx < x + w;
            xx++
        )
        {
            int edge =
                xx == x ||
                xx == x + w - 1 ||
                yy == y ||
                yy == y + h - 1;

            if (edge)
            {
                /*
                   Rounded corners stay grass.
                */

                int corner =
                    (xx == x ||
                     xx == x + w - 1) &&
                    (yy == y ||
                     yy == y + h - 1);

                if (corner)
                    continue;
            }

            int tile =
                ((xx + yy) % 4 == 0)
                ? T_WATER_SHINE
                : T_WATER;

            ground(
                xx,
                yy,
                tile
            );

            setSolid(
                xx,
                yy,
                1
            );
        }
    }

    /*
       top bank
    */

    for (
        int xx = x + 1;
        xx < x + w - 1;
        xx++
    )
    {
        ground(
            xx,
            y,
            T_BANK_TOP
        );
    }

    /*
       bottom bank
    */

    for (
        int xx = x + 1;
        xx < x + w - 1;
        xx++
    )
    {
        ground(
            xx,
            y + h - 1,
            T_BANK_BOTTOM
        );
    }
}


/* =========================================================
   FLOWER PATCH
   ========================================================= */

static void flowers(
    int x,
    int y,
    int w,
    int h
)
{
    for (
        int yy = y;
        yy < y + h;
        yy++
    )
    {
        for (
            int xx = x;
            xx < x + w;
            xx++
        )
        {
            if (
                ((xx + yy) & 1) == 0
            )
            {
                foreground(
                    xx,
                    yy,
                    T_FLOWER
                );
            }
        }
    }
}


/* =========================================================
   WORLD
   ========================================================= */

static void makeWorld(void)
{
    /*
       clear collision and maps
    */

    for (
        int y = 0;
        y < MAP_H;
        y++
    )
    {
        for (
            int x = 0;
            x < MAP_W;
            x++
        )
        {
            collision[y][x] = 0;

            int grassTile =
                T_GRASS;

            int n =
                (x * 13 +
                 y * 7) % 37;

            if (n == 0)
                grassTile =
                    T_GRASS_A;

            if (n == 11)
                grassTile =
                    T_GRASS_B;

            ground(
                x,
                y,
                grassTile
            );

            foreground(
                x,
                y,
                T_EMPTY
            );
        }
    }


    /* =====================================================
       MAIN EAST/WEST ROAD
       ===================================================== */

    for (
        int x = 0;
        x < MAP_W;
        x++
    )
    {
        ground(
            x,
            14,
            T_PATH_TOP
        );

        ground(
            x,
            15,
            T_PATH
        );

        ground(
            x,
            16,
            ((x & 3) == 0)
                ? T_PATH_LIGHT
                : T_PATH
        );

        ground(
            x,
            17,
            T_PATH
        );

        ground(
            x,
            18,
            T_PATH_BOTTOM
        );
    }


    /* =====================================================
       NORTH/SOUTH ROAD
       ===================================================== */

    for (
        int y = 0;
        y < MAP_H;
        y++
    )
    {
        ground(
            30,
            y,
            T_PATH
        );

        ground(
            31,
            y,
            ((y & 3) == 0)
                ? T_PATH_LIGHT
                : T_PATH
        );

        ground(
            32,
            y,
            T_PATH
        );
    }


    /* =====================================================
       NORTH HOUSES
       ===================================================== */

    makeHouse(
        3,
        3,
        9
    );

    makeHouse(
        17,
        3,
        10
    );

    makeHouse(
        36,
        3,
        9
    );

    makeHouse(
        50,
        3,
        10
    );


    /* =====================================================
       NORTH GARDENS
       ===================================================== */

    makeFenceYard(
        13,
        5,
        4,
        7,
        14
    );

    flowers(
        14,
        7,
        2,
        3
    );


    makeFenceYard(
        46,
        5,
        4,
        7,
        47
    );

    flowers(
        47,
        7,
        2,
        3
    );


    /* =====================================================
       TREE GROUPS

       Deliberately uneven spacing.
       ===================================================== */

    makeTree(
        0,
        0
    );

    makeTree(
        27,
        1
    );

    makeTree(
        60,
        0
    );

    makeTree(
        12,
        0
    );

    makeTree(
        45,
        0
    );


    /*
       Decorative bushes, not at doors.
    */

    makeBush(
        1,
        12
    );

    makeBush(
        11,
        12
    );

    makeBush(
        25,
        12
    );

    makeBush(
        37,
        12
    );

    makeBush(
        44,
        12
    );

    makeBush(
        58,
        12
    );


    /* =====================================================
       SOUTH HOUSES
       ===================================================== */

    makeHouse(
        3,
        22,
        10
    );

    makeHouse(
        18,
        22,
        9
    );

    makeHouse(
        38,
        22,
        9
    );

    makeHouse(
        52,
        22,
        9
    );


    /* =====================================================
       PARK / POND
       ===================================================== */

    makePond(
        29,
        21,
        7,
        8
    );


    flowers(
        27,
        21,
        2,
        4
    );

    flowers(
        36,
        22,
        2,
        3
    );


    /*
       Park trees.
    */

    makeTree(
        14,
        20
    );

    makeTree(
        47,
        20
    );

    makeTree(
        0,
        21
    );

    makeTree(
        60,
        21
    );


    /* =====================================================
       SMALL SOUTH GARDENS
       ===================================================== */

    makeFenceYard(
        4,
        29,
        9,
        3,
        7
    );

    makeFenceYard(
        39,
        29,
        8,
        3,
        42
    );


    /* =====================================================
       PROPS
       ===================================================== */

    foreground(
        27,
        13,
        T_SIGN
    );

    setSolid(
        27,
        13,
        1
    );


    foreground(
        37,
        20,
        T_STONE
    );

    foreground(
        24,
        20,
        T_STONE
    );


    /* =====================================================
       FINAL DOOR SAFETY PASS

       This runs LAST.

       No bush, fence, flower or tree can remain in these
       approaches.
       ===================================================== */

    const int northDoors[8] =
    {
        6, 7,
        21, 22,
        39, 40,
        54, 55
    };

    for (
        int i = 0;
        i < 8;
        i++
    )
    {
        int x =
            northDoors[i];

        for (
            int y = 12;
            y <= 14;
            y++
        )
        {
            ground(
                x,
                y,
                T_PATH
            );

            foreground(
                x,
                y,
                T_EMPTY
            );

            setSolid(
                x,
                y,
                0
            );
        }
    }


    /*
       Central intersection is always free.
    */

    for (
        int y = 13;
        y <= 19;
        y++
    )
    {
        for (
            int x = 29;
            x <= 33;
            x++
        )
        {
            ground(
                x,
                y,
                T_PATH
            );

            foreground(
                x,
                y,
                T_EMPTY
            );

            setSolid(
                x,
                y,
                0
            );
        }
    }
}


/* =========================================================
   COLLISION
   ========================================================= */

static int solidAt(
    int tx,
    int ty
)
{
    if (
        tx < 0 ||
        ty < 0 ||
        tx >= MAP_W ||
        ty >= MAP_H
    )
        return 1;

    return
        collision[ty][tx];
}


static int blocked(
    int x,
    int y
)
{
    /*
       Only player's feet collide.
    */

    int left =
        x + 4;

    int right =
        x + 11;

    int top =
        y + 24;

    int bottom =
        y + 28;

    if (
        solidAt(
            left / 8,
            top / 8
        )
    )
        return 1;

    if (
        solidAt(
            right / 8,
            top / 8
        )
    )
        return 1;

    if (
        solidAt(
            left / 8,
            bottom / 8
        )
    )
        return 1;

    if (
        solidAt(
            right / 8,
            bottom / 8
        )
    )
        return 1;

    return 0;
}


/* =========================================================
   PLAYER GRAPHICS
   ========================================================= */

static void objPixel(
    u16 *base,
    int x,
    int y,
    u8 colour
)
{
    if (
        x < 0 ||
        x >= 16 ||
        y < 0 ||
        y >= 32
    )
        return;

    int tileX =
        x >> 3;

    int tileY =
        y >> 3;

    int localX =
        x & 7;

    int localY =
        y & 7;

    int tile =
        tileY * 2 +
        tileX;

    px(
        &base[tile * 16],
        localX,
        localY,
        colour
    );
}


static void clearPlayer(
    u16 *base
)
{
    for (
        int i = 0;
        i < 128;
        i++
    )
        base[i] = 0;
}


static void playerFrame(
    u16 *base,
    int direction,
    int frame
)
{
    clearPlayer(
        base
    );

    int outline = 1;
    int hair = 2;
    int skin = 3;
    int shirt = 4;
    int pants = 5;
    int shoes = 6;
    int shine = 7;
    int shadow = 8;


    /*
       ground shadow
    */

    for (
        int x = 4;
        x <= 11;
        x++
    )
        objPixel(
            base,
            x,
            29,
            shadow
        );


    /*
       head
    */

    for (
        int y = 7;
        y <= 12;
        y++
    )
    {
        for (
            int x = 5;
            x <= 10;
            x++
        )
            objPixel(
                base,
                x,
                y,
                skin
            );
    }


    /*
       hair
    */

    for (
        int x = 5;
        x <= 10;
        x++
    )
    {
        objPixel(
            base,
            x,
            6,
            hair
        );

        objPixel(
            base,
            x,
            7,
            hair
        );
    }

    objPixel(
        base,
        4,
        8,
        hair
    );

    objPixel(
        base,
        11,
        8,
        hair
    );


    /*
       face direction
    */

    if (
        direction ==
        DIR_DOWN
    )
    {
        objPixel(
            base,
            6,
            10,
            outline
        );

        objPixel(
            base,
            9,
            10,
            outline
        );
    }
    else if (
        direction ==
        DIR_UP
    )
    {
        for (
            int x = 5;
            x <= 10;
            x++
        )
            objPixel(
                base,
                x,
                10,
                hair
            );
    }
    else if (
        direction ==
        DIR_LEFT
    )
    {
        objPixel(
            base,
            5,
            10,
            outline
        );
    }
    else
    {
        objPixel(
            base,
            10,
            10,
            outline
        );
    }


    /*
       shirt
    */

    for (
        int y = 14;
        y <= 20;
        y++
    )
    {
        for (
            int x = 5;
            x <= 10;
            x++
        )
            objPixel(
                base,
                x,
                y,
                shirt
            );
    }

    objPixel(
        base,
        6,
        15,
        shine
    );

    objPixel(
        base,
        6,
        16,
        shine
    );


    /*
       arms
    */

    int a =
        frame ? 1 : 0;

    int b =
        frame ? 0 : 1;

    for (
        int y = 15;
        y <= 20;
        y++
    )
    {
        objPixel(
            base,
            3,
            y + a,
            skin
        );

        objPixel(
            base,
            4,
            y + a,
            skin
        );

        objPixel(
            base,
            11,
            y + b,
            skin
        );

        objPixel(
            base,
            12,
            y + b,
            skin
        );
    }


    /*
       waist
    */

    for (
        int y = 21;
        y <= 23;
        y++
    )
    {
        for (
            int x = 5;
            x <= 10;
            x++
        )
            objPixel(
                base,
                x,
                y,
                pants
            );
    }


    /*
       legs
    */

    if (
        frame == 0
    )
    {
        for (
            int y = 24;
            y <= 27;
            y++
        )
        {
            objPixel(
                base,
                5,
                y,
                pants
            );

            objPixel(
                base,
                6,
                y,
                pants
            );

            objPixel(
                base,
                9,
                y,
                pants
            );

            objPixel(
                base,
                10,
                y,
                pants
            );
        }

        objPixel(
            base,
            4,
            28,
            shoes
        );

        objPixel(
            base,
            5,
            28,
            shoes
        );

        objPixel(
            base,
            6,
            28,
            shoes
        );

        objPixel(
            base,
            9,
            28,
            shoes
        );

        objPixel(
            base,
            10,
            28,
            shoes
        );

        objPixel(
            base,
            11,
            28,
            shoes
        );
    }
    else
    {
        for (
            int y = 24;
            y <= 27;
            y++
        )
        {
            objPixel(
                base,
                4,
                y,
                pants
            );

            objPixel(
                base,
                5,
                y,
                pants
            );

            objPixel(
                base,
                10,
                y,
                pants
            );

            objPixel(
                base,
                11,
                y,
                pants
            );
        }

        objPixel(
            base,
            3,
            28,
            shoes
        );

        objPixel(
            base,
            4,
            28,
            shoes
        );

        objPixel(
            base,
            5,
            28,
            shoes
        );

        objPixel(
            base,
            10,
            28,
            shoes
        );

        objPixel(
            base,
            11,
            28,
            shoes
        );

        objPixel(
            base,
            12,
            28,
            shoes
        );
    }
}


/* =========================================================
   PALETTES
   ========================================================= */

static void makePalettes(void)
{
    /*
       BG palette

       0 must be transparent on BG1.
    */

    BG_PALETTE[0] =
        RGB5(0,0,0);

    /* grass */
    BG_PALETTE[1] =
        RGB5(10,23,10);

    BG_PALETTE[2] =
        RGB5(6,18,7);

    /* path */
    BG_PALETTE[3] =
        RGB5(25,21,13);

    BG_PALETTE[4] =
        RGB5(29,25,17);

    /* building */
    BG_PALETTE[5] =
        RGB5(25,21,15);

    BG_PALETTE[6] =
        RGB5(29,25,19);

    BG_PALETTE[7] =
        RGB5(16,12,8);

    /* roof */
    BG_PALETTE[8] =
        RGB5(23,6,5);

    BG_PALETTE[9] =
        RGB5(30,11,8);

    BG_PALETTE[10] =
        RGB5(15,3,3);

    /* wood */
    BG_PALETTE[11] =
        RGB5(12,7,3);

    /* water/window */
    BG_PALETTE[12] =
        RGB5(7,18,28);

    BG_PALETTE[13] =
        RGB5(18,28,31);

    /* foliage */
    BG_PALETTE[14] =
        RGB5(3,16,5);

    BG_PALETTE[15] =
        RGB5(9,27,10);


    /*
       Player
    */

    SPRITE_PALETTE[0] =
        RGB5(31,0,31);

    SPRITE_PALETTE[1] =
        RGB5(3,3,4);

    SPRITE_PALETTE[2] =
        RGB5(9,5,3);

    SPRITE_PALETTE[3] =
        RGB5(30,22,16);

    SPRITE_PALETTE[4] =
        RGB5(5,12,28);

    SPRITE_PALETTE[5] =
        RGB5(5,7,14);

    SPRITE_PALETTE[6] =
        RGB5(3,3,4);

    SPRITE_PALETTE[7] =
        RGB5(18,22,31);

    SPRITE_PALETTE[8] =
        RGB5(4,12,4);
}


/* =========================================================
   CLAMP
   ========================================================= */

static int clampInt(
    int value,
    int min,
    int max
)
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

    irqEnable(
        IRQ_VBLANK
    );


    /*
       Mode 0:
       BG0 ground
       BG1 foreground
       OBJ player
    */

    SetMode(
        MODE_0 |
        BG0_ON |
        BG1_ON |
        OBJ_ON |
        OBJ_1D_MAP
    );


    makePalettes();


    /*
       BG0:
       screen blocks 28 + 29
       lower priority
    */

    REG_BG0CNT =
        BG_PRIORITY(2) |
        CHAR_BASE(0) |
        SCREEN_BASE(28) |
        BG_16_COLOR |
        BG_SIZE_1;


    /*
       BG1:
       screen blocks 30 + 31
       higher priority than sprite.

       Transparent colour 0 allows the player
       and BG0 to show through empty areas.
    */

    REG_BG1CNT =
        BG_PRIORITY(0) |
        CHAR_BASE(0) |
        SCREEN_BASE(30) |
        BG_16_COLOR |
        BG_SIZE_1;


    makeTiles();

    makeWorld();


    /*
       Player
    */

    u16 *playerTiles =
        (u16 *)SPRITE_GFX;

    int playerX =
        244;

    int playerY =
        92;

    int direction =
        DIR_DOWN;

    int moving =
        0;

    int walkFrame =
        0;

    int walkTimer =
        0;


    playerFrame(
        playerTiles,
        direction,
        0
    );


    /*
       Disable unused sprites
    */

    for (
        int i = 1;
        i < 128;
        i++
    )
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


        int nx =
            playerX;

        int ny =
            playerY;

        moving =
            0;


        /*
           Cardinal movement
        */

        if (
            keys & KEY_LEFT
        )
        {
            nx--;

            direction =
                DIR_LEFT;

            moving = 1;
        }
        else if (
            keys & KEY_RIGHT
        )
        {
            nx++;

            direction =
                DIR_RIGHT;

            moving = 1;
        }
        else if (
            keys & KEY_UP
        )
        {
            ny--;

            direction =
                DIR_UP;

            moving = 1;
        }
        else if (
            keys & KEY_DOWN
        )
        {
            ny++;

            direction =
                DIR_DOWN;

            moving = 1;
        }


        nx =
            clampInt(
                nx,
                0,
                WORLD_W - 16
            );

        ny =
            clampInt(
                ny,
                0,
                WORLD_H - 32
            );


        /*
           Collision
        */

        if (
            !blocked(
                nx,
                playerY
            )
        )
        {
            playerX =
                nx;
        }

        if (
            !blocked(
                playerX,
                ny
            )
        )
        {
            playerY =
                ny;
        }


        /*
           animation
        */

        if (
            moving
        )
        {
            walkTimer++;

            if (
                walkTimer >= 10
            )
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
            moving
                ? walkFrame
                : 0
        );


        /*
           camera
        */

        int cameraX =
            playerX +
            8 -
            SCREEN_W / 2;

        int cameraY =
            playerY +
            16 -
            SCREEN_H / 2;


        cameraX =
            clampInt(
                cameraX,
                0,
                WORLD_W -
                SCREEN_W
            );

        cameraY =
            clampInt(
                cameraY,
                0,
                WORLD_H -
                SCREEN_H
            );


        /*
           Both layers scroll together.
        */

        REG_BG0HOFS =
            cameraX;

        REG_BG0VOFS =
            cameraY;

        REG_BG1HOFS =
            cameraX;

        REG_BG1VOFS =
            cameraY;


        /*
           player screen position
        */

        int screenX =
            playerX -
            cameraX;

        int screenY =
            playerY -
            cameraY;


        OAM[0].attr0 =
            ATTR0_COLOR_16 |
            ATTR0_TALL |
            (screenY & 0xFF);

        OAM[0].attr1 =
            ATTR1_SIZE_32 |
            (screenX & 0x1FF);

        /*
           Priority 1:
           BG1 priority 0 can cover player.
           BG0 priority 2 stays behind player.
        */

        OAM[0].attr2 =
            ATTR2_PRIORITY(1);
    }


    return 0;
}
