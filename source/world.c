#include <gba.h>

#include "world.h"
#include "metatiles.h"

u16 worldMap[META_MAP_H][META_MAP_W];

static void put(int x, int y, int id)
{
    if (
        x < 0 || y < 0 ||
        x >= META_MAP_W ||
        y >= META_MAP_H
    )
        return;

    worldMap[y][x] = id;
}

/*
    HOUSE V3

    Width:  6 metatiles = 96 pixels
    Height: 5 metatiles = 80 pixels

    Roof:       y + 0, y + 1
    Upper wall: y + 2
    Windows:    y + 3
    Foundation: y + 4

    Entrance: x + 3
*/

static void house(int x, int y)
{
    int i;

    /* Upper roof */

    put(x,     y, META_ROOF_LEFT);

    for (i = 1; i < 5; i++)
        put(x + i, y, META_ROOF_MIDDLE);

    put(x + 5, y, META_ROOF_RIGHT);

    /* Roof overhang */

    put(x, y + 1, META_ROOF_EAVE_LEFT);

    for (i = 1; i < 5; i++)
        put(x + i, y + 1, META_ROOF_EAVE_MIDDLE);

    put(x + 5, y + 1, META_ROOF_EAVE_RIGHT);

    /*
        Upper facade.

        This extra wall row makes the house taller
        instead of stretching it horizontally.
    */

    put(x,     y + 2, META_WALL);
    put(x + 1, y + 2, META_WALL_DETAIL);
    put(x + 2, y + 2, META_WALL);
    put(x + 3, y + 2, META_WALL_DETAIL);
    put(x + 4, y + 2, META_WALL);
    put(x + 5, y + 2, META_WALL);

    /*
        Main facade.

        Two windows and one centered entrance.
    */

    put(x,     y + 3, META_WALL);
    put(x + 1, y + 3, META_WINDOW);
    put(x + 2, y + 3, META_WALL_DETAIL);
    put(x + 3, y + 3, META_DOOR_TOP);
    put(x + 4, y + 3, META_WINDOW);
    put(x + 5, y + 3, META_WALL);

    /*
        Lower facade and foundation.

        The entrance remains walkable.
    */

    put(x,     y + 4, META_WALL);
    put(x + 1, y + 4, META_FLOWER_BOX);
    put(x + 2, y + 4, META_WALL);
    put(x + 3, y + 4, META_DOOR_BOTTOM);
    put(x + 4, y + 4, META_FLOWER_BOX);
    put(x + 5, y + 4, META_WALL);

    /*
        Straight entrance path to the road.
    */

    put(x + 3, y + 5, META_PATH_V);
    put(x + 3, y + 6, META_PATH_V);
    put(x + 3, y + 7, META_PATH_V);
}

static void tree(int x, int y)
{
    put(x,     y,     META_TREE_TL);
    put(x + 1, y,     META_TREE_TR);
    put(x,     y + 1, META_TREE_BL);
    put(x + 1, y + 1, META_TREE_BR);
}

void worldInit(void)
{
    int x;
    int y;

    /* Grass */

    for (y = 0; y < META_MAP_H; y++)
    {
        for (x = 0; x < META_MAP_W; x++)
        {
            int n = x * 37 + y * 17 + x * y * 3;

            worldMap[y][x] = META_GRASS;

            if ((n % 41) == 7)
                worldMap[y][x] = META_GRASS_DETAIL;
        }
    }

    /* Main road */

    for (x = 0; x < META_MAP_W; x++)
    {
        put(x, 10, META_PATH_TOP);
        put(x, 11, META_PATH_BOTTOM);
    }

    /* Central connecting path */

    for (y = 7; y <= 9; y++)
        put(15, y, META_PATH_V);

    /*
        Compact houses.

        Both fit entirely inside the world.
    */

    house(4, 2);
    house(22, 2);

    /* Trees away from the entrances */

    tree(0, 0);
    tree(11, 1);
    tree(18, 1);
    tree(29, 0);

    tree(1, 13);
    tree(5, 13);
    tree(9, 12);
    tree(21, 13);
    tree(26, 13);
    tree(30, 12);

    /* Bushes */

    put(2, 8, META_BUSH);
    put(11, 8, META_BUSH);
    put(20, 8, META_BUSH);
    put(29, 8, META_BUSH);

    /* Flowers */

    put(1, 7, META_FLOWER);
    put(12, 7, META_FLOWER);
    put(19, 7, META_FLOWER);
    put(30, 7, META_FLOWER);

    /* Pond */

    for (y = 13; y <= 15; y++)
    {
        for (x = 13; x <= 18; x++)
            put(x, y, META_WATER);
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
        left[y * 32 + x] = tile;
    else
        right[y * 32 + (x - 32)] = tile;
}

void worldDraw(void)
{
    volatile u16 *bg0L = SCREEN_BASE_BLOCK(28);
    volatile u16 *bg0R = SCREEN_BASE_BLOCK(29);
    volatile u16 *bg1L = SCREEN_BASE_BLOCK(30);
    volatile u16 *bg1R = SCREEN_BASE_BLOCK(31);

    int i;
    int mx;
    int my;

    for (i = 0; i < 1024; i++)
    {
        bg0L[i] = 0;
        bg0R[i] = 0;
        bg1L[i] = 0;
        bg1R[i] = 0;
    }

    for (my = 0; my < META_MAP_H; my++)
    {
        for (mx = 0; mx < META_MAP_W; mx++)
        {
            int id = worldMap[my][mx];
            Metatile *m = &metatiles[id];

            int tx = mx * 2;
            int ty = my * 2;

            writeTile(bg0L, bg0R, tx,     ty,     m->bottom[0]);
            writeTile(bg0L, bg0R, tx + 1, ty,     m->bottom[1]);
            writeTile(bg0L, bg0R, tx,     ty + 1, m->bottom[2]);
            writeTile(bg0L, bg0R, tx + 1, ty + 1, m->bottom[3]);

            writeTile(bg1L, bg1R, tx,     ty,     m->top[0]);
            writeTile(bg1L, bg1R, tx + 1, ty,     m->top[1]);
            writeTile(bg1L, bg1R, tx,     ty + 1, m->top[2]);
            writeTile(bg1L, bg1R, tx + 1, ty + 1, m->top[3]);
        }
    }
}

int worldIsBlocked(int pixelX, int pixelY)
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
        return 1;

    mx = pixelX >> 4;
    my = pixelY >> 4;

    id = worldMap[my][mx];

    return metatiles[id].collision;
}
