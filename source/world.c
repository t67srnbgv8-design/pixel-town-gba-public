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
        x<0 ||
        y<0 ||
        x>=META_MAP_W ||
        y>=META_MAP_H
    )
        return;

    worldMap[y][x]=id;
}

/*
   Compact house.

   5 metatiles wide = 80px.

   Roof:   16px
   wall:   32px
   door:   32px

   Much closer to the player scale.
*/

static void house(
    int x,
    int y
)
{
    /*
       Roof
    */

    put(x,   y,META_ROOF_L);
    put(x+1, y,META_ROOF_M);
    put(x+2, y,META_ROOF_M);
    put(x+3, y,META_ROOF_M);
    put(x+4, y,META_ROOF_R);

    /*
       Upper facade
    */

    put(x,   y+1,META_WALL);
    put(x+1, y+1,META_WINDOW_TOP);
    put(x+2, y+1,META_DOOR_TOP);
    put(x+3, y+1,META_WINDOW_TOP);
    put(x+4, y+1,META_WALL);

    /*
       Lower facade
    */

    put(x,   y+2,META_WALL);
    put(x+1, y+2,META_WINDOW_BOTTOM);
    put(x+2, y+2,META_DOOR_BOTTOM);
    put(x+3, y+2,META_WINDOW_BOTTOM);
    put(x+4, y+2,META_WALL);

    /*
       Narrow entrance path.
    */

    put(
        x+2,
        y+3,
        META_PATH_V
    );

    put(
        x+2,
        y+4,
        META_PATH_V
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
        META_TREE_TL
    );

    put(
        x+1,
        y,
        META_TREE_TR
    );

    put(
        x,
        y+1,
        META_TREE_BL
    );

    put(
        x+1,
        y+1,
        META_TREE_BR
    );
}

void worldInit(void)
{
    /*
       Grass base.
    */

    for (int y=0; y<META_MAP_H; y++)
    {
        for (int x=0; x<META_MAP_W; x++)
        {
            worldMap[y][x]=META_GRASS;

            /*
               Sparse natural variation.
               Not a checkerboard.
            */

            int n=
                x*37 +
                y*17 +
                x*y*3;

            if ((n%41)==7)
                worldMap[y][x]
                    =META_GRASS_DETAIL;
        }
    }

    /*
       Main horizontal road.

       ONLY 32px tall.
    */

    for (int x=0; x<META_MAP_W; x++)
    {
        put(
            x,
            9,
            META_PATH_TOP
        );

        put(
            x,
            10,
            META_PATH_BOTTOM
        );
    }

    /*
       Narrow vertical street.
    */

    for (int y=5; y<=10; y++)
    {
        put(
            15,
            y,
            META_PATH_V
        );
    }

    /*
       Houses.

       More space between them.
    */

    house(
        5,
        3
    );

    house(
        21,
        3
    );

    /*
       Trees.

       Slightly irregular positioning.
    */

    tree(0,1);
    tree(2,2);

    tree(11,0);

    tree(17,1);

    tree(28,1);

    tree(1,12);
    tree(4,13);

    tree(9,12);

    tree(21,12);
    tree(26,13);

    tree(29,11);

    /*
       Small bushes around houses.
    */

    put(
        4,
        7,
        META_BUSH
    );

    put(
        10,
        7,
        META_BUSH
    );

    put(
        20,
        7,
        META_BUSH
    );

    put(
        26,
        7,
        META_BUSH
    );

    /*
       Flowers.

       ALWAYS BG0.
    */

    put(
        3,
        6,
        META_FLOWER
    );

    put(
        11,
        6,
        META_FLOWER
    );

    put(
        19,
        6,
        META_FLOWER
    );

    put(
        27,
        6,
        META_FLOWER
    );

    /*
       Pond.
    */

    for (int y=12; y<=14; y++)
    {
        for (int x=13; x<=18; x++)
        {
            put(
                x,
                y,
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
    if (x<32)
    {
        left[
            y*32+x
        ]=tile;
    }
    else
    {
        right[
            y*32+(x-32)
        ]=tile;
    }
}

void worldDraw(void)
{
    volatile u16 *bg0L=
        SCREEN_BASE_BLOCK(28);

    volatile u16 *bg0R=
        SCREEN_BASE_BLOCK(29);

    volatile u16 *bg1L=
        SCREEN_BASE_BLOCK(30);

    volatile u16 *bg1R=
        SCREEN_BASE_BLOCK(31);

    for (int i=0; i<1024; i++)
    {
        bg0L[i]=0;
        bg0R[i]=0;

        bg1L[i]=0;
        bg1R[i]=0;
    }

    for (
        int my=0;
        my<META_MAP_H;
        my++
    )
    {
        for (
            int mx=0;
            mx<META_MAP_W;
            mx++
        )
        {
            int id=
                worldMap[my][mx];

            Metatile *m=
                &metatiles[id];

            int tx=mx*2;
            int ty=my*2;

            /*
               GROUND
            */

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
               OVERHEAD

               In v13 only tree canopy
               graphics use this layer.
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
        pixelX<0 ||
        pixelY<0 ||
        pixelX>=WORLD_W ||
        pixelY>=WORLD_H
    )
        return 1;

    int mx=
        pixelX>>4;

    int my=
        pixelY>>4;

    int id=
        worldMap[my][mx];

    return
        metatiles[id]
        .collision;
}
