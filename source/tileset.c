#include <gba.h>
#include "tileset.h"

static u16 *gfx;

static void clearTile(int tile)
{
    u16 *t = &gfx[tile * 16];

    for (int i = 0; i < 16; i++)
        t[i] = 0;
}

static void px(int tile, int x, int y, int colour)
{
    if (x < 0 || x > 7 || y < 0 || y > 7)
        return;

    u16 *t = &gfx[tile * 16];

    int p = y * 8 + x;
    int word = p >> 2;
    int shift = (p & 3) * 4;

    t[word] &= ~(0xF << shift);
    t[word] |= (colour & 15) << shift;
}

static void rect(
    int tile,
    int x1,
    int y1,
    int x2,
    int y2,
    int colour
)
{
    for (int y = y1; y <= y2; y++)
        for (int x = x1; x <= x2; x++)
            px(tile,x,y,colour);
}

static void fillTile(int tile, int colour)
{
    rect(tile,0,0,7,7,colour);
}

/*
    Palette

    0 transparent
    1 grass
    2 dark grass
    3 path
    4 path highlight
    5 wall
    6 wall shadow
    7 dark outline/brown
    8 roof red
    9 roof highlight
    10 roof shadow
    11 tree green
    12 tree light
    13 tree dark
    14 water
    15 light/flower
*/

void tilesetInit(void)
{
    gfx = (u16 *)CHAR_BASE_ADR(0);

    for (int i=0; i<TILE_COUNT; i++)
        clearTile(i);

    BG_PALETTE[0]  = RGB5(0,0,0);

    BG_PALETTE[1]  = RGB5(12,24,10);
    BG_PALETTE[2]  = RGB5(7,18,7);

    BG_PALETTE[3]  = RGB5(24,20,12);
    BG_PALETTE[4]  = RGB5(29,25,17);

    BG_PALETTE[5]  = RGB5(28,24,18);
    BG_PALETTE[6]  = RGB5(20,16,12);
    BG_PALETTE[7]  = RGB5(9,7,5);

    BG_PALETTE[8]  = RGB5(24,6,5);
    BG_PALETTE[9]  = RGB5(31,12,8);
    BG_PALETTE[10] = RGB5(14,4,4);

    BG_PALETTE[11] = RGB5(8,20,7);
    BG_PALETTE[12] = RGB5(15,27,10);
    BG_PALETTE[13] = RGB5(4,13,5);

    BG_PALETTE[14] = RGB5(8,20,29);
    BG_PALETTE[15] = RGB5(30,30,23);

    /*
       GRASS

       Mostly flat on purpose.
       Detail is sparse so the map
       doesn't look like a checkerboard.
    */

    fillTile(TILE_GRASS,1);

    fillTile(TILE_GRASS_DARK,1);

    px(TILE_GRASS_DARK,1,6,2);
    px(TILE_GRASS_DARK,2,5,2);

    px(TILE_GRASS_DARK,5,3,2);
    px(TILE_GRASS_DARK,6,2,2);

    fillTile(TILE_GRASS_FLOWER,1);

    px(TILE_GRASS_FLOWER,2,5,2);
    px(TILE_GRASS_FLOWER,3,4,15);
    px(TILE_GRASS_FLOWER,4,5,2);

    /*
       PATH
    */

    fillTile(TILE_PATH,3);

    px(TILE_PATH,1,2,4);
    px(TILE_PATH,6,5,4);
    px(TILE_PATH,4,7,4);

    fillTile(TILE_PATH_DARK,3);

    px(TILE_PATH_DARK,0,4,7);
    px(TILE_PATH_DARK,4,1,4);
    px(TILE_PATH_DARK,7,6,7);

    fillTile(TILE_PATH_EDGE,1);

    for (int x=0; x<8; x++) {
        px(TILE_PATH_EDGE,x,5,4);
        px(TILE_PATH_EDGE,x,6,3);
        px(TILE_PATH_EDGE,x,7,3);
    }

    /*
       ROOF

       Strong highlight/shadow bands,
       but no giant repetitive stripes.
    */

    fillTile(TILE_ROOF_RED,8);

    for (int x=0; x<8; x++) {
        px(TILE_ROOF_RED,x,0,9);
        px(TILE_ROOF_RED,x,7,10);
    }

    px(TILE_ROOF_RED,1,3,9);
    px(TILE_ROOF_RED,5,3,9);

    fillTile(TILE_ROOF_RED_LIGHT,8);

    for (int x=0; x<8; x++) {
        px(TILE_ROOF_RED_LIGHT,x,0,9);
        px(TILE_ROOF_RED_LIGHT,x,1,9);
    }

    px(TILE_ROOF_RED_LIGHT,2,4,9);
    px(TILE_ROOF_RED_LIGHT,6,4,9);

    fillTile(TILE_ROOF_RED_DARK,8);

    for (int x=0; x<8; x++) {
        px(TILE_ROOF_RED_DARK,x,6,10);
        px(TILE_ROOF_RED_DARK,x,7,10);
    }

    fillTile(TILE_ROOF_EDGE,8);

    for (int x=0; x<8; x++) {
        px(TILE_ROOF_EDGE,x,4,10);
        px(TILE_ROOF_EDGE,x,5,7);
        px(TILE_ROOF_EDGE,x,6,7);
    }

    px(TILE_ROOF_EDGE,0,7,7);
    px(TILE_ROOF_EDGE,7,7,7);

    /*
       Sloped roof corners.
    */

    fillTile(TILE_ROOF_CORNER_L,0);

    for (int y=0; y<8; y++) {
        int start = 7-y;

        for (int x=start; x<8; x++)
            px(
                TILE_ROOF_CORNER_L,
                x,y,8
            );

        if (start < 8)
            px(
                TILE_ROOF_CORNER_L,
                start,y,10
            );
    }

    fillTile(TILE_ROOF_CORNER_R,0);

    for (int y=0; y<8; y++) {
        int end = y;

        for (int x=0; x<=end; x++)
            px(
                TILE_ROOF_CORNER_R,
                x,y,8
            );

        px(
            TILE_ROOF_CORNER_R,
            end,y,10
        );
    }

    /*
       WALL
    */

    fillTile(TILE_WALL,5);

    for (int x=0; x<8; x++)
        px(TILE_WALL,x,7,6);

    px(TILE_WALL,1,2,15);
    px(TILE_WALL,6,2,15);

    fillTile(TILE_WALL_SHADOW,5);

    for (int x=0; x<8; x++) {
        px(TILE_WALL_SHADOW,x,6,6);
        px(TILE_WALL_SHADOW,x,7,7);
    }

    /*
       WINDOW
    */

    fillTile(TILE_WINDOW,5);

    rect(
        TILE_WINDOW,
        1,1,
        6,6,
        7
    );

    rect(
        TILE_WINDOW,
        2,2,
        5,5,
        14
    );

    px(TILE_WINDOW,2,2,15);
    px(TILE_WINDOW,3,2,15);

    for (int y=2; y<=5; y++)
        px(TILE_WINDOW,4,y,7);

    /*
       DOOR
    */

    fillTile(TILE_DOOR,5);

    rect(
        TILE_DOOR,
        1,0,
        6,7,
        7
    );

    rect(
        TILE_DOOR,
        2,1,
        5,7,
        6
    );

    px(TILE_DOOR,5,4,15);

    /*
       TREE GRAPHICS

       Transparent corners are critical.

       This is what makes several tiles
       merge into one canopy instead of
       visible green squares.
    */

    /* top-left */

    clearTile(TILE_TREE_TL);

    rect(TILE_TREE_TL,3,1,7,7,11);

    px(TILE_TREE_TL,5,0,13);
    px(TILE_TREE_TL,6,0,11);

    px(TILE_TREE_TL,2,3,13);
    px(TILE_TREE_TL,2,4,11);

    px(TILE_TREE_TL,4,2,12);
    px(TILE_TREE_TL,5,2,12);
    px(TILE_TREE_TL,3,4,12);

    px(TILE_TREE_TL,3,7,13);
    px(TILE_TREE_TL,4,7,13);

    /* top-right */

    clearTile(TILE_TREE_TR);

    rect(TILE_TREE_TR,0,1,4,7,11);

    px(TILE_TREE_TR,1,0,11);
    px(TILE_TREE_TR,2,0,13);

    px(TILE_TREE_TR,5,3,13);
    px(TILE_TREE_TR,5,4,11);

    px(TILE_TREE_TR,2,2,12);
    px(TILE_TREE_TR,3,2,12);
    px(TILE_TREE_TR,4,4,12);

    px(TILE_TREE_TR,3,7,13);
    px(TILE_TREE_TR,4,7,13);

    /*
       middle left
    */

    clearTile(TILE_TREE_ML);

    rect(TILE_TREE_ML,1,0,7,7,11);

    px(TILE_TREE_ML,0,2,13);
    px(TILE_TREE_ML,0,3,11);
    px(TILE_TREE_ML,0,4,11);

    px(TILE_TREE_ML,2,1,12);
    px(TILE_TREE_ML,3,1,12);

    px(TILE_TREE_ML,1,6,13);
    px(TILE_TREE_ML,2,7,13);
    px(TILE_TREE_ML,3,7,13);

    /*
       middle right
    */

    clearTile(TILE_TREE_MR);

    rect(TILE_TREE_MR,0,0,6,7,11);

    px(TILE_TREE_MR,7,2,13);
    px(TILE_TREE_MR,7,3,11);
    px(TILE_TREE_MR,7,4,11);

    px(TILE_TREE_MR,4,1,12);
    px(TILE_TREE_MR,5,1,12);

    px(TILE_TREE_MR,5,6,13);
    px(TILE_TREE_MR,4,7,13);

    /*
       lower canopy
    */

    clearTile(TILE_TREE_BL);

    rect(TILE_TREE_BL,2,0,7,4,11);

    px(TILE_TREE_BL,1,0,13);
    px(TILE_TREE_BL,1,1,11);

    px(TILE_TREE_BL,3,4,13);
    px(TILE_TREE_BL,4,5,13);
    px(TILE_TREE_BL,5,5,13);

    clearTile(TILE_TREE_BR);

    rect(TILE_TREE_BR,0,0,5,4,11);

    px(TILE_TREE_BR,6,0,13);
    px(TILE_TREE_BR,6,1,11);

    px(TILE_TREE_BR,2,4,13);
    px(TILE_TREE_BR,3,5,13);
    px(TILE_TREE_BR,4,5,13);

    /*
       TRUNK
    */

    clearTile(TILE_TREE_TRUNK);

    rect(
        TILE_TREE_TRUNK,
        3,0,
        4,6,
        7
    );

    px(TILE_TREE_TRUNK,2,6,7);
    px(TILE_TREE_TRUNK,5,6,7);

    px(TILE_TREE_TRUNK,2,7,6);
    px(TILE_TREE_TRUNK,3,7,6);
    px(TILE_TREE_TRUNK,4,7,6);
    px(TILE_TREE_TRUNK,5,7,6);

    /*
       BUSH

       IMPORTANT:
       this is a BOTTOM layer object.
    */

    fillTile(TILE_BUSH,1);

    rect(
        TILE_BUSH,
        1,4,
        6,6,
        11
    );

    rect(
        TILE_BUSH,
        2,3,
        5,6,
        11
    );

    px(TILE_BUSH,2,3,12);
    px(TILE_BUSH,5,4,12);

    px(TILE_BUSH,1,6,13);
    px(TILE_BUSH,6,6,13);

    /*
       FENCE
    */

    fillTile(TILE_FENCE,1);

    for (int x=0; x<8; x++) {
        px(TILE_FENCE,x,4,7);
        px(TILE_FENCE,x,6,6);
    }

    fillTile(TILE_FENCE_POST,1);

    rect(
        TILE_FENCE_POST,
        3,2,
        4,7,
        7
    );

    px(TILE_FENCE_POST,2,2,7);
    px(TILE_FENCE_POST,5,2,7);

    /*
       WATER
    */

    fillTile(TILE_WATER,14);

    px(TILE_WATER,1,2,15);
    px(TILE_WATER,2,2,15);

    px(TILE_WATER,5,6,15);
    px(TILE_WATER,6,6,15);

    fillTile(TILE_WATER_LIGHT,14);

    for (int x=1; x<=5; x++)
        px(
            TILE_WATER_LIGHT,
            x,3,15
        );

    fillTile(TILE_WATER_EDGE,1);

    for (int x=0; x<8; x++) {
        px(TILE_WATER_EDGE,x,4,3);
        px(TILE_WATER_EDGE,x,5,7);
        px(TILE_WATER_EDGE,x,6,14);
        px(TILE_WATER_EDGE,x,7,14);
    }
}
