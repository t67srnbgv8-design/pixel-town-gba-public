#include <gba.h>

#include "world.h"
#include "metatiles.h"

u16 worldMap[META_MAP_H][META_MAP_W];

static void put(
    int x,
    int y,
    int id
)
{
    if (
        x < 0 ||
        y < 0 ||
        x >= META_MAP_W ||
        y >= META_MAP_H
    )
    {
        return;
    }

    worldMap[y][x] = id;
}

/* =========================================================
   HOUSE

   7 metatiles wide = 112 px

   Structure:

        /---------------\
       /      ROOF       \
      /___________________\
      |      EAVES        |
      | WIN | DOOR | WIN  |
      |_____ENTRANCE______|

   Individual reusable pieces rather than one giant
   house graphic.
   ========================================================= */

static void house(
    int x,
    int y
)
{
    int i;

    /*
        Upper roof.

        Width: 7 metatiles.
    */

    put(
        x,
        y,
        META_ROOF_LEFT
    );

    for (
        i = 1;
        i <= 5;
        i++
    )
    {
        put(
            x + i,
            y,
            META_ROOF_MIDDLE
        );
    }

    put(
        x + 6,
        y,
        META_ROOF_RIGHT
    );

    /*
        Lower roof / eaves.
    */

    put(
        x,
        y + 1,
        META_ROOF_EAVE_LEFT
    );

    for (
        i = 1;
        i <= 5;
        i++
    )
    {
        put(
            x + i,
            y + 1,
            META_ROOF_EAVE_MIDDLE
        );
    }

    put(
        x + 6,
        y + 1,
        META_ROOF_EAVE_RIGHT
    );

    /*
        Main facade.

        Windows are full 16x16 details.

        Door is centered.
    */

    put(
        x,
        y + 2,
        META_WALL
    );

    put(
        x + 1,
        y + 2,
        META_WINDOW
    );

    put(
        x + 2,
        y + 2,
        META_WALL_DETAIL
    );

    put(
        x + 3,
        y + 2,
        META_DOOR_TOP
    );

    put(
        x + 4,
        y + 2,
        META_WALL_DETAIL
    );

    put(
        x + 5,
        y + 2,
        META_WINDOW
    );

    put(
        x + 6,
        y + 2,
        META_WALL
    );

    /*
        Lower facade.

        Door continues toward the ground.

        Decorative flower boxes give the front
        more visual detail.
    */

    put(
        x,
        y + 3,
        META_WALL
    );

    put(
        x + 1,
        y + 3,
        META_FLOWER_BOX
    );

    put(
        x + 2,
        y + 3,
        META_WALL
    );

    put(
        x + 3,
        y + 3,
        META_DOOR_BOTTOM
    );

    put(
        x + 4,
        y + 3,
        META_SIGN
    );

    put(
        x + 5,
        y + 3,
        META_FLOWER_BOX
    );

    put(
        x + 6,
        y + 3,
        META_WALL
    );

    /*
        Front path.
    */

    put(
        x + 3,
        y + 4,
        META_PATH_V
    );

    put(
        x + 3,
        y + 5,
        META_PATH_V
    );
}

/* =========================================================
   TREE
   ========================================================= */

static void tree(
    int x,
    int y
)
{
    put(
        x,
        y,
        META_TREE_TL
    );

    put(
        x + 1,
        y,
        META_TREE_TR
    );

    put(
        x,
        y + 1,
        META_TREE_BL
    );

    put(
        x + 1,
        y + 1,
        META_TREE_BR
    );
}

/* =========================================================
   WORLD
   ========================================================= */

void worldInit(void)
{
    int x;
    int y;

    /*
        Grass base.
    */

    for (
        y = 0;
        y < META_MAP_H;
        y++
    )
    {
        for (
            x = 0;
            x < META_MAP_W;
            x++
        )
        {
            int n;

            worldMap[y][x] =
                META_GRASS;

            n =
                x * 37 +
                y * 17 +
                x * y * 3;

            if ((n % 41) == 7)
            {
                worldMap[y][x] =
                    META_GRASS_DETAIL;
            }
        }
    }

    /*
        Main horizontal road.
    */

    for (
        x = 0;
        x < META_MAP_W;
        x++
    )
    {
        put(
            x,
            10,
            META_PATH_TOP
        );

        put(
            x,
            11,
            META_PATH_BOTTOM
        );
    }

    /*
        Vertical central street.
    */

    for (
        y = 6;
        y <= 11;
        y++
    )
    {
        put(
            15,
            y,
            META_PATH_V
        );
    }

    /*
        NEW HOUSES.

        Wider than before and spaced apart.
    */

    house(
        3,
        3
    );

    house(
        21,
        3
    );

    /*
        Trees.
    */

    tree(0,0);

    tree(11,1);

    tree(17,1);

    tree(29,0);

    tree(1,13);

    tree(5,13);

    tree(9,12);

    tree(21,13);

    tree(26,13);

    tree(30,12);

    /*
        Bushes around properties.
    */

    put(
        2,
        8,
        META_BUSH
    );

    put(
        10,
        8,
        META_BUSH
    );

    put(
        20,
        8,
        META_BUSH
    );

    put(
        28,
        8,
        META_BUSH
    );

    /*
        Flowers.
    */

    put(
        1,
        7,
        META_FLOWER
    );

    put(
        11,
        7,
        META_FLOWER
    );

    put(
        19,
        7,
        META_FLOWER
    );

    put(
        29,
        7,
        META_FLOWER
    );

    /*
        Pond.
    */

    for (
        y = 13;
        y <= 15;
        y++
    )
    {
        for (
            x = 13;
            x <= 18;
            x++
        )
        {
            put(
                x,
                y,
                META_WATER
            );
        }
    }
}

/* =========================================================
   TILEMAP WRITING
   ========================================================= */

static void writeTile(
    volatile u16 *left,
    volatile u16 *right,

    int x,
    int y,

    int tile
)
{
    if (x < 32)
    {
        left[
            y * 32 + x
        ] = tile;
    }
    else
    {
        right[
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
    int i;
    int mx;
    int my;

    volatile u16 *bg0L =
        SCREEN_BASE_BLOCK(28);

    volatile u16 *bg0R =
        SCREEN_BASE_BLOCK(29);

    volatile u16 *bg1L =
        SCREEN_BASE_BLOCK(30);

    volatile u16 *bg1R =
        SCREEN_BASE_BLOCK(31);

    /*
        Clear both 512px BG layers.
    */

    for (
        i = 0;
        i < 1024;
        i++
    )
    {
        bg0L[i] = 0;
        bg0R[i] = 0;

        bg1L[i] = 0;
        bg1R[i] = 0;
    }

    /*
        Expand 16x16 metatiles into
        four 8x8 hardware tiles.
    */

    for (
        my = 0;
        my < META_MAP_H;
        my++
    )
    {
        for (
            mx = 0;
            mx < META_MAP_W;
            mx++
        )
        {
            int id;
            int tx;
            int ty;

            Metatile *m;

            id =
                worldMap[my][mx];

            m =
                &metatiles[id];

            tx =
                mx * 2;

            ty =
                my * 2;

            /*
                BG0 / ground.
            */

            writeTile(
                bg0L,
                bg0R,
                tx,
                ty,
                m->bottom[0]
            );

            writeTile(
                bg0L,
                bg0R,
                tx + 1,
                ty,
                m->bottom[1]
            );

            writeTile(
                bg0L,
                bg0R,
                tx,
                ty + 1,
                m->bottom[2]
            );

            writeTile(
                bg0L,
                bg0R,
                tx + 1,
                ty + 1,
                m->bottom[3]
            );

            /*
                BG1 / overhead.
            */

            writeTile(
                bg1L,
                bg1R,
                tx,
                ty,
                m->top[0]
            );

            writeTile(
                bg1L,
                bg1R,
                tx + 1,
                ty,
                m->top[1]
            );

            writeTile(
                bg1L,
                bg1R,
                tx,
                ty + 1,
                m->top[2]
            );

            writeTile(
                bg1L,
                bg1R,
                tx + 1,
                ty + 1,
                m->top[3]
            );
        }
    }
}

/* =========================================================
   COLLISION
   ========================================================= */

int worldIsBlocked(
    int pixelX,
    int pixelY
)
{
    int mx;
    int my;
    int id;

    if (
        pixelX < 0 ||
        pixelY < 0 ||
        pixelX >= WORLD_W ||
        pixelY >= WORLD_H
    )
    {
        return 1;
    }

    mx =
        pixelX >> 4;

    my =
        pixelY >> 4;

    id =
        worldMap[my][mx];

    return
        metatiles[id]
        .collision;
}
