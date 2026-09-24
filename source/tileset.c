#include <gba.h>
#include "tileset.h"

static u16 *gfx;

static void clearTile(int tile)
{
    for (int i=0; i<16; i++)
        gfx[tile*16+i] = 0;
}

static void px(int tile,int x,int y,int c)
{
    if (x<0 || x>7 || y<0 || y>7)
        return;

    int p = y*8+x;
    int word = tile*16+(p>>2);
    int shift = (p&3)*4;

    gfx[word] &= ~(0xF<<shift);
    gfx[word] |= (c&15)<<shift;
}

static void rect(
    int tile,
    int x1,int y1,
    int x2,int y2,
    int c
)
{
    for (int y=y1; y<=y2; y++)
        for (int x=x1; x<=x2; x++)
            px(tile,x,y,c);
}

static void fill(int tile,int c)
{
    rect(tile,0,0,7,7,c);
}

/* --------------------------------------------------
   TREE HELPERS
   -------------------------------------------------- */

static void treeBlob(
    int tile,
    int cx,
    int cy,
    int radius
)
{
    for (int y=0; y<8; y++)
    {
        for (int x=0; x<8; x++)
        {
            int dx=x-cx;
            int dy=y-cy;

            if (dx*dx+dy*dy <= radius*radius)
                px(tile,x,y,11);
        }
    }
}

static void treeShade(int tile)
{
    for (int x=1; x<7; x++)
        px(tile,x,7,13);

    px(tile,1,6,13);
    px(tile,6,6,13);

    px(tile,2,2,12);
    px(tile,3,1,12);
    px(tile,4,2,12);
}

static void makeTreeTile(
    int tile,
    int type
)
{
    clearTile(tile);

    switch(type)
    {
        case 0:
            treeBlob(tile,6,6,5);
            break;

        case 1:
            treeBlob(tile,3,5,6);
            break;

        case 2:
            treeBlob(tile,4,5,6);
            break;

        case 3:
            treeBlob(tile,1,6,5);
            break;

        case 4:
            treeBlob(tile,6,3,6);
            break;

        case 5:
            fill(tile,11);
            break;

        case 6:
            fill(tile,11);
            break;

        case 7:
            treeBlob(tile,1,3,6);
            break;

        case 8:
            treeBlob(tile,6,2,5);
            break;

        case 9:
            fill(tile,11);
            break;

        case 10:
            fill(tile,11);
            break;

        case 11:
            treeBlob(tile,1,2,5);
            break;

        case 12:
            treeBlob(tile,6,0,5);
            break;

        case 13:
            treeBlob(tile,3,0,6);
            break;

        case 14:
            treeBlob(tile,4,0,6);
            break;

        case 15:
            treeBlob(tile,1,0,5);
            break;
    }

    treeShade(tile);
}

void tilesetInit(void)
{
    gfx = (u16*)CHAR_BASE_ADR(0);

    for (int i=0; i<TILE_COUNT; i++)
        clearTile(i);

    /*
       Original palette inspired by
       colourful early-2000s GBA RPGs.
    */

    BG_PALETTE[0]  = RGB5(0,0,0);

    BG_PALETTE[1]  = RGB5(13,25,11); /* grass */
    BG_PALETTE[2]  = RGB5(8,19,8);   /* grass dark */

    BG_PALETTE[3]  = RGB5(25,21,13); /* path */
    BG_PALETTE[4]  = RGB5(30,26,18); /* path light */

    BG_PALETTE[5]  = RGB5(29,25,18); /* wall */
    BG_PALETTE[6]  = RGB5(20,16,11); /* wall shadow */

    BG_PALETTE[7]  = RGB5(8,7,6);    /* outline */

    BG_PALETTE[8]  = RGB5(24,7,6);   /* roof */
    BG_PALETTE[9]  = RGB5(31,13,9);  /* roof light */
    BG_PALETTE[10] = RGB5(14,4,4);   /* roof dark */

    BG_PALETTE[11] = RGB5(7,20,7);   /* tree */
    BG_PALETTE[12] = RGB5(16,28,11); /* tree light */
    BG_PALETTE[13] = RGB5(3,12,5);   /* tree dark */

    BG_PALETTE[14] = RGB5(8,20,29);  /* blue */
    BG_PALETTE[15] = RGB5(31,30,22); /* cream */

    /* ==================================================
       GRASS
       ================================================== */

    fill(TILE_GRASS,1);

    fill(TILE_GRASS_DETAIL,1);

    px(TILE_GRASS_DETAIL,1,6,2);
    px(TILE_GRASS_DETAIL,2,5,2);

    px(TILE_GRASS_DETAIL,6,2,2);
    px(TILE_GRASS_DETAIL,5,3,2);

    fill(TILE_FLOWER,1);

    px(TILE_FLOWER,3,3,15);
    px(TILE_FLOWER,2,4,15);
    px(TILE_FLOWER,4,4,15);
    px(TILE_FLOWER,3,5,2);

    /* ==================================================
       PATH
       ================================================== */

    fill(TILE_PATH,3);

    px(TILE_PATH,1,2,4);
    px(TILE_PATH,6,5,4);
    px(TILE_PATH,4,7,6);

    fill(TILE_PATH_EDGE_L,1);

    for (int y=0; y<8; y++)
    {
        px(TILE_PATH_EDGE_L,5,y,2);
        px(TILE_PATH_EDGE_L,6,y,3);
        px(TILE_PATH_EDGE_L,7,y,3);
    }

    fill(TILE_PATH_EDGE_R,1);

    for (int y=0; y<8; y++)
    {
        px(TILE_PATH_EDGE_R,0,y,3);
        px(TILE_PATH_EDGE_R,1,y,3);
        px(TILE_PATH_EDGE_R,2,y,2);
    }

    fill(TILE_PATH_EDGE_T,1);

    for (int x=0; x<8; x++)
    {
        px(TILE_PATH_EDGE_T,x,5,2);
        px(TILE_PATH_EDGE_T,x,6,3);
        px(TILE_PATH_EDGE_T,x,7,3);
    }

    fill(TILE_PATH_EDGE_B,1);

    for (int x=0; x<8; x++)
    {
        px(TILE_PATH_EDGE_B,x,0,3);
        px(TILE_PATH_EDGE_B,x,1,3);
        px(TILE_PATH_EDGE_B,x,2,2);
    }

    /* ==================================================
       ROOF

       Each piece is different now.
       ================================================== */

    clearTile(TILE_ROOF_TL);

    for (int y=0; y<8; y++)
    {
        int start=7-y;

        for (int x=start; x<8; x++)
            px(TILE_ROOF_TL,x,y,8);

        px(TILE_ROOF_TL,start,y,10);
    }

    fill(TILE_ROOF_TM,8);

    for (int x=0; x<8; x++)
        px(TILE_ROOF_TM,x,0,9);

    px(TILE_ROOF_TM,2,4,9);
    px(TILE_ROOF_TM,6,4,9);

    clearTile(TILE_ROOF_TR);

    for (int y=0; y<8; y++)
    {
        int end=y;

        for (int x=0; x<=end; x++)
            px(TILE_ROOF_TR,x,y,8);

        px(TILE_ROOF_TR,end,y,10);
    }

    fill(TILE_ROOF_ML,8);

    for (int y=0; y<8; y++)
        px(TILE_ROOF_ML,0,y,10);

    px(TILE_ROOF_ML,3,2,9);

    fill(TILE_ROOF_MM,8);

    for (int x=0; x<8; x++)
        px(TILE_ROOF_MM,x,7,10);

    px(TILE_ROOF_MM,2,2,9);
    px(TILE_ROOF_MM,6,2,9);

    fill(TILE_ROOF_MR,8);

    for (int y=0; y<8; y++)
        px(TILE_ROOF_MR,7,y,10);

    px(TILE_ROOF_MR,4,2,9);

    fill(TILE_ROOF_BL,8);

    for (int x=0; x<8; x++)
    {
        px(TILE_ROOF_BL,x,5,10);
        px(TILE_ROOF_BL,x,6,7);
    }

    px(TILE_ROOF_BL,0,7,7);
    px(TILE_ROOF_BL,1,7,7);

    fill(TILE_ROOF_BM,8);

    for (int x=0; x<8; x++)
    {
        px(TILE_ROOF_BM,x,5,10);
        px(TILE_ROOF_BM,x,6,7);
        px(TILE_ROOF_BM,x,7,6);
    }

    fill(TILE_ROOF_BR,8);

    for (int x=0; x<8; x++)
    {
        px(TILE_ROOF_BR,x,5,10);
        px(TILE_ROOF_BR,x,6,7);
    }

    px(TILE_ROOF_BR,6,7,7);
    px(TILE_ROOF_BR,7,7,7);

    /* ==================================================
       WALL
       ================================================== */

    fill(TILE_WALL,5);

    px(TILE_WALL,0,0,15);
    px(TILE_WALL,7,0,15);

    fill(TILE_WALL_BASE,5);

    for (int x=0; x<8; x++)
    {
        px(TILE_WALL_BASE,x,5,6);
        px(TILE_WALL_BASE,x,6,6);
        px(TILE_WALL_BASE,x,7,7);
    }

    /* ==================================================
       BIG WINDOWS
       ================================================== */

    fill(TILE_WINDOW_T,5);

    rect(TILE_WINDOW_T,1,2,6,7,7);
    rect(TILE_WINDOW_T,2,3,5,7,14);

    px(TILE_WINDOW_T,2,3,15);
    px(TILE_WINDOW_T,3,3,15);

    fill(TILE_WINDOW_B,5);

    rect(TILE_WINDOW_B,1,0,6,4,7);
    rect(TILE_WINDOW_B,2,0,5,3,14);

    for (int y=0; y<=3; y++)
        px(TILE_WINDOW_B,4,y,7);

    px(TILE_WINDOW_B,0,6,6);
    px(TILE_WINDOW_B,7,6,6);

    /* ==================================================
       32 PIXEL DOOR

       Two separate 16px metatiles use these
       top/bottom graphics.
       ================================================== */

    fill(TILE_DOOR_T,5);

    rect(TILE_DOOR_T,1,0,6,7,7);
    rect(TILE_DOOR_T,2,1,5,7,6);

    px(TILE_DOOR_T,2,1,15);
    px(TILE_DOOR_T,3,1,15);

    fill(TILE_DOOR_B,5);

    rect(TILE_DOOR_B,1,0,6,7,7);
    rect(TILE_DOOR_B,2,0,5,6,6);

    px(TILE_DOOR_B,5,3,15);

    for (int x=1; x<=6; x++)
        px(TILE_DOOR_B,x,7,7);

    /* ==================================================
       TREE — 32x32 UNIQUE GRAPHIC

       Sixteen unique 8x8 pieces.
       No repeated 16x16 tree blocks.
       ================================================== */

    makeTreeTile(TILE_TREE_00,0);
    makeTreeTile(TILE_TREE_01,1);
    makeTreeTile(TILE_TREE_02,2);
    makeTreeTile(TILE_TREE_03,3);

    makeTreeTile(TILE_TREE_10,4);
    makeTreeTile(TILE_TREE_11,5);
    makeTreeTile(TILE_TREE_12,6);
    makeTreeTile(TILE_TREE_13,7);

    makeTreeTile(TILE_TREE_20,8);
    makeTreeTile(TILE_TREE_21,9);
    makeTreeTile(TILE_TREE_22,10);
    makeTreeTile(TILE_TREE_23,11);

    makeTreeTile(TILE_TREE_30,12);
    makeTreeTile(TILE_TREE_31,13);
    makeTreeTile(TILE_TREE_32,14);
    makeTreeTile(TILE_TREE_33,15);

    /* ==================================================
       BUSH
       ================================================== */

    fill(TILE_BUSH,1);

    rect(TILE_BUSH,1,4,6,6,11);
    rect(TILE_BUSH,2,3,5,6,11);

    px(TILE_BUSH,2,3,12);
    px(TILE_BUSH,5,4,12);

    px(TILE_BUSH,1,6,13);
    px(TILE_BUSH,6,6,13);

    /* ==================================================
       WATER
       ================================================== */

    fill(TILE_WATER,14);

    px(TILE_WATER,1,2,15);
    px(TILE_WATER,2,2,15);

    px(TILE_WATER,5,6,15);
    px(TILE_WATER,6,6,15);
}
