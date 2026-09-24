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
    if (x < 0 ||
        y < 0 ||
        x >= META_MAP_W ||
        y >= META_MAP_H)
        return;

    worldMap[y][x] = id;
}

static void house(
    int x,
    int y,
    int w
)
{
    /*
       Upper roof
    */

    put(
        x,
        y,
        META_ROOF_LEFT
    );

    for (int i=1; i<w-1; i++)
        put(
            x+i,
            y,
            META_ROOF_MIDDLE
        );

    put(
        x+w-1,
        y,
        META_ROOF_RIGHT
    );

    /*
       Lower roof
    */

    put(
        x,
        y+1,
        META_ROOF_LOW_LEFT
    );

    for (int i=1; i<w-1; i++)
        put(
            x+i,
            y+1,
            META_ROOF_LOW_MIDDLE
        );

    put(
        x+w-1,
        y+1,
        META_ROOF_LOW_RIGHT
    );

    /*
       Facade
    */

    for (int i=0; i<w; i++)
        put(
            x+i,
            y+2,
            META_WALL
        );

    if (w >= 5) {
        put(
            x+1,
            y+2,
            META_WINDOW
        );

        put(
            x+w-2,
            y+2,
            META_WINDOW
        );
    }

    put(
        x+w/2,
        y+2,
        META_DOOR
    );

    /*
       Entrance
    */

    put(
        x+w/2,
        y+3,
        META_PATH
    );

    put(
        x+w/2,
        y+4,
        META_PATH
    );
}

static void tree(
    int x,
    int y
)
{
    put(
        x,
        y,
        META_TREE_TOP_LEFT
    );

    put(
        x+1,
        y,
        META_TREE_TOP_RIGHT
    );

    put(
        x,
        y+1,
        META_TREE_LOW_LEFT
    );

    put(
        x+1,
        y+1,
        META_TREE_LOW_RIGHT
    );
}

void worldInit(void)
{
    /*
       Clean grass base.

       Only sparse variation.
       No checkerboard.
    */

    for (int y=0; y<META_MAP_H; y++) {
        for (int x=0; x<META_MAP_W; x++) {

            worldMap[y][x] =
                META_GRASS;

            if (
                ((x*13 + y*7) % 29)
                == 4
            )
                worldMap[y][x] =
                    META_GRASS_DETAIL;
        }
    }

    /*
       Main road
    */

    for (int x=0; x<META_MAP_W; x++) {
        put(
            x,
            8,
            META_PATH_EDGE
        );

        put(
            x,
            9,
            META_PATH
        );
    }

    /*
       Vertical road
    */

    for (int y=5; y<16; y++) {
        put(
            15,
            y,
            META_PATH
        );

        put(
            16,
            y,
            META_PATH
        );
    }

    /*
       Two compact houses.

       5 metatiles = 80 pixels wide.
    */

    house(
        4,
        3,
        5
    );

    house(
        22,
        3,
        5
    );

    /*
       Tree groups.

       Not placed on a rigid grid.
    */

    tree(0,1);
    tree(2,0);

    tree(10,1);
    tree(12,2);

    tree(18,1);
    tree(28,1);

    tree(1,11);
    tree(4,12);

    tree(25,11);
    tree(29,12);

    /*
       Gardens.

       Flowers are BG0.
    */

    put(9,6,META_FLOWER);
    put(10,6,META_FLOWER);
    put(11,6,META_BUSH);

    put(20,6,META_BUSH);
    put(21,6,META_FLOWER);
    put(22,6,META_FLOWER);

    /*
       Fences
    */

    for (int x=7; x<=11; x++)
        put(
            x,
            11,
            META_FENCE
        );

    for (int x=21; x<=25; x++)
        put(
            x,
            11,
            META_FENCE
        );

    /*
       Small pond
    */

    for (int y=12; y<=14; y++) {
        for (int x=13; x<=18; x++) {

            if (y == 12)
                put(
                    x,y,
                    META_WATER_EDGE
                );
            else
                put(
                    x,y,
                    META_WATER
                );
        }
    }
}

static void writeTile(
    volatile u16 *left,
    volatile u16 *right,
    int x,
    int y,
    int tile
)
{
    if (x < 32)
        left[
            y*32+x
        ] = tile;
    else
        right[
            y*32+(x-32)
        ] = tile;
}

void worldDraw(void)
{
    volatile u16 *bg0L =
        SCREEN_BASE_BLOCK(28);

    volatile u16 *bg0R =
        SCREEN_BASE_BLOCK(29);

    volatile u16 *bg1L =
        SCREEN_BASE_BLOCK(30);

    volatile u16 *bg1R =
        SCREEN_BASE_BLOCK(31);

    for (int i=0; i<1024; i++) {
        bg0L[i] = 0;
        bg0R[i] = 0;

        bg1L[i] = 0;
        bg1R[i] = 0;
    }

    for (int my=0;
         my<META_MAP_H;
         my++)
    {
        for (int mx=0;
             mx<META_MAP_W;
             mx++)
        {
            int id =
                worldMap[my][mx];

            Metatile *m =
                &metatiles[id];

            int tx = mx*2;
            int ty = my*2;

            writeTile(
                bg0L,bg0R,
                tx,ty,
                m->bottom[0]
            );

            writeTile(
                bg0L,bg0R,
                tx+1,ty,
                m->bottom[1]
            );

            writeTile(
                bg0L,bg0R,
                tx,ty+1,
                m->bottom[2]
            );

            writeTile(
                bg0L,bg0R,
                tx+1,ty+1,
                m->bottom[3]
            );

            /*
               ONLY intentional overhead
               graphics reach BG1.
            */

            writeTile(
                bg1L,bg1R,
                tx,ty,
                m->top[0]
            );

            writeTile(
                bg1L,bg1R,
                tx+1,ty,
                m->top[1]
            );

            writeTile(
                bg1L,bg1R,
                tx,ty+1,
                m->top[2]
            );

            writeTile(
                bg1L,bg1R,
                tx+1,ty+1,
                m->top[3]
            );
        }
    }
}

int worldIsBlocked(
    int pixelX,
    int pixelY
)
{
    if (
        pixelX < 0 ||
        pixelY < 0 ||
        pixelX >= WORLD_W ||
        pixelY >= WORLD_H
    )
        return 1;

    int mx =
        pixelX >> 4;

    int my =
        pixelY >> 4;

    return
        metatiles[
            worldMap[my][mx]
        ].collision;
}
