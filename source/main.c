#include <gba.h>

#define MAP_W 64
#define MAP_H 32
#define WORLD_W (MAP_W * 8)
#define WORLD_H (MAP_H * 8)

#define SCREEN_W 240
#define SCREEN_H 160

/* =========================================================
   TILE IDS
   ========================================================= */

#define T_GRASS        0
#define T_GRASS_DETAIL 1
#define T_PATH         2
#define T_PATH_EDGE    3

#define T_WALL         4
#define T_WALL_SHADOW  5
#define T_ROOF         6
#define T_ROOF_LIGHT   7
#define T_ROOF_EDGE    8
#define T_DOOR         9
#define T_WINDOW       10

#define T_TREE_LIGHT   11
#define T_TREE_DARK    12
#define T_TRUNK        13

#define T_BUSH         14
#define T_FLOWER       15

#define T_FENCE_H      16
#define T_FENCE_V      17

#define T_WATER        18
#define T_WATER_LIGHT  19
#define T_WATER_EDGE   20

#define T_STONE        21
#define T_SIGN         22
#define T_GARDEN       23

#define DIR_DOWN  0
#define DIR_UP    1
#define DIR_LEFT  2
#define DIR_RIGHT 3

/* =========================================================
   TILE HELPERS
   ========================================================= */

static void tilePixel(u16 *tile, int x, int y, u8 c)
{
    int p = y * 8 + x;
    int word = p >> 2;
    int shift = (p & 3) * 4;

    tile[word] &= ~(0xF << shift);
    tile[word] |= (c & 0xF) << shift;
}

static void fillTile(u16 *tile, u8 c)
{
    u16 v =
        c |
        (c << 4) |
        (c << 8) |
        (c << 12);

    for (int i = 0; i < 16; i++)
        tile[i] = v;
}

/* =========================================================
   TILE GRAPHICS
   ========================================================= */

static void makeTiles(void)
{
    u16 *t = (u16 *)CHAR_BASE_ADR(0);

    /* GRASS */
    fillTile(&t[T_GRASS * 16], 1);

    /* GRASS DETAIL */
    fillTile(&t[T_GRASS_DETAIL * 16], 1);

    tilePixel(&t[T_GRASS_DETAIL * 16], 1, 2, 2);
    tilePixel(&t[T_GRASS_DETAIL * 16], 2, 1, 2);

    tilePixel(&t[T_GRASS_DETAIL * 16], 6, 5, 2);
    tilePixel(&t[T_GRASS_DETAIL * 16], 5, 6, 2);

    /* PATH */
    fillTile(&t[T_PATH * 16], 3);

    tilePixel(&t[T_PATH * 16], 1, 1, 4);
    tilePixel(&t[T_PATH * 16], 6, 3, 4);
    tilePixel(&t[T_PATH * 16], 3, 6, 4);

    /* PATH EDGE */
    fillTile(&t[T_PATH_EDGE * 16], 1);

    for (int x = 0; x < 8; x++)
    {
        tilePixel(
            &t[T_PATH_EDGE * 16],
            x,
            5,
            3
        );

        tilePixel(
            &t[T_PATH_EDGE * 16],
            x,
            6,
            3
        );

        tilePixel(
            &t[T_PATH_EDGE * 16],
            x,
            7,
            3
        );
    }

    /* WALL */
    fillTile(&t[T_WALL * 16], 5);

    for (int x = 0; x < 8; x++)
    {
        tilePixel(&t[T_WALL * 16], x, 3, 6);
        tilePixel(&t[T_WALL * 16], x, 7, 6);
    }

    /* WALL SHADOW */
    fillTile(&t[T_WALL_SHADOW * 16], 6);

    for (int x = 0; x < 8; x++)
    {
        tilePixel(
            &t[T_WALL_SHADOW * 16],
            x,
            6,
            7
        );

        tilePixel(
            &t[T_WALL_SHADOW * 16],
            x,
            7,
            7
        );
    }

    /* ROOF */
    fillTile(&t[T_ROOF * 16], 8);

    for (int y = 1; y < 8; y += 3)
    {
        for (int x = 0; x < 8; x++)
            tilePixel(
                &t[T_ROOF * 16],
                x,
                y,
                9
            );
    }

    /* roof tile vertical breaks */
    for (int y = 0; y < 8; y++)
    {
        if (y < 3)
            tilePixel(
                &t[T_ROOF * 16],
                4,
                y,
                10
            );
        else if (y >= 4 && y < 7)
            tilePixel(
                &t[T_ROOF * 16],
                2,
                y,
                10
            );
    }

    /* ROOF LIGHT */
    fillTile(&t[T_ROOF_LIGHT * 16], 9);

    for (int x = 0; x < 8; x++)
    {
        tilePixel(
            &t[T_ROOF_LIGHT * 16],
            x,
            6,
            8
        );

        tilePixel(
            &t[T_ROOF_LIGHT * 16],
            x,
            7,
            8
        );
    }

    /* ROOF EDGE */
    fillTile(&t[T_ROOF_EDGE * 16], 8);

    for (int x = 0; x < 8; x++)
    {
        tilePixel(
            &t[T_ROOF_EDGE * 16],
            x,
            4,
            10
        );

        tilePixel(
            &t[T_ROOF_EDGE * 16],
            x,
            5,
            10
        );

        tilePixel(
            &t[T_ROOF_EDGE * 16],
            x,
            6,
            7
        );

        tilePixel(
            &t[T_ROOF_EDGE * 16],
            x,
            7,
            7
        );
    }

    /* DOOR */
    fillTile(&t[T_DOOR * 16], 11);

    for (int y = 0; y < 8; y++)
    {
        tilePixel(
            &t[T_DOOR * 16],
            0,
            y,
            7
        );

        tilePixel(
            &t[T_DOOR * 16],
            7,
            y,
            7
        );
    }

    tilePixel(&t[T_DOOR * 16], 5, 4, 15);

    /* WINDOW */
    fillTile(&t[T_WINDOW * 16], 5);

    for (int y = 1; y <= 6; y++)
    {
        for (int x = 1; x <= 6; x++)
        {
            tilePixel(
                &t[T_WINDOW * 16],
                x,
                y,
                12
            );
        }
    }

    for (int x = 1; x <= 6; x++)
        tilePixel(
            &t[T_WINDOW * 16],
            x,
            4,
            13
        );

    for (int y = 1; y <= 6; y++)
        tilePixel(
            &t[T_WINDOW * 16],
            4,
            y,
            13
        );

    /* TREE LIGHT */
    fillTile(&t[T_TREE_LIGHT * 16], 1);

    for (int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            int dx = x - 3;
            int dy = y - 4;

            if (dx * dx + dy * dy < 19)
            {
                tilePixel(
                    &t[T_TREE_LIGHT * 16],
                    x,
                    y,
                    ((x + y) & 1)
                        ? 14
                        : 15
                );
            }
        }
    }

    /* TREE DARK */
    fillTile(&t[T_TREE_DARK * 16], 1);

    for (int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            int dx = x - 4;
            int dy = y - 3;

            if (dx * dx + dy * dy < 20)
            {
                tilePixel(
                    &t[T_TREE_DARK * 16],
                    x,
                    y,
                    ((x + y) & 1)
                        ? 14
                        : 2
                );
            }
        }
    }

    /* TRUNK */
    fillTile(&t[T_TRUNK * 16], 1);

    for (int y = 0; y < 8; y++)
    {
        tilePixel(
            &t[T_TRUNK * 16],
            3,
            y,
            11
        );

        tilePixel(
            &t[T_TRUNK * 16],
            4,
            y,
            11
        );
    }

    tilePixel(
        &t[T_TRUNK * 16],
        2,
        7,
        11
    );

    tilePixel(
        &t[T_TRUNK * 16],
        5,
        7,
        11
    );

    /* BUSH */
    fillTile(&t[T_BUSH * 16], 1);

    for (int y = 2; y <= 6; y++)
    {
        for (int x = 1; x <= 6; x++)
        {
            if (!(
                y == 2 &&
                (x == 1 || x == 6)
            ))
            {
                tilePixel(
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

    /* FLOWER */
    fillTile(&t[T_FLOWER * 16], 1);

    tilePixel(&t[T_FLOWER * 16], 2, 2, 10);
    tilePixel(&t[T_FLOWER * 16], 3, 2, 13);
    tilePixel(&t[T_FLOWER * 16], 2, 3, 13);

    tilePixel(&t[T_FLOWER * 16], 6, 5, 10);
    tilePixel(&t[T_FLOWER * 16], 5, 5, 13);

    /* HORIZONTAL FENCE */
    fillTile(&t[T_FENCE_H * 16], 1);

    for (int x = 0; x < 8; x++)
    {
        tilePixel(
            &t[T_FENCE_H * 16],
            x,
            3,
            11
        );

        tilePixel(
            &t[T_FENCE_H * 16],
            x,
            5,
            11
        );
    }

    tilePixel(&t[T_FENCE_H * 16], 1, 1, 11);
    tilePixel(&t[T_FENCE_H * 16], 1, 7, 11);

    tilePixel(&t[T_FENCE_H * 16], 6, 1, 11);
    tilePixel(&t[T_FENCE_H * 16], 6, 7, 11);

    /* VERTICAL FENCE */
    fillTile(&t[T_FENCE_V * 16], 1);

    for (int y = 0; y < 8; y++)
    {
        tilePixel(
            &t[T_FENCE_V * 16],
            3,
            y,
            11
        );

        tilePixel(
            &t[T_FENCE_V * 16],
            5,
            y,
            11
        );
    }

    /* WATER */
    fillTile(&t[T_WATER * 16], 12);

    tilePixel(&t[T_WATER * 16], 1, 2, 13);
    tilePixel(&t[T_WATER * 16], 2, 2, 13);

    tilePixel(&t[T_WATER * 16], 5, 6, 13);
    tilePixel(&t[T_WATER * 16], 6, 6, 13);

    /* WATER LIGHT */
    fillTile(&t[T_WATER_LIGHT * 16], 12);

    for (int x = 1; x <= 5; x++)
        tilePixel(
            &t[T_WATER_LIGHT * 16],
            x,
            3,
            13
        );

    for (int x = 3; x <= 7; x++)
        tilePixel(
            &t[T_WATER_LIGHT * 16],
            x,
            7,
            13
        );

    /* WATER EDGE */
    fillTile(&t[T_WATER_EDGE * 16], 1);

    for (int x = 0; x < 8; x++)
    {
        tilePixel(
            &t[T_WATER_EDGE * 16],
            x,
            5,
            4
        );

        tilePixel(
            &t[T_WATER_EDGE * 16],
            x,
            6,
            12
        );

        tilePixel(
            &t[T_WATER_EDGE * 16],
            x,
            7,
            12
        );
    }

    /* STONE */
    fillTile(&t[T_STONE * 16], 1);

    for (int y = 4; y <= 6; y++)
    {
        for (int x = 2; x <= 5; x++)
            tilePixel(
                &t[T_STONE * 16],
                x,
                y,
                6
            );
    }

    /* SIGN */
    fillTile(&t[T_SIGN * 16], 1);

    for (int y = 1; y <= 4; y++)
    {
        for (int x = 1; x <= 6; x++)
            tilePixel(
                &t[T_SIGN * 16],
                x,
                y,
                11
            );
    }

    tilePixel(&t[T_SIGN * 16], 3, 5, 11);
    tilePixel(&t[T_SIGN * 16], 4, 5, 11);
    tilePixel(&t[T_SIGN * 16], 3, 6, 11);
    tilePixel(&t[T_SIGN * 16], 4, 6, 11);
    tilePixel(&t[T_SIGN * 16], 3, 7, 11);
    tilePixel(&t[T_SIGN * 16], 4, 7, 11);

    /* GARDEN SOIL */
    fillTile(&t[T_GARDEN * 16], 4);

    for (int y = 1; y < 8; y += 3)
    {
        for (int x = 0; x < 8; x++)
            tilePixel(
                &t[T_GARDEN * 16],
                x,
                y,
                3
            );
    }
}

/* =========================================================
   MAP ACCESS
   ========================================================= */

static void setMap(int x, int y, int tile)
{
    if (
        x < 0 ||
        y < 0 ||
        x >= MAP_W ||
        y >= MAP_H
    )
        return;

    if (x < 32)
    {
        u16 *map =
            (u16 *)SCREEN_BASE_BLOCK(30);

        map[y * 32 + x] = tile;
    }
    else
    {
        u16 *map =
            (u16 *)SCREEN_BASE_BLOCK(31);

        map[
            y * 32 +
            (x - 32)
        ] = tile;
    }
}

static int getMap(int x, int y)
{
    if (
        x < 0 ||
        y < 0 ||
        x >= MAP_W ||
        y >= MAP_H
    )
        return T_WALL;

    if (x < 32)
    {
        u16 *map =
            (u16 *)SCREEN_BASE_BLOCK(30);

        return
            map[y * 32 + x]
            & 0x3FF;
    }

    u16 *map =
        (u16 *)SCREEN_BASE_BLOCK(31);

    return
        map[
            y * 32 +
            (x - 32)
        ]
        & 0x3FF;
}

/* =========================================================
   HOUSE
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
       Roof highlight
    */

    for (
        int xx = x + 1;
        xx < right;
        xx++
    )
    {
        setMap(
            xx,
            y,
            T_ROOF_LIGHT
        );
    }

    /*
       Main roof
    */

    for (
        int yy = y + 1;
        yy <= y + 3;
        yy++
    )
    {
        for (
            int xx = x;
            xx <= right;
            xx++
        )
        {
            setMap(
                xx,
                yy,
                T_ROOF
            );
        }
    }

    /*
       Roof overhang
    */

    for (
        int xx = x - 1;
        xx <= right + 1;
        xx++
    )
    {
        setMap(
            xx,
            y + 4,
            T_ROOF_EDGE
        );
    }

    /*
       Facade
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
            setMap(
                xx,
                yy,
                T_WALL
            );
        }
    }

    /*
       Bottom shadow
    */

    for (
        int xx = x;
        xx <= right;
        xx++
    )
    {
        setMap(
            xx,
            y + 8,
            T_WALL_SHADOW
        );
    }

    /*
       Windows
    */

    setMap(
        x + 1,
        y + 6,
        T_WINDOW
    );

    setMap(
        x + 2,
        y + 6,
        T_WINDOW
    );

    setMap(
        right - 2,
        y + 6,
        T_WINDOW
    );

    setMap(
        right - 1,
        y + 6,
        T_WINDOW
    );

    /*
       Door position
    */

    int doorX =
        x + width / 2 - 1;

    setMap(
        doorX,
        y + 7,
        T_DOOR
    );

    setMap(
        doorX + 1,
        y + 7,
        T_DOOR
    );

    setMap(
        doorX,
        y + 8,
        T_DOOR
    );

    setMap(
        doorX + 1,
        y + 8,
        T_DOOR
    );

    /*
       IMPORTANT:
       Always clear a path in front of door.
    */

    for (
        int yy = y + 9;
        yy <= y + 12;
        yy++
    )
    {
        setMap(
            doorX,
            yy,
            T_PATH
        );

        setMap(
            doorX + 1,
            yy,
            T_PATH
        );
    }
}

/* =========================================================
   BIG TREE
   ========================================================= */

static void makeTree(
    int x,
    int y
)
{
    /*
       24 px wide crown.
    */

    setMap(
        x + 1,
        y,
        T_TREE_LIGHT
    );

    setMap(
        x,
        y + 1,
        T_TREE_LIGHT
    );

    setMap(
        x + 1,
        y + 1,
        T_TREE_DARK
    );

    setMap(
        x + 2,
        y + 1,
        T_TREE_LIGHT
    );

    setMap(
        x,
        y + 2,
        T_TREE_DARK
    );

    setMap(
        x + 1,
        y + 2,
        T_TREE_LIGHT
    );

    setMap(
        x + 2,
        y + 2,
        T_TREE_DARK
    );

    setMap(
        x + 1,
        y + 3,
        T_TRUNK
    );
}

/* =========================================================
   FENCED GARDEN
   ========================================================= */

static void makeGarden(
    int x,
    int y,
    int w,
    int h,
    int gateX
)
{
    /*
       Soil inside.
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
            setMap(
                xx,
                yy,
                T_GARDEN
            );
        }
    }

    /*
       Top fence.
    */

    for (
        int xx = x;
        xx < x + w;
        xx++
    )
    {
        setMap(
            xx,
            y,
            T_FENCE_H
        );
    }

    /*
       Bottom fence with gate.
    */

    for (
        int xx = x;
        xx < x + w;
        xx++
    )
    {
        if (
            xx != gateX &&
            xx != gateX + 1
        )
        {
            setMap(
                xx,
                y + h - 1,
                T_FENCE_H
            );
        }
    }

    /*
       Sides.
    */

    for (
        int yy = y + 1;
        yy < y + h - 1;
        yy++
    )
    {
        setMap(
            x,
            yy,
            T_FENCE_V
        );

        setMap(
            x + w - 1,
            yy,
            T_FENCE_V
        );
    }

    /*
       Flowers inside garden.
    */

    if (w > 5 && h > 4)
    {
        setMap(
            x + 2,
            y + 2,
            T_FLOWER
        );

        setMap(
            x + 3,
            y + 2,
            T_FLOWER
        );

        setMap(
            x + w - 3,
            y + h - 3,
            T_FLOWER
        );
    }

    /*
       Gate area stays walkable.
    */

    setMap(
        gateX,
        y + h - 1,
        T_PATH
    );

    setMap(
        gateX + 1,
        y + h - 1,
        T_PATH
    );
}

/* =========================================================
   POND
   ========================================================= */

static void makePond(
    int x,
    int y,
    int w,
    int h
)
{
    /*
       Top bank.
    */

    for (
        int xx = x + 1;
        xx < x + w - 1;
        xx++
    )
    {
        setMap(
            xx,
            y,
            T_WATER_EDGE
        );
    }

    /*
       Water.
    */

    for (
        int yy = y + 1;
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
                ((xx + yy) & 3) == 0
            )
            {
                setMap(
                    xx,
                    yy,
                    T_WATER_LIGHT
                );
            }
            else
            {
                setMap(
                    xx,
                    yy,
                    T_WATER
                );
            }
        }
    }

    /*
       Make corners less rectangular.
    */

    setMap(
        x,
        y,
        T_GRASS
    );

    setMap(
        x + w - 1,
        y,
        T_GRASS
    );

    setMap(
        x,
        y + h - 1,
        T_GRASS
    );

    setMap(
        x + w - 1,
        y + h - 1,
        T_GRASS
    );
}

/* =========================================================
   WORLD
   ========================================================= */

static void makeWorld(void)
{
    /*
       Base grass.
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
            if (
                (x * 7 + y * 11)
                % 31 == 0
            )
            {
                setMap(
                    x,
                    y,
                    T_GRASS_DETAIL
                );
            }
            else
            {
                setMap(
                    x,
                    y,
                    T_GRASS
                );
            }
        }
    }

    /*
       Main road.
       24 px wide.
    */

    for (
        int y = 15;
        y <= 17;
        y++
    )
    {
        for (
            int x = 0;
            x < MAP_W;
            x++
        )
        {
            setMap(
                x,
                y,
                T_PATH
            );
        }
    }

    /*
       Horizontal borders.
    */

    for (
        int x = 0;
        x < MAP_W;
        x++
    )
    {
        setMap(
            x,
            14,
            T_PATH_EDGE
        );
    }

    /*
       Vertical main path.
    */

    for (
        int x = 30;
        x <= 32;
        x++
    )
    {
        for (
            int y = 0;
            y < MAP_H;
            y++
        )
        {
            setMap(
                x,
                y,
                T_PATH
            );
        }
    }

    /*
       NORTH DISTRICT
    */

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

    /*
       Gardens between houses.
    */

    makeGarden(
        13,
        4,
        4,
        8,
        14
    );

    makeGarden(
        46,
        4,
        4,
        8,
        47
    );

    /*
       IMPORTANT:
       Restore all northern door approaches
       AFTER gardens/decorations.
    */

    for (
        int y = 12;
        y <= 14;
        y++
    )
    {
        setMap(
            6,
            y,
            T_PATH
        );

        setMap(
            7,
            y,
            T_PATH
        );

        setMap(
            21,
            y,
            T_PATH
        );

        setMap(
            22,
            y,
            T_PATH
        );

        setMap(
            39,
            y,
            T_PATH
        );

        setMap(
            40,
            y,
            T_PATH
        );

        setMap(
            54,
            y,
            T_PATH
        );

        setMap(
            55,
            y,
            T_PATH
        );
    }

    /*
       Trees around north properties,
       never in door paths.
    */

    makeTree(
        0,
        2
    );

    makeTree(
        28,
        2
    );

    makeTree(
        61,
        2
    );

    /*
       Bushes along gardens.
    */

    setMap(
        12,
        10,
        T_BUSH
    );

    setMap(
        16,
        10,
        T_BUSH
    );

    setMap(
        45,
        10,
        T_BUSH
    );

    setMap(
        49,
        10,
        T_BUSH
    );

    /*
       SOUTH DISTRICT
    */

    makeHouse(
        4,
        21,
        10
    );

    makeHouse(
        19,
        21,
        9
    );

    makeHouse(
        36,
        21,
        10
    );

    makeHouse(
        51,
        21,
        10
    );

    /*
       Pond / park.
       Positioned between road and homes
       without blocking entrances.
    */

    makePond(
        29,
        20,
        6,
        8
    );

    /*
       Trees around park.
    */

    makeTree(
        0,
        20
    );

    makeTree(
        15,
        20
    );

    makeTree(
        47,
        20
    );

    makeTree(
        61,
        20
    );

    /*
       Southern fenced gardens.
    */

    makeGarden(
        4,
        29,
        10,
        3,
        8
    );

    makeGarden(
        37,
        29,
        9,
        3,
        40
    );

    /*
       Park flowers.
    */

    setMap(
        27,
        20,
        T_FLOWER
    );

    setMap(
        27,
        21,
        T_FLOWER
    );

    setMap(
        35,
        20,
        T_FLOWER
    );

    setMap(
        35,
        21,
        T_FLOWER
    );

    /*
       Stones.
    */

    setMap(
        26,
        19,
        T_STONE
    );

    setMap(
        36,
        19,
        T_STONE
    );

    /*
       Town sign.
    */

    setMap(
        34,
        13,
        T_SIGN
    );

    /*
       FINAL DOOR SAFETY PASS.

       Anything previously drawn in front
       of north-facing houses gets replaced
       by walkable path.
    */

    for (
        int y = 12;
        y <= 14;
        y++
    )
    {
        setMap(6, y, T_PATH);
        setMap(7, y, T_PATH);

        setMap(21, y, T_PATH);
        setMap(22, y, T_PATH);

        setMap(39, y, T_PATH);
        setMap(40, y, T_PATH);

        setMap(54, y, T_PATH);
        setMap(55, y, T_PATH);
    }

    /*
       Keep center intersection completely clear.
    */

    for (
        int y = 13;
        y <= 18;
        y++
    )
    {
        for (
            int x = 29;
            x <= 33;
            x++
        )
        {
            setMap(
                x,
                y,
                T_PATH
            );
        }
    }
}

/* =========================================================
   COLLISION
   ========================================================= */

static int solidTile(
    int tx,
    int ty
)
{
    int t =
        getMap(
            tx,
            ty
        );

    if (
        t == T_WALL ||
        t == T_WALL_SHADOW ||
        t == T_ROOF ||
        t == T_ROOF_LIGHT ||
        t == T_ROOF_EDGE ||
        t == T_TREE_LIGHT ||
        t == T_TREE_DARK ||
        t == T_TRUNK ||
        t == T_BUSH ||
        t == T_FENCE_H ||
        t == T_FENCE_V ||
        t == T_WATER ||
        t == T_WATER_LIGHT ||
        t == T_WATER_EDGE ||
        t == T_SIGN
    )
    {
        return 1;
    }

    return 0;
}

static int blocked(
    int x,
    int y
)
{
    int left =
        x + 4;

    int right =
        x + 11;

    int top =
        y + 24;

    int bottom =
        y + 28;

    if (
        solidTile(
            left / 8,
            top / 8
        )
    )
        return 1;

    if (
        solidTile(
            right / 8,
            top / 8
        )
    )
        return 1;

    if (
        solidTile(
            left / 8,
            bottom / 8
        )
    )
        return 1;

    if (
        solidTile(
            right / 8,
            bottom / 8
        )
    )
        return 1;

    return 0;
}

/* =========================================================
   PLAYER
   ========================================================= */

static void objPixel(
    u16 *base,
    int x,
    int y,
    u8 c
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

    int tileNumber =
        tileY * 2 +
        tileX;

    tilePixel(
        &base[
            tileNumber * 16
        ],
        localX,
        localY,
        c
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
    {
        base[i] = 0;
    }
}

static void drawPlayer(
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
    int highlight = 7;
    int shadow = 8;

    /*
       Shadow
    */

    for (
        int x = 4;
        x <= 11;
        x++
    )
    {
        objPixel(
            base,
            x,
            29,
            shadow
        );
    }

    /*
       Head
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
        {
            objPixel(
                base,
                x,
                y,
                skin
            );
        }
    }

    /*
       Hair
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
       Directional face
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
        {
            objPixel(
                base,
                x,
                10,
                hair
            );
        }
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
       Body
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
        {
            objPixel(
                base,
                x,
                y,
                shirt
            );
        }
    }

    objPixel(
        base,
        6,
        15,
        highlight
    );

    objPixel(
        base,
        6,
        16,
        highlight
    );

    /*
       Arms
    */

    int armA =
        frame ? 1 : 0;

    int armB =
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
            y + armA,
            skin
        );

        objPixel(
            base,
            4,
            y + armA,
            skin
        );

        objPixel(
            base,
            11,
            y + armB,
            skin
        );

        objPixel(
            base,
            12,
            y + armB,
            skin
        );
    }

    /*
       Pants
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
       Walking legs
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

static void setupPalettes(void)
{
    BG_PALETTE[0] =
        RGB5(0,0,0);

    /* grass */
    BG_PALETTE[1] =
        RGB5(9,23,9);

    BG_PALETTE[2] =
        RGB5(5,17,6);

    /* paths / soil */
    BG_PALETTE[3] =
        RGB5(24,20,12);

    BG_PALETTE[4] =
        RGB5(29,25,16);

    /* walls */
    BG_PALETTE[5] =
        RGB5(28,24,18);

    BG_PALETTE[6] =
        RGB5(22,18,13);

    BG_PALETTE[7] =
        RGB5(15,12,9);

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

    /* water */
    BG_PALETTE[12] =
        RGB5(7,18,28);

    BG_PALETTE[13] =
        RGB5(17,27,31);

    /* vegetation */
    BG_PALETTE[14] =
        RGB5(4,17,5);

    BG_PALETTE[15] =
        RGB5(10,28,10);

    /*
       Player palette
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
   HELPERS
   ========================================================= */

static int clampInt(
    int value,
    int min,
    int max
)
{
    if (
        value < min
    )
        return min;

    if (
        value > max
    )
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
        SCREEN_BASE(30) |
        BG_16_COLOR |
        BG_SIZE_1;

    makeTiles();

    makeWorld();

    u16 *playerTiles =
        (u16 *)SPRITE_GFX;

    /*
       Start at center intersection.
    */

    int playerX = 244;
    int playerY = 92;

    int direction =
        DIR_DOWN;

    int moving = 0;

    int walkFrame = 0;

    int walkTimer = 0;

    drawPlayer(
        playerTiles,
        direction,
        0
    );

    /*
       Disable unused sprites.
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

        moving = 0;

        /*
           Movement.
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
           Collision separately on X/Y.
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
           Walk animation.
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

        drawPlayer(
            playerTiles,
            direction,
            moving
                ? walkFrame
                : 0
        );

        /*
           Camera.
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

        REG_BG0HOFS =
            cameraX;

        REG_BG0VOFS =
            cameraY;

        /*
           Convert world position
           to screen position.
        */

        int screenX =
            playerX -
            cameraX;

        int screenY =
            playerY -
            cameraY;

        /*
           Player sprite.
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
