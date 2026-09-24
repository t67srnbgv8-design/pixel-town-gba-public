#include <gba.h>
#include "tileset.h"

static void pixel(u16 *tile, int x, int y, u8 c)
{
    int p = y * 8 + x;
    int word = p >> 2;
    int shift = (p & 3) * 4;

    tile[word] &= ~(0xF << shift);
    tile[word] |= (c & 0xF) << shift;
}

static void fill(u16 *tile, u8 c)
{
    u16 v = c | (c << 4) | (c << 8) | (c << 12);

    for (int i = 0; i < 16; i++)
        tile[i] = v;
}

void tilesetInit(void)
{
    u16 *gfx = (u16 *)CHAR_BASE_ADR(0);

    /* Palette */
    BG_PALETTE[0]  = RGB5(0,0,0);
    BG_PALETTE[1]  = RGB5(10,23,10);   /* grass */
    BG_PALETTE[2]  = RGB5(5,17,6);
    BG_PALETTE[3]  = RGB5(25,21,13);   /* path */
    BG_PALETTE[4]  = RGB5(30,25,16);
    BG_PALETTE[5]  = RGB5(24,20,14);
    BG_PALETTE[6]  = RGB5(29,25,19);   /* wall */
    BG_PALETTE[7]  = RGB5(15,11,7);
    BG_PALETTE[8]  = RGB5(22,5,4);     /* roof */
    BG_PALETTE[9]  = RGB5(30,10,7);
    BG_PALETTE[10] = RGB5(14,3,3);
    BG_PALETTE[11] = RGB5(12,7,3);     /* wood */
    BG_PALETTE[12] = RGB5(6,18,29);    /* water */
    BG_PALETTE[13] = RGB5(17,28,31);
    BG_PALETTE[14] = RGB5(3,15,5);     /* foliage dark */
    BG_PALETTE[15] = RGB5(8,27,10);    /* foliage light */

    fill(&gfx[TILE_EMPTY * 16], 0);

    /* grass */
    fill(&gfx[TILE_GRASS * 16], 1);

    fill(&gfx[TILE_GRASS_DETAIL_A * 16], 1);
    pixel(&gfx[TILE_GRASS_DETAIL_A * 16], 1,6,2);
    pixel(&gfx[TILE_GRASS_DETAIL_A * 16], 2,5,2);
    pixel(&gfx[TILE_GRASS_DETAIL_A * 16], 6,2,2);

    fill(&gfx[TILE_GRASS_DETAIL_B * 16], 1);
    pixel(&gfx[TILE_GRASS_DETAIL_B * 16], 2,2,15);
    pixel(&gfx[TILE_GRASS_DETAIL_B * 16], 3,1,15);
    pixel(&gfx[TILE_GRASS_DETAIL_B * 16], 6,6,2);

    /* paths */
    fill(&gfx[TILE_PATH * 16], 3);
    pixel(&gfx[TILE_PATH * 16], 1,2,4);
    pixel(&gfx[TILE_PATH * 16], 5,1,4);
    pixel(&gfx[TILE_PATH * 16], 3,6,4);
    pixel(&gfx[TILE_PATH * 16], 7,4,4);

    fill(&gfx[TILE_PATH_LIGHT * 16], 3);
    pixel(&gfx[TILE_PATH_LIGHT * 16], 2,1,4);
    pixel(&gfx[TILE_PATH_LIGHT * 16], 6,5,4);
    pixel(&gfx[TILE_PATH_LIGHT * 16], 4,7,4);

    fill(&gfx[TILE_PATH_EDGE_TOP * 16], 1);
    for (int x=0; x<8; x++) {
        pixel(&gfx[TILE_PATH_EDGE_TOP * 16],x,5,4);
        pixel(&gfx[TILE_PATH_EDGE_TOP * 16],x,6,3);
        pixel(&gfx[TILE_PATH_EDGE_TOP * 16],x,7,3);
    }

    fill(&gfx[TILE_PATH_EDGE_BOTTOM * 16],1);
    for (int x=0; x<8; x++) {
        pixel(&gfx[TILE_PATH_EDGE_BOTTOM * 16],x,0,3);
        pixel(&gfx[TILE_PATH_EDGE_BOTTOM * 16],x,1,3);
        pixel(&gfx[TILE_PATH_EDGE_BOTTOM * 16],x,2,4);
    }

    /* wall */
    fill(&gfx[TILE_WALL * 16],6);
    for (int x=0; x<8; x++) {
        pixel(&gfx[TILE_WALL * 16],x,3,5);
        pixel(&gfx[TILE_WALL * 16],x,7,5);
    }

    fill(&gfx[TILE_WALL_SHADOW * 16],5);
    for (int x=0; x<8; x++)
        pixel(&gfx[TILE_WALL_SHADOW * 16],x,7,7);

    /* window */
    fill(&gfx[TILE_WINDOW * 16],6);
    for (int y=1; y<=6; y++)
        for (int x=1; x<=6; x++)
            pixel(&gfx[TILE_WINDOW * 16],x,y,12);

    for (int y=1; y<=6; y++)
        pixel(&gfx[TILE_WINDOW * 16],4,y,13);

    for (int x=1; x<=6; x++)
        pixel(&gfx[TILE_WINDOW * 16],x,4,13);

    /* door */
    fill(&gfx[TILE_DOOR * 16],11);
    for (int y=0; y<8; y++) {
        pixel(&gfx[TILE_DOOR * 16],0,y,7);
        pixel(&gfx[TILE_DOOR * 16],7,y,7);
    }
    pixel(&gfx[TILE_DOOR * 16],5,4,15);

    /* roof */
    fill(&gfx[TILE_ROOF * 16],8);
    for (int y=1; y<8; y+=3)
        for (int x=0; x<8; x++)
            pixel(&gfx[TILE_ROOF * 16],x,y,9);

    fill(&gfx[TILE_ROOF_LIGHT * 16],9);
    for (int x=0; x<8; x++) {
        pixel(&gfx[TILE_ROOF_LIGHT * 16],x,6,8);
        pixel(&gfx[TILE_ROOF_LIGHT * 16],x,7,8);
    }

    fill(&gfx[TILE_ROOF_DARK * 16],10);
    for (int x=0; x<8; x++)
        pixel(&gfx[TILE_ROOF_DARK * 16],x,0,8);

    fill(&gfx[TILE_ROOF_EDGE * 16],8);
    for (int x=0; x<8; x++) {
        pixel(&gfx[TILE_ROOF_EDGE * 16],x,4,10);
        pixel(&gfx[TILE_ROOF_EDGE * 16],x,5,7);
        pixel(&gfx[TILE_ROOF_EDGE * 16],x,6,7);
        pixel(&gfx[TILE_ROOF_EDGE * 16],x,7,7);
    }

    /* foliage */
    fill(&gfx[TILE_TREE_LIGHT * 16],0);
    for (int y=0; y<8; y++)
        for (int x=0; x<8; x++) {
            int dx=x-4, dy=y-4;
            if (dx*dx + dy*dy < 24)
                pixel(&gfx[TILE_TREE_LIGHT * 16],x,y,
                      ((x+y)%3==0) ? 15 : 14);
        }

    fill(&gfx[TILE_TREE_MID * 16],0);
    for (int y=0; y<8; y++)
        for (int x=0; x<8; x++) {
            int dx=x-3, dy=y-3;
            if (dx*dx + dy*dy < 26)
                pixel(&gfx[TILE_TREE_MID * 16],x,y,
                      ((x+y)&1) ? 15 : 14);
        }

    fill(&gfx[TILE_TREE_DARK * 16],0);
    for (int y=0; y<8; y++)
        for (int x=0; x<8; x++) {
            int dx=x-4, dy=y-3;
            if (dx*dx + dy*dy < 25)
                pixel(&gfx[TILE_TREE_DARK * 16],x,y,
                      ((x+y)&1) ? 14 : 2);
        }

    fill(&gfx[TILE_TREE_TRUNK * 16],0);
    for (int y=0; y<8; y++) {
        pixel(&gfx[TILE_TREE_TRUNK * 16],3,y,11);
        pixel(&gfx[TILE_TREE_TRUNK * 16],4,y,11);
    }
    pixel(&gfx[TILE_TREE_TRUNK * 16],2,7,11);
    pixel(&gfx[TILE_TREE_TRUNK * 16],5,7,11);

    /* bush */
    fill(&gfx[TILE_BUSH * 16],0);
    for (int y=2; y<=6; y++)
        for (int x=1; x<=6; x++)
            pixel(&gfx[TILE_BUSH * 16],x,y,
                  ((x+y)&1) ? 15 : 14);

    /* flowers */
    fill(&gfx[TILE_FLOWER * 16],0);
    pixel(&gfx[TILE_FLOWER * 16],2,2,9);
    pixel(&gfx[TILE_FLOWER * 16],3,2,13);
    pixel(&gfx[TILE_FLOWER * 16],2,3,13);
    pixel(&gfx[TILE_FLOWER * 16],6,5,9);
    pixel(&gfx[TILE_FLOWER * 16],5,5,13);

    /* fences */
    fill(&gfx[TILE_FENCE_H * 16],0);
    for (int x=0; x<8; x++) {
        pixel(&gfx[TILE_FENCE_H * 16],x,3,11);
        pixel(&gfx[TILE_FENCE_H * 16],x,5,11);
    }

    fill(&gfx[TILE_FENCE_V * 16],0);
    for (int y=0; y<8; y++) {
        pixel(&gfx[TILE_FENCE_V * 16],3,y,11);
        pixel(&gfx[TILE_FENCE_V * 16],5,y,11);
    }

    fill(&gfx[TILE_FENCE_POST * 16],0);
    for (int y=1; y<8; y++)
        for (int x=2; x<=5; x++)
            pixel(&gfx[TILE_FENCE_POST * 16],x,y,11);

    /* water */
    fill(&gfx[TILE_WATER * 16],12);
    pixel(&gfx[TILE_WATER * 16],1,2,13);
    pixel(&gfx[TILE_WATER * 16],2,2,13);
    pixel(&gfx[TILE_WATER * 16],5,6,13);

    fill(&gfx[TILE_WATER_LIGHT * 16],12);
    for (int x=1; x<=5; x++)
        pixel(&gfx[TILE_WATER_LIGHT * 16],x,3,13);
    for (int x=3; x<=7; x++)
        pixel(&gfx[TILE_WATER_LIGHT * 16],x,7,13);

    fill(&gfx[TILE_WATER_EDGE * 16],4);
    for (int x=0; x<8; x++) {
        pixel(&gfx[TILE_WATER_EDGE * 16],x,5,3);
        pixel(&gfx[TILE_WATER_EDGE * 16],x,6,12);
        pixel(&gfx[TILE_WATER_EDGE * 16],x,7,12);
    }

    /* soil */
    fill(&gfx[TILE_SOIL * 16],4);
    for (int y=1; y<8; y+=3)
        for (int x=0; x<8; x++)
            pixel(&gfx[TILE_SOIL * 16],x,y,3);

    /* stone */
    fill(&gfx[TILE_STONE * 16],0);
    for (int y=3; y<=6; y++)
        for (int x=2; x<=5; x++)
            pixel(&gfx[TILE_STONE * 16],x,y,5);

    /* sign */
    fill(&gfx[TILE_SIGN * 16],0);
    for (int y=1; y<=4; y++)
        for (int x=1; x<=6; x++)
            pixel(&gfx[TILE_SIGN * 16],x,y,11);

    pixel(&gfx[TILE_SIGN * 16],3,5,11);
    pixel(&gfx[TILE_SIGN * 16],4,5,11);
    pixel(&gfx[TILE_SIGN * 16],3,6,11);
    pixel(&gfx[TILE_SIGN * 16],4,6,11);
    pixel(&gfx[TILE_SIGN * 16],3,7,11);
    pixel(&gfx[TILE_SIGN * 16],4,7,11);
}
