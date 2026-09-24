#include <gba.h>

#include "world.h"
#include "metatiles.h"

u16 worldMap[META_MAP_H][META_MAP_W];

static void putMeta(
    int x,
    int y,
    int id
)
{
    if (x < 0 || x >= META_MAP_W)
        return;

    if (y < 0 || y >= META_MAP_H)
        return;

    worldMap[y][x] = id;
}

static void makeHouse(
    int x,
    int y,
    int width
)
{
    if (width < 3)
        return;

    putMeta(x,y,META_ROOF_TL);

    for (int i=1; i<width-1; i++)
        putMeta(x+i,y,META_ROOF_T);

    putMeta(
        x+width-1,
        y,
        META_ROOF_TR
    );

    putMeta(x,y+1,META_ROOF_L);

    for (int i=1; i<width-1; i++)
        putMeta(x+i,y+1,META_ROOF_M);

    putMeta(
        x+width-1,
        y+1,
        META_ROOF_R
    );

    putMeta(x,y+2,META_ROOF_BL);

    for (int i=1; i<width-1; i++)
        putMeta(x+i,y+2,META_ROOF_B);

    putMeta(
        x+width-1,
        y+2,
        META_ROOF_BR
    );

    for (int i=0; i<width; i++)
        putMeta(
            x+i,
            y+3,
            META_WALL
        );

    if (width >= 5) {
        putMeta(
            x+1,
            y+3,
            META_WINDOW
        );

        putMeta(
            x+width-2,
            y+3,
            META_WINDOW
        );
    }

    putMeta(
        x + width/2,
        y+3,
        META_DOOR
    );

    /*
       short entrance path
    */

    putMeta(
        x + width/2,
        y+4,
        META_PATH
    );

    putMeta(
        x + width/2,
        y+5,
        META_PATH
    );
}

static void makeTree(
    int x,
    int y
)
{
    putMeta(
        x,
        y,
        META_TREE_TOP_L
    );

    putMeta(
        x+1,
        y,
        META_TREE_TOP_R
    );

    putMeta(
        x,
        y+1,
        META_TREE_BOTTOM_L
    );

    putMeta(
        x+1,
        y+1,
        META_TREE_BOTTOM_R
    );
}

static void makePond(
    int x,
    int y,
    int w,
    int h
)
{
    for (int yy=0; yy<h; yy++) {
        for (int xx=0; xx<w; xx++) {

            int id = META_WATER;

            if (yy == 0)
                id = META_WATER_TOP;

            if (yy == h-1)
                id = META_WATER_BOTTOM;

            if (xx == 0)
                id = META_WATER_LEFT;

            if (xx == w-1)
                id = META_WATER_RIGHT;

            putMeta(
                x+xx,
                y+yy,
                id
            );
        }
    }
}

void worldInit(void)
{
    /*
       Base grass
    */

    for (int y=0; y<META_MAP_H; y++) {
        for (int x=0; x<META_MAP_W; x++) {

            int n =
                (x * 17 +
                 y * 31) % 19;

            if (n == 3)
                worldMap[y][x] =
                    META_GRASS_A;

            else if (n == 8)
                worldMap[y][x] =
                    META_GRASS_B;

            else
                worldMap[y][x] =
                    META_GRASS;
        }
    }

    /*
       Main horizontal road.

       Much narrower than v10.
    */

    for (int x=0; x<META_MAP_W; x++) {

        putMeta(
            x,
            8,
            META_PATH_TOP
        );

        putMeta(
            x,
            9,
            META_PATH
        );

        putMeta(
            x,
            10,
            META_PATH_BOTTOM
        );
    }

    /*
       Vertical town path.
    */

    for (int y=0; y<META_MAP_H; y++) {
        putMeta(
            15,
            y,
            META_PATH
        );
    }

    /*
       Houses.

       Smaller and much closer to
       classic handheld RPG scale.
    */

    makeHouse(
        2,
        2,
        5
    );

    makeHouse(
        9,
        2,
        5
    );

    makeHouse(
        18,
        2,
        5
    );

    makeHouse(
        25,
        2,
        5
    );

    /*
       Trees around town.
    */

    makeTree(0,0);
    makeTree(6,0);
    makeTree(16,0);
    makeTree(23,0);
    makeTree(30,0);

    makeTree(1,12);
    makeTree(5,13);
    makeTree(10,12);

    makeTree(20,12);
    makeTree(25,13);
    makeTree(29,12);

    /*
       Pond / park
    */

    makePond(
        13,
        12,
        5,
        3
    );

    /*
       Bushes
    */

    putMeta(7,6,META_BUSH);
    putMeta(8,6,META_BUSH);

    putMeta(23,6,META_BUSH);
    putMeta(24,6,META_BUSH);

    putMeta(3,7,META_FLOWER);
    putMeta(4,7,META_FLOWER);

    putMeta(27,7,META_FLOWER);
    putMeta(28,7,META_FLOWER);

    /*
       Small fences.
    */

    for (int x=7; x<=11; x++)
        putMeta(
            x,
            11,
            META_FENCE_H
        );

    for (int x=21; x<=25; x++)
        putMeta(
            x,
            11,
            META_FENCE_H
        );
}

static void writeScreenTile(
    volatile u16 *left,
    volatile u16 *right,
    int tx,
    int ty,
    u16 tile
)
{
    if (tx < 32)
        left[
            ty * 32 + tx
        ] = tile;
    else
        right[
            ty * 32 +
            (tx - 32)
        ] = tile;
}

void worldDraw(void)
{
    volatile u16 *bottomLeft =
        SCREEN_BASE_BLOCK(28);

    volatile u16 *bottomRight =
        SCREEN_BASE_BLOCK(29);

    volatile u16 *topLeft =
        SCREEN_BASE_BLOCK(30);

    volatile u16 *topRight =
        SCREEN_BASE_BLOCK(31);

    for (int i=0; i<1024; i++) {
        bottomLeft[i] = 0;
        bottomRight[i] = 0;

        topLeft[i] = 0;
        topRight[i] = 0;
    }

    /*
       Each 16x16 metatile becomes
       four 8x8 hardware BG tiles.
    */

    for (int my=0; my<META_MAP_H; my++) {
        for (int mx=0; mx<META_MAP_W; mx++) {

            int id =
                worldMap[my][mx];

            Metatile *m =
                &metatiles[id];

            int tx = mx * 2;
            int ty = my * 2;

            writeScreenTile(
                bottomLeft,
                bottomRight,
                tx,
                ty,
                m->bottom[0]
            );

            writeScreenTile(
                bottomLeft,
                bottomRight,
                tx+1,
                ty,
                m->bottom[1]
            );

            writeScreenTile(
                bottomLeft,
                bottomRight,
                tx,
                ty+1,
                m->bottom[2]
            );

            writeScreenTile(
                bottomLeft,
                bottomRight,
                tx+1,
                ty+1,
                m->bottom[3]
            );

            writeScreenTile(
                topLeft,
                topRight,
                tx,
                ty,
                m->top[0]
            );

            writeScreenTile(
                topLeft,
                topRight,
                tx+1,
                ty,
                m->top[1]
            );

            writeScreenTile(
                topLeft,
                topRight,
                tx,
                ty+1,
                m->top[2]
            );

            writeScreenTile(
                topLeft,
                topRight,
                tx+1,
                ty+1,
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
    if (pixelX < 0 ||
        pixelY < 0 ||
        pixelX >= WORLD_W ||
        pixelY >= WORLD_H)
        return 1;

    int mx =
        pixelX >> 4;

    int my =
        pixelY >> 4;

    int id =
        worldMap[my][mx];

    return
        metatiles[id].collision;
}
