#include <gba.h>

#include "world.h"
#include "tileset.h"

MapTile world[MAP_H][MAP_W];


/* =========================================================
   BASIC TILE SETTER
   ========================================================= */

static void setTile(
    int x,
    int y,
    u16 groundTile,
    u16 upperTile,
    int isSolid
)
{
    if (
        x < 0 ||
        y < 0 ||
        x >= MAP_W ||
        y >= MAP_H
    )
        return;

    world[y][x].ground = groundTile;
    world[y][x].upper = upperTile;

    world[y][x].collision =
        isSolid ? 1 : 0;

    world[y][x].elevation = 0;
    world[y][x].behavior = BEHAVIOR_NORMAL;
}


/* =========================================================
   TREE

   Ground layer:
   trunk

   Upper layer:
   canopy

   Only trunk/base blocks movement.
   ========================================================= */

static void makeTree(
    int x,
    int y
)
{
    if (
        x < 0 ||
        y < 0 ||
        x + 3 >= MAP_W ||
        y + 3 >= MAP_H
    )
        return;

    /*
       Upper crown
    */

    world[y][x + 1].upper =
        TILE_TREE_LIGHT;

    world[y][x + 2].upper =
        TILE_TREE_LIGHT;


    /*
       Middle crown
    */

    world[y + 1][x].upper =
        TILE_TREE_MID;

    world[y + 1][x + 1].upper =
        TILE_TREE_LIGHT;

    world[y + 1][x + 2].upper =
        TILE_TREE_LIGHT;

    world[y + 1][x + 3].upper =
        TILE_TREE_MID;


    /*
       Lower crown
    */

    world[y + 2][x].upper =
        TILE_TREE_DARK;

    world[y + 2][x + 1].upper =
        TILE_TREE_MID;

    world[y + 2][x + 2].upper =
        TILE_TREE_MID;

    world[y + 2][x + 3].upper =
        TILE_TREE_DARK;


    /*
       Trunks
    */

    world[y + 3][x + 1].ground =
        TILE_TREE_TRUNK;

    world[y + 3][x + 2].ground =
        TILE_TREE_TRUNK;


    world[y + 3][x + 1].collision = 1;
    world[y + 3][x + 2].collision = 1;
}


/* =========================================================
   BUSH
   ========================================================= */

static void makeBush(
    int x,
    int y
)
{
    if (
        x < 0 ||
        y < 0 ||
        x >= MAP_W ||
        y >= MAP_H
    )
        return;

    world[y][x].upper =
        TILE_BUSH;

    world[y][x].collision = 1;
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

    int door =
        x + width / 2 - 1;


    /*
       Roof
    */

    for (
        int xx = x - 1;
        xx <= right + 1;
        xx++
    )
    {
        if (
            xx < 0 ||
            xx >= MAP_W
        )
            continue;

        if (
            y >= 0 &&
            y < MAP_H
        )
        {
            world[y][xx].upper =
                TILE_ROOF_LIGHT;
        }

        if (
            y + 1 >= 0 &&
            y + 1 < MAP_H
        )
        {
            world[y + 1][xx].upper =
                TILE_ROOF;
        }

        if (
            y + 2 >= 0 &&
            y + 2 < MAP_H
        )
        {
            world[y + 2][xx].upper =
                TILE_ROOF;
        }

        if (
            y + 3 >= 0 &&
            y + 3 < MAP_H
        )
        {
            world[y + 3][xx].upper =
                TILE_ROOF_DARK;
        }

        if (
            y + 4 >= 0 &&
            y + 4 < MAP_H
        )
        {
            world[y + 4][xx].upper =
                TILE_ROOF_EDGE;
        }
    }


    /*
       Roof collision
    */

    for (
        int yy = y;
        yy <= y + 4;
        yy++
    )
    {
        if (
            yy < 0 ||
            yy >= MAP_H
        )
            continue;

        for (
            int xx = x - 1;
            xx <= right + 1;
            xx++
        )
        {
            if (
                xx < 0 ||
                xx >= MAP_W
            )
                continue;

            world[yy][xx].collision = 1;
        }
    }


    /*
       Walls
    */

    for (
        int yy = y + 5;
        yy <= y + 8;
        yy++
    )
    {
        if (
            yy < 0 ||
            yy >= MAP_H
        )
            continue;

        for (
            int xx = x;
            xx <= right;
            xx++
        )
        {
            if (
                xx < 0 ||
                xx >= MAP_W
            )
                continue;

            if (
                yy == y + 8
            )
            {
                world[yy][xx].ground =
                    TILE_WALL_SHADOW;
            }
            else
            {
                world[yy][xx].ground =
                    TILE_WALL;
            }

            world[yy][xx].collision = 1;
        }
    }


    /*
       Windows
    */

    if (
        y + 6 < MAP_H
    )
    {
        if (
            x + 1 >= 0 &&
            x + 1 < MAP_W
        )
        {
            world[y + 6][x + 1].ground =
                TILE_WINDOW;
        }

        if (
            x + 2 >= 0 &&
            x + 2 < MAP_W
        )
        {
            world[y + 6][x + 2].ground =
                TILE_WINDOW;
        }

        if (
            right - 2 >= 0 &&
            right - 2 < MAP_W
        )
        {
            world[y + 6][right - 2].ground =
                TILE_WINDOW;
        }

        if (
            right - 1 >= 0 &&
            right - 1 < MAP_W
        )
        {
            world[y + 6][right - 1].ground =
                TILE_WINDOW;
        }
    }


    /*
       Double door
    */

    if (
        door >= 0 &&
        door + 1 < MAP_W
    )
    {
        if (
            y + 7 >= 0 &&
            y + 7 < MAP_H
        )
        {
            world[y + 7][door].ground =
                TILE_DOOR;

            world[y + 7][door + 1].ground =
                TILE_DOOR;
        }

        if (
            y + 8 >= 0 &&
            y + 8 < MAP_H
        )
        {
            world[y + 8][door].ground =
                TILE_DOOR;

            world[y + 8][door + 1].ground =
                TILE_DOOR;

            world[y + 8][door].behavior =
                BEHAVIOR_DOOR;

            world[y + 8][door + 1].behavior =
                BEHAVIOR_DOOR;
        }
    }


    /*
       Entrance path
    */

    for (
        int yy = y + 9;
        yy <= y + 13;
        yy++
    )
    {
        if (
            yy < 0 ||
            yy >= MAP_H
        )
            continue;

        if (
            door < 0 ||
            door + 1 >= MAP_W
        )
            continue;

        world[yy][door].ground =
            TILE_PATH;

        world[yy][door + 1].ground =
            TILE_PATH_LIGHT;


        world[yy][door].upper =
            TILE_EMPTY;

        world[yy][door + 1].upper =
            TILE_EMPTY;


        world[yy][door].collision = 0;
        world[yy][door + 1].collision = 0;
    }
}


/* =========================================================
   POND
   ========================================================= */

static void makePond(
    int x,
    int y,
    int width,
    int height
)
{
    for (
        int yy = 0;
        yy < height;
        yy++
    )
    {
        for (
            int xx = 0;
            xx < width;
            xx++
        )
        {
            int wx =
                x + xx;

            int wy =
                y + yy;


            if (
                wx < 0 ||
                wy < 0 ||
                wx >= MAP_W ||
                wy >= MAP_H
            )
                continue;


            /*
               Rounded corners
            */

            int corner =
                (
                    xx == 0 ||
                    xx == width - 1
                )
                &&
                (
                    yy == 0 ||
                    yy == height - 1
                );

            if (corner)
                continue;


            if (
                (xx + yy) % 4 == 0
            )
            {
                world[wy][wx].ground =
                    TILE_WATER_LIGHT;
            }
            else
            {
                world[wy][wx].ground =
                    TILE_WATER;
            }


            world[wy][wx].collision = 1;

            world[wy][wx].behavior =
                BEHAVIOR_WATER;
        }
    }


    /*
       Top bank
    */

    for (
        int xx = 1;
        xx < width - 1;
        xx++
    )
    {
        int wx =
            x + xx;

        if (
            wx < 0 ||
            wx >= MAP_W ||
            y < 0 ||
            y >= MAP_H
        )
            continue;

        world[y][wx].ground =
            TILE_WATER_EDGE;

        world[y][wx].collision = 1;
    }
}


/* =========================================================
   GARDEN
   ========================================================= */

static void makeGarden(
    int x,
    int y,
    int width,
    int height,
    int gate
)
{
    /*
       Soil patches
    */

    for (
        int yy = y + 1;
        yy < y + height - 1;
        yy++
    )
    {
        if (
            yy < 0 ||
            yy >= MAP_H
        )
            continue;

        for (
            int xx = x + 1;
            xx < x + width - 1;
            xx++
        )
        {
            if (
                xx < 0 ||
                xx >= MAP_W
            )
                continue;

            if (
                (xx + yy) % 2 == 0
            )
            {
                world[yy][xx].ground =
                    TILE_SOIL;
            }
        }
    }


    /*
       Top and bottom fences
    */

    for (
        int xx = x;
        xx < x + width;
        xx++
    )
    {
        if (
            xx < 0 ||
            xx >= MAP_W
        )
            continue;


        if (
            y >= 0 &&
            y < MAP_H
        )
        {
            world[y][xx].upper =
                TILE_FENCE_H;

            world[y][xx].collision = 1;
        }


        int bottom =
            y + height - 1;

        if (
            bottom >= 0 &&
            bottom < MAP_H
        )
        {
            if (
                xx != gate &&
                xx != gate + 1
            )
            {
                world[bottom][xx].upper =
                    TILE_FENCE_H;

                world[bottom][xx].collision = 1;
            }
        }
    }


    /*
       Side fences
    */

    for (
        int yy = y;
        yy < y + height;
        yy++
    )
    {
        if (
            yy < 0 ||
            yy >= MAP_H
        )
            continue;


        if (
            x >= 0 &&
            x < MAP_W
        )
        {
            world[yy][x].upper =
                TILE_FENCE_V;

            world[yy][x].collision = 1;
        }


        int right =
            x + width - 1;

        if (
            right >= 0 &&
            right < MAP_W
        )
        {
            world[yy][right].upper =
                TILE_FENCE_V;

            world[yy][right].collision = 1;
        }
    }


    /*
       Corner posts
    */

    int right =
        x + width - 1;

    int bottom =
        y + height - 1;


    if (
        x >= 0 &&
        x < MAP_W &&
        y >= 0 &&
        y < MAP_H
    )
    {
        world[y][x].upper =
            TILE_FENCE_POST;
    }


    if (
        right >= 0 &&
        right < MAP_W &&
        y >= 0 &&
        y < MAP_H
    )
    {
        world[y][right].upper =
            TILE_FENCE_POST;
    }


    if (
        x >= 0 &&
        x < MAP_W &&
        bottom >= 0 &&
        bottom < MAP_H
    )
    {
        world[bottom][x].upper =
            TILE_FENCE_POST;
    }


    if (
        right >= 0 &&
        right < MAP_W &&
        bottom >= 0 &&
        bottom < MAP_H
    )
    {
        world[bottom][right].upper =
            TILE_FENCE_POST;
    }


    /*
       Open gate
    */

    if (
        bottom >= 0 &&
        bottom < MAP_H
    )
    {
        if (
            gate >= 0 &&
            gate < MAP_W
        )
        {
            world[bottom][gate].upper =
                TILE_EMPTY;

            world[bottom][gate].collision = 0;
        }

        if (
            gate + 1 >= 0 &&
            gate + 1 < MAP_W
        )
        {
            world[bottom][gate + 1].upper =
                TILE_EMPTY;

            world[bottom][gate + 1].collision = 0;
        }
    }
}


/* =========================================================
   WORLD INITIALIZATION
   ========================================================= */

void worldInit(void)
{
    /*
       Base grass
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
            int grass =
                TILE_GRASS;

            int variation =
                (x * 17 + y * 11) % 53;


            if (
                variation == 4
            )
            {
                grass =
                    TILE_GRASS_DETAIL_A;
            }
            else if (
                variation == 17
            )
            {
                grass =
                    TILE_GRASS_DETAIL_B;
            }


            setTile(
                x,
                y,
                grass,
                TILE_EMPTY,
                0
            );


            world[y][x].behavior =
                BEHAVIOR_GRASS;
        }
    }


    /* =====================================================
       MAIN HORIZONTAL ROAD
       ===================================================== */

    for (
        int x = 0;
        x < MAP_W;
        x++
    )
    {
        world[14][x].ground =
            TILE_PATH_EDGE_TOP;

        world[15][x].ground =
            TILE_PATH;

        world[16][x].ground =
            (x % 4 == 0)
            ? TILE_PATH_LIGHT
            : TILE_PATH;

        world[17][x].ground =
            TILE_PATH;

        world[18][x].ground =
            TILE_PATH_EDGE_BOTTOM;


        for (
            int y = 14;
            y <= 18;
            y++
        )
        {
            world[y][x].behavior =
                BEHAVIOR_NORMAL;

            world[y][x].collision = 0;
        }
    }


    /* =====================================================
       CENTRAL NORTH/SOUTH ROAD
       ===================================================== */

    for (
        int y = 0;
        y < MAP_H;
        y++
    )
    {
        world[y][30].ground =
            TILE_PATH;

        world[y][31].ground =
            (y % 4 == 0)
            ? TILE_PATH_LIGHT
            : TILE_PATH;

        world[y][32].ground =
            TILE_PATH;


        world[y][30].behavior =
            BEHAVIOR_NORMAL;

        world[y][31].behavior =
            BEHAVIOR_NORMAL;

        world[y][32].behavior =
            BEHAVIOR_NORMAL;


        world[y][30].collision = 0;
        world[y][31].collision = 0;
        world[y][32].collision = 0;
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
       GARDENS
       ===================================================== */

    makeGarden(
        13,
        5,
        4,
        7,
        14
    );

    makeGarden(
        46,
        5,
        4,
        7,
        47
    );


    /*
       Flowers in gardens
    */

    world[7][14].upper =
        TILE_FLOWER;

    world[9][15].upper =
        TILE_FLOWER;


    world[7][47].upper =
        TILE_FLOWER;

    world[9][48].upper =
        TILE_FLOWER;


    /* =====================================================
       NORTH TREES
       ===================================================== */

    makeTree(
        0,
        0
    );

    makeTree(
        12,
        0
    );

    makeTree(
        27,
        1
    );

    makeTree(
        45,
        0
    );

    makeTree(
        60,
        0
    );


    /* =====================================================
       BUSHES
       ===================================================== */

    int bushes[][2] =
    {
        {1,12},
        {11,12},
        {25,12},
        {37,12},
        {44,12},
        {58,12}
    };


    int bushCount =
        sizeof(bushes) /
        sizeof(bushes[0]);


    for (
        int i = 0;
        i < bushCount;
        i++
    )
    {
        int x =
            bushes[i][0];

        int y =
            bushes[i][1];


        makeBush(
            x,
            y
        );
    }


    /* =====================================================
       PARK + POND
       ===================================================== */

    makePond(
        28,
        21,
        8,
        8
    );


    makeTree(
        14,
        20
    );

    makeTree(
        47,
        20
    );


    /* =====================================================
       FLOWERS AROUND POND
       ===================================================== */

    for (
        int y = 21;
        y <= 25;
        y += 2
    )
    {
        world[y][26].upper =
            TILE_FLOWER;

        world[y][37].upper =
            TILE_FLOWER;
    }


    /* =====================================================
       STONES
       ===================================================== */

    world[20][24].upper =
        TILE_STONE;

    world[20][38].upper =
        TILE_STONE;


    /* =====================================================
       SIGN
       ===================================================== */

    world[13][27].upper =
        TILE_SIGN;

    world[13][27].collision = 1;

    world[13][27].behavior =
        BEHAVIOR_SIGN;


    /* =====================================================
       FINAL HOUSE ENTRANCE SAFETY

       This is deliberately last so decorations cannot
       block the north house entrances.
       ===================================================== */

    int doors[] =
    {
        6,
        7,

        21,
        22,

        39,
        40,

        54,
        55
    };


    int doorCount =
        sizeof(doors) /
        sizeof(doors[0]);


    for (
        int i = 0;
        i < doorCount;
        i++
    )
    {
        int x =
            doors[i];


        for (
            int y = 12;
            y <= 14;
            y++
        )
        {
            world[y][x].ground =
                TILE_PATH;

            world[y][x].upper =
                TILE_EMPTY;

            world[y][x].collision = 0;

            world[y][x].behavior =
                BEHAVIOR_NORMAL;
        }
    }


    /* =====================================================
       CENTRAL INTERSECTION SAFETY
       ===================================================== */

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
            world[y][x].ground =
                TILE_PATH;

            world[y][x].upper =
                TILE_EMPTY;

            world[y][x].collision = 0;

            world[y][x].behavior =
                BEHAVIOR_NORMAL;
        }
    }
}


/* =========================================================
   WRITE 64x32 MAP

   Each 64x32 BG uses two screen blocks.
   ========================================================= */

static void writeMap(
    int base,
    int x,
    int y,
    u16 tile
)
{
    if (
        x < 0 ||
        y < 0 ||
        x >= MAP_W ||
        y >= MAP_H
    )
        return;


    if (
        x < 32
    )
    {
        u16 *map =
            (u16 *)SCREEN_BASE_BLOCK(base);

        map[
            y * 32 + x
        ] = tile;
    }
    else
    {
        u16 *map =
            (u16 *)SCREEN_BASE_BLOCK(
                base + 1
            );

        map[
            y * 32 +
            (x - 32)
        ] = tile;
    }
}


/* =========================================================
   DRAW WORLD
   ========================================================= */

void worldDraw(void)
{
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
            writeMap(
                28,
                x,
                y,
                world[y][x].ground
            );

            writeMap(
                30,
                x,
                y,
                world[y][x].upper
            );
        }
    }
}


/* =========================================================
   COLLISION QUERY
   ========================================================= */

int worldIsBlocked(
    int pixelX,
    int pixelY
)
{
    int tileX =
        pixelX / 8;

    int tileY =
        pixelY / 8;


    if (
        tileX < 0 ||
        tileY < 0 ||
        tileX >= MAP_W ||
        tileY >= MAP_H
    )
        return 1;


    return
        world[tileY][tileX].collision;
}
