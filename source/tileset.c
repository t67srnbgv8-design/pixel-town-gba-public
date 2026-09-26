#include <gba.h>
#include "tileset.h"

static u16 *gfx;

static void clearTile(int tile)
{
    int i;

    for (i = 0; i < 16; i++)
        gfx[tile * 16 + i] = 0;
}

static void px(
    int tile,
    int x,
    int y,
    int color
)
{
    int p;
    int word;
    int shift;

    if (x < 0 || x > 7 || y < 0 || y > 7)
        return;

    p = y * 8 + x;
    word = tile * 16 + (p >> 2);
    shift = (p & 3) * 4;

    gfx[word] &= ~(0xF << shift);
    gfx[word] |= (color & 15) << shift;
}

static void rect(
    int tile,
    int x1,
    int y1,
    int x2,
    int y2,
    int color
)
{
    int x;
    int y;

    for (y = y1; y <= y2; y++)
    {
        for (x = x1; x <= x2; x++)
            px(tile, x, y, color);
    }
}

static void fill(
    int tile,
    int color
)
{
    rect(
        tile,
        0, 0,
        7, 7,
        color
    );
}

/* =========================================================
   TREE
   ========================================================= */

static void treeBlob(
    int tile,
    int cx,
    int cy,
    int radius
)
{
    int x;
    int y;

    for (y = 0; y < 8; y++)
    {
        for (x = 0; x < 8; x++)
        {
            int dx = x - cx;
            int dy = y - cy;

            if (
                dx * dx +
                dy * dy <=
                radius * radius
            )
            {
                px(
                    tile,
                    x,
                    y,
                    11
                );
            }
        }
    }
}

static void treeHighlight(
    int tile,
    int type
)
{
    if ((type & 1) == 0)
    {
        px(tile, 2, 2, 12);
        px(tile, 3, 2, 12);
        px(tile, 2, 3, 12);
    }

    if ((type & 2) != 0)
    {
        px(tile, 5, 4, 13);
        px(tile, 6, 5, 13);
    }
}

static void makeTreeTile(
    int tile,
    int type
)
{
    clearTile(tile);

    switch (type)
    {
        case 0:
            treeBlob(tile, 6, 6, 5);
            break;

        case 1:
            treeBlob(tile, 3, 5, 6);
            break;

        case 2:
            treeBlob(tile, 4, 5, 6);
            break;

        case 3:
            treeBlob(tile, 1, 6, 5);
            break;

        case 4:
            treeBlob(tile, 6, 3, 6);
            break;

        case 5:
        case 6:
        case 9:
        case 10:
            fill(tile, 11);
            break;

        case 7:
            treeBlob(tile, 1, 3, 6);
            break;

        case 8:
            treeBlob(tile, 6, 2, 5);
            break;

        case 11:
            treeBlob(tile, 1, 2, 5);
            break;

        case 12:
            treeBlob(tile, 6, 0, 5);
            break;

        case 13:
            treeBlob(tile, 3, 0, 6);
            break;

        case 14:
            treeBlob(tile, 4, 0, 6);
            break;

        case 15:
            treeBlob(tile, 1, 0, 5);
            break;
    }

    treeHighlight(
        tile,
        type
    );
}

/* =========================================================
   TILESET
   ========================================================= */

void tilesetInit(void)
{
    int i;
    int x;
    int y;

    gfx =
        (u16 *)CHAR_BASE_ADR(0);

    for (i = 0; i < TILE_COUNT; i++)
        clearTile(i);

    /* =====================================================
       PALETTE
       ===================================================== */

    BG_PALETTE[0] =
        RGB5(0,0,0);

    BG_PALETTE[1] =
        RGB5(13,25,11);

    BG_PALETTE[2] =
        RGB5(7,18,7);

    BG_PALETTE[3] =
        RGB5(24,20,12);

    BG_PALETTE[4] =
        RGB5(30,26,18);

    BG_PALETTE[5] =
        RGB5(29,25,18);

    BG_PALETTE[6] =
        RGB5(20,16,11);

    BG_PALETTE[7] =
        RGB5(7,6,6);

    BG_PALETTE[8] =
        RGB5(23,6,6);

    BG_PALETTE[9] =
        RGB5(31,13,9);

    BG_PALETTE[10] =
        RGB5(13,3,4);

    BG_PALETTE[11] =
        RGB5(7,20,7);

    BG_PALETTE[12] =
        RGB5(16,28,11);

    BG_PALETTE[13] =
        RGB5(3,11,5);

    BG_PALETTE[14] =
        RGB5(8,20,29);

    BG_PALETTE[15] =
        RGB5(31,30,22);

    /* =====================================================
       GRASS
       ===================================================== */

    fill(
        TILE_GRASS,
        1
    );

    fill(
        TILE_GRASS_DETAIL,
        1
    );

    px(TILE_GRASS_DETAIL,1,6,2);
    px(TILE_GRASS_DETAIL,2,5,2);

    px(TILE_GRASS_DETAIL,5,2,2);
    px(TILE_GRASS_DETAIL,6,3,2);

    fill(
        TILE_FLOWER,
        1
    );

    px(TILE_FLOWER,3,2,15);
    px(TILE_FLOWER,2,3,15);
    px(TILE_FLOWER,4,3,15);
    px(TILE_FLOWER,3,4,2);
    px(TILE_FLOWER,3,5,2);

    /* =====================================================
       PATH
       ===================================================== */

    fill(
        TILE_PATH,
        3
    );

    px(TILE_PATH,1,2,4);
    px(TILE_PATH,6,5,4);
    px(TILE_PATH,3,7,6);

    fill(
        TILE_PATH_EDGE_L,
        1
    );

    for (y = 0; y < 8; y++)
    {
        px(TILE_PATH_EDGE_L,5,y,2);
        px(TILE_PATH_EDGE_L,6,y,3);
        px(TILE_PATH_EDGE_L,7,y,3);
    }

    fill(
        TILE_PATH_EDGE_R,
        1
    );

    for (y = 0; y < 8; y++)
    {
        px(TILE_PATH_EDGE_R,0,y,3);
        px(TILE_PATH_EDGE_R,1,y,3);
        px(TILE_PATH_EDGE_R,2,y,2);
    }

    fill(
        TILE_PATH_EDGE_T,
        1
    );

    for (x = 0; x < 8; x++)
    {
        px(TILE_PATH_EDGE_T,x,5,2);
        px(TILE_PATH_EDGE_T,x,6,3);
        px(TILE_PATH_EDGE_T,x,7,3);
    }

    fill(
        TILE_PATH_EDGE_B,
        1
    );

    for (x = 0; x < 8; x++)
    {
        px(TILE_PATH_EDGE_B,x,0,3);
        px(TILE_PATH_EDGE_B,x,1,3);
        px(TILE_PATH_EDGE_B,x,2,2);
    }

    /* =====================================================
       HOUSE ROOF

       Sloped upper pieces + tiled middle +
       dark projecting eave.
       ===================================================== */

    clearTile(
        TILE_ROOF_SLOPE_L
    );

    for (y = 0; y < 8; y++)
    {
        int start =
            7 - y;

        for (
            x = start;
            x < 8;
            x++
        )
        {
            px(
                TILE_ROOF_SLOPE_L,
                x,
                y,
                8
            );
        }

        px(
            TILE_ROOF_SLOPE_L,
            start,
            y,
            10
        );

        if (start + 1 < 8)
        {
            px(
                TILE_ROOF_SLOPE_L,
                start + 1,
                y,
                9
            );
        }
    }

    fill(
        TILE_ROOF_TOP,
        8
    );

    for (x = 0; x < 8; x++)
        px(TILE_ROOF_TOP,x,0,10);

    for (x = 1; x < 8; x += 4)
        px(TILE_ROOF_TOP,x,2,9);

    for (x = 3; x < 8; x += 4)
        px(TILE_ROOF_TOP,x,5,10);

    clearTile(
        TILE_ROOF_SLOPE_R
    );

    for (y = 0; y < 8; y++)
    {
        int end = y;

        for (
            x = 0;
            x <= end;
            x++
        )
        {
            px(
                TILE_ROOF_SLOPE_R,
                x,
                y,
                8
            );
        }

        px(
            TILE_ROOF_SLOPE_R,
            end,
            y,
            10
        );

        if (end - 1 >= 0)
        {
            px(
                TILE_ROOF_SLOPE_R,
                end - 1,
                y,
                9
            );
        }
    }

    fill(
        TILE_ROOF_RED,
        8
    );

    for (x = 0; x < 8; x++)
    {
        if ((x & 3) == 0)
            px(TILE_ROOF_RED,x,3,10);

        if ((x & 3) == 2)
            px(TILE_ROOF_RED,x,6,9);
    }

    fill(
        TILE_ROOF_RED_LIGHT,
        8
    );

    for (x = 0; x < 8; x++)
    {
        px(
            TILE_ROOF_RED_LIGHT,
            x,
            0,
            9
        );
    }

    px(TILE_ROOF_RED_LIGHT,2,3,9);
    px(TILE_ROOF_RED_LIGHT,6,5,10);

    fill(
        TILE_ROOF_RED_DARK,
        8
    );

    for (x = 0; x < 8; x++)
    {
        px(
            TILE_ROOF_RED_DARK,
            x,
            6,
            10
        );

        px(
            TILE_ROOF_RED_DARK,
            x,
            7,
            10
        );
    }

    px(TILE_ROOF_RED_DARK,1,2,9);
    px(TILE_ROOF_RED_DARK,5,4,9);

    fill(
        TILE_ROOF_EAVE_L,
        8
    );

    for (x = 0; x < 8; x++)
    {
        px(
            TILE_ROOF_EAVE_L,
            x,
            4,
            10
        );

        px(
            TILE_ROOF_EAVE_L,
            x,
            5,
            7
        );

        px(
            TILE_ROOF_EAVE_L,
            x,
            6,
            7
        );
    }

    px(TILE_ROOF_EAVE_L,0,7,7);
    px(TILE_ROOF_EAVE_L,1,7,7);

    fill(
        TILE_ROOF_EAVE_M,
        8
    );

    for (x = 0; x < 8; x++)
    {
        px(TILE_ROOF_EAVE_M,x,4,10);
        px(TILE_ROOF_EAVE_M,x,5,7);
        px(TILE_ROOF_EAVE_M,x,6,7);
        px(TILE_ROOF_EAVE_M,x,7,6);
    }

    fill(
        TILE_ROOF_EAVE_R,
        8
    );

    for (x = 0; x < 8; x++)
    {
        px(
            TILE_ROOF_EAVE_R,
            x,
            4,
            10
        );

        px(
            TILE_ROOF_EAVE_R,
            x,
            5,
            7
        );

        px(
            TILE_ROOF_EAVE_R,
            x,
            6,
            7
        );
    }

    px(TILE_ROOF_EAVE_R,6,7,7);
    px(TILE_ROOF_EAVE_R,7,7,7);

    /* =====================================================
       WALL
       ===================================================== */

    fill(
        TILE_WALL,
        5
    );

    for (y = 1; y < 8; y += 4)
    {
        px(TILE_WALL,1,y,15);
        px(TILE_WALL,6,y,6);
    }

    fill(
        TILE_WALL_DETAIL,
        5
    );

    for (x = 0; x < 8; x++)
    {
        px(
            TILE_WALL_DETAIL,
            x,
            0,
            15
        );
    }

    px(TILE_WALL_DETAIL,1,3,6);
    px(TILE_WALL_DETAIL,6,5,6);

    fill(
        TILE_WALL_BASE,
        5
    );

    for (x = 0; x < 8; x++)
    {
        px(TILE_WALL_BASE,x,5,6);
        px(TILE_WALL_BASE,x,6,6);
        px(TILE_WALL_BASE,x,7,7);
    }

    /* =====================================================
       WINDOW

       One window = full 16x16 metatile.
       ===================================================== */

    fill(
        TILE_WINDOW_TL,
        5
    );

    rect(
        TILE_WINDOW_TL,
        2,2,
        7,7,
        7
    );

    rect(
        TILE_WINDOW_TL,
        3,3,
        7,7,
        14
    );

    px(TILE_WINDOW_TL,3,3,15);
    px(TILE_WINDOW_TL,4,3,15);

    fill(
        TILE_WINDOW_TR,
        5
    );

    rect(
        TILE_WINDOW_TR,
        0,2,
        5,7,
        7
    );

    rect(
        TILE_WINDOW_TR,
        0,3,
        4,7,
        14
    );

    px(TILE_WINDOW_TR,1,3,15);

    fill(
        TILE_WINDOW_BL,
        5
    );

    rect(
        TILE_WINDOW_BL,
        2,0,
        7,4,
        7
    );

    rect(
        TILE_WINDOW_BL,
        3,0,
        7,3,
        14
    );

    for (y = 0; y <= 3; y++)
        px(TILE_WINDOW_BL,7,y,7);

    rect(
        TILE_WINDOW_BL,
        1,5,
        7,6,
        6
    );

    fill(
        TILE_WINDOW_BR,
        5
    );

    rect(
        TILE_WINDOW_BR,
        0,0,
        5,4,
        7
    );

    rect(
        TILE_WINDOW_BR,
        0,0,
        4,3,
        14
    );

    rect(
        TILE_WINDOW_BR,
        0,5,
        6,6,
        6
    );

    /* =====================================================
       DOOR

       Full 16x32 entrance when two metatiles
       are stacked vertically.
       ===================================================== */

    fill(
        TILE_DOOR_TL,
        5
    );

    rect(
        TILE_DOOR_TL,
        3,1,
        7,7,
        7
    );

    rect(
        TILE_DOOR_TL,
        4,2,
        7,7,
        6
    );

    fill(
        TILE_DOOR_TR,
        5
    );

    rect(
        TILE_DOOR_TR,
        0,1,
        4,7,
        7
    );

    rect(
        TILE_DOOR_TR,
        0,2,
        3,7,
        6
    );

    fill(
        TILE_DOOR_BL,
        5
    );

    rect(
        TILE_DOOR_BL,
        3,0,
        7,7,
        7
    );

    rect(
        TILE_DOOR_BL,
        4,0,
        7,6,
        6
    );

    fill(
        TILE_DOOR_BR,
        5
    );

    rect(
        TILE_DOOR_BR,
        0,0,
        4,7,
        7
    );

    rect(
        TILE_DOOR_BR,
        0,0,
        3,6,
        6
    );

    px(
        TILE_DOOR_BR,
        2,
        3,
        15
    );

    for (x = 0; x < 8; x++)
    {
        px(
            TILE_DOOR_BL,
            x,
            7,
            7
        );

        px(
            TILE_DOOR_BR,
            x,
            7,
            7
        );
    }

    /* =====================================================
       SIGN
       ===================================================== */

    fill(
        TILE_SIGN,
        5
    );

    rect(
        TILE_SIGN,
        1,2,
        6,6,
        6
    );

    rect(
        TILE_SIGN,
        2,3,
        5,5,
        15
    );

    px(TILE_SIGN,3,4,8);
    px(TILE_SIGN,4,4,8);

    /* =====================================================
       FLOWER BOX
       ===================================================== */

    fill(
        TILE_FLOWER_BOX,
        5
    );

    rect(
        TILE_FLOWER_BOX,
        0,5,
        7,7,
        6
    );

    px(TILE_FLOWER_BOX,1,4,9);
    px(TILE_FLOWER_BOX,3,3,15);
    px(TILE_FLOWER_BOX,5,4,9);

    /* =====================================================
       TREES
       ===================================================== */

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

    /* =====================================================
       BUSH
       ===================================================== */

    fill(
        TILE_BUSH,
        1
    );

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

    /* =====================================================
       WATER
       ===================================================== */

    fill(
        TILE_WATER,
        14
    );

    px(TILE_WATER,1,2,15);
    px(TILE_WATER,2,2,15);

    px(TILE_WATER,5,6,15);
    px(TILE_WATER,6,6,15);
}
