#include "metatiles.h"
#include "tileset.h"

Metatile metatiles[META_COUNT];

static void setMeta(
    int id,
    u16 b0, u16 b1,
    u16 b2, u16 b3,
    u16 t0, u16 t1,
    u16 t2, u16 t3,
    int collision,
    int behavior,
    int elevation
)
{
    Metatile *m = &metatiles[id];

    m->bottom[0] = b0;
    m->bottom[1] = b1;
    m->bottom[2] = b2;
    m->bottom[3] = b3;

    m->top[0] = t0;
    m->top[1] = t1;
    m->top[2] = t2;
    m->top[3] = t3;

    m->collision = collision;
    m->behavior = behavior;
    m->elevation = elevation;
}

void metatilesInit(void)
{
    setMeta(
        META_GRASS,
        TILE_GRASS,TILE_GRASS,
        TILE_GRASS,TILE_GRASS,
        0,0,0,0,
        0,BEHAVIOR_GRASS,0
    );

    setMeta(
        META_GRASS_A,
        TILE_GRASS,TILE_GRASS_DETAIL_A,
        TILE_GRASS,TILE_GRASS,
        0,0,0,0,
        0,BEHAVIOR_GRASS,0
    );

    setMeta(
        META_GRASS_B,
        TILE_GRASS,TILE_GRASS,
        TILE_GRASS_DETAIL_B,TILE_GRASS,
        0,0,0,0,
        0,BEHAVIOR_GRASS,0
    );

    setMeta(
        META_PATH,
        TILE_PATH,TILE_PATH_LIGHT,
        TILE_PATH_LIGHT,TILE_PATH,
        0,0,0,0,
        0,BEHAVIOR_NORMAL,0
    );

    setMeta(
        META_PATH_TOP,
        TILE_PATH_EDGE_TOP,
        TILE_PATH_EDGE_TOP,
        TILE_PATH,
        TILE_PATH_LIGHT,
        0,0,0,0,
        0,BEHAVIOR_NORMAL,0
    );

    setMeta(
        META_PATH_BOTTOM,
        TILE_PATH,
        TILE_PATH_LIGHT,
        TILE_PATH_EDGE_BOTTOM,
        TILE_PATH_EDGE_BOTTOM,
        0,0,0,0,
        0,BEHAVIOR_NORMAL,0
    );

    setMeta(
        META_PATH_LEFT,
        TILE_PATH,TILE_PATH,
        TILE_PATH_LIGHT,TILE_PATH,
        0,0,0,0,
        0,BEHAVIOR_NORMAL,0
    );

    setMeta(
        META_PATH_RIGHT,
        TILE_PATH,TILE_PATH_LIGHT,
        TILE_PATH,TILE_PATH,
        0,0,0,0,
        0,BEHAVIOR_NORMAL,0
    );

    /* HOUSE ROOF */

    setMeta(
        META_ROOF_TL,
        TILE_ROOF_LIGHT,TILE_ROOF_LIGHT,
        TILE_ROOF,TILE_ROOF,
        0,0,0,0,
        1,BEHAVIOR_NORMAL,1
    );

    setMeta(
        META_ROOF_T,
        TILE_ROOF_LIGHT,TILE_ROOF_LIGHT,
        TILE_ROOF,TILE_ROOF,
        0,0,0,0,
        1,BEHAVIOR_NORMAL,1
    );

    setMeta(
        META_ROOF_TR,
        TILE_ROOF_LIGHT,TILE_ROOF_LIGHT,
        TILE_ROOF,TILE_ROOF,
        0,0,0,0,
        1,BEHAVIOR_NORMAL,1
    );

    setMeta(
        META_ROOF_L,
        TILE_ROOF,TILE_ROOF,
        TILE_ROOF,TILE_ROOF,
        0,0,0,0,
        1,BEHAVIOR_NORMAL,1
    );

    setMeta(
        META_ROOF_M,
        TILE_ROOF,TILE_ROOF,
        TILE_ROOF,TILE_ROOF,
        0,0,0,0,
        1,BEHAVIOR_NORMAL,1
    );

    setMeta(
        META_ROOF_R,
        TILE_ROOF,TILE_ROOF,
        TILE_ROOF,TILE_ROOF,
        0,0,0,0,
        1,BEHAVIOR_NORMAL,1
    );

    setMeta(
        META_ROOF_BL,
        TILE_ROOF,TILE_ROOF,
        TILE_ROOF_EDGE,TILE_ROOF_EDGE,
        0,0,0,0,
        1,BEHAVIOR_NORMAL,1
    );

    setMeta(
        META_ROOF_B,
        TILE_ROOF,TILE_ROOF,
        TILE_ROOF_EDGE,TILE_ROOF_EDGE,
        0,0,0,0,
        1,BEHAVIOR_NORMAL,1
    );

    setMeta(
        META_ROOF_BR,
        TILE_ROOF,TILE_ROOF,
        TILE_ROOF_EDGE,TILE_ROOF_EDGE,
        0,0,0,0,
        1,BEHAVIOR_NORMAL,1
    );

    /* HOUSE WALL */

    setMeta(
        META_WALL,
        TILE_WALL,TILE_WALL,
        TILE_WALL_SHADOW,TILE_WALL_SHADOW,
        0,0,0,0,
        1,BEHAVIOR_NORMAL,0
    );

    setMeta(
        META_WINDOW,
        TILE_WALL,TILE_WALL,
        TILE_WINDOW,TILE_WINDOW,
        0,0,0,0,
        1,BEHAVIOR_NORMAL,0
    );

    setMeta(
        META_DOOR,
        TILE_WALL,TILE_WALL,
        TILE_DOOR,TILE_DOOR,
        0,0,0,0,
        0,BEHAVIOR_DOOR,0
    );

    /*
       TREE

          TOP TOP
          LOW LOW

       Important:
       canopy is TOP layer.
       trunk stays BOTTOM.

       This is what fixes the old
       "green garbage over player" problem.
    */

    setMeta(
        META_TREE_TOP_L,
        TILE_GRASS,TILE_GRASS,
        TILE_GRASS,TILE_GRASS,
        TILE_TREE_LIGHT,
        TILE_TREE_MID,
        TILE_TREE_MID,
        TILE_TREE_DARK,
        1,BEHAVIOR_NORMAL,1
    );

    setMeta(
        META_TREE_TOP_R,
        TILE_GRASS,TILE_GRASS,
        TILE_GRASS,TILE_GRASS,
        TILE_TREE_MID,
        TILE_TREE_LIGHT,
        TILE_TREE_DARK,
        TILE_TREE_MID,
        1,BEHAVIOR_NORMAL,1
    );

    setMeta(
        META_TREE_BOTTOM_L,
        TILE_GRASS,TILE_GRASS,
        TILE_GRASS,TILE_TREE_TRUNK,
        TILE_TREE_DARK,
        TILE_TREE_MID,
        0,0,
        1,BEHAVIOR_NORMAL,0
    );

    setMeta(
        META_TREE_BOTTOM_R,
        TILE_GRASS,TILE_GRASS,
        TILE_TREE_TRUNK,TILE_GRASS,
        TILE_TREE_MID,
        TILE_TREE_DARK,
        0,0,
        1,BEHAVIOR_NORMAL,0
    );

    setMeta(
        META_BUSH,
        TILE_GRASS,TILE_GRASS,
        TILE_GRASS,TILE_GRASS,
        TILE_BUSH,TILE_BUSH,
        TILE_BUSH,TILE_BUSH,
        1,BEHAVIOR_NORMAL,0
    );

    setMeta(
        META_FLOWER,
        TILE_GRASS,TILE_GRASS,
        TILE_GRASS,TILE_GRASS,
        TILE_FLOWER,0,
        0,TILE_FLOWER,
        0,BEHAVIOR_NORMAL,0
    );

    setMeta(
        META_FENCE_H,
        TILE_GRASS,TILE_GRASS,
        TILE_GRASS,TILE_GRASS,
        TILE_FENCE_H,TILE_FENCE_H,
        TILE_FENCE_POST,TILE_FENCE_POST,
        1,BEHAVIOR_NORMAL,0
    );

    setMeta(
        META_FENCE_V,
        TILE_GRASS,TILE_GRASS,
        TILE_GRASS,TILE_GRASS,
        TILE_FENCE_V,TILE_FENCE_POST,
        TILE_FENCE_V,TILE_FENCE_POST,
        1,BEHAVIOR_NORMAL,0
    );

    /* WATER */

    setMeta(
        META_WATER,
        TILE_WATER,TILE_WATER_LIGHT,
        TILE_WATER_LIGHT,TILE_WATER,
        0,0,0,0,
        1,BEHAVIOR_WATER,0
    );

    setMeta(
        META_WATER_TOP,
        TILE_WATER_EDGE,TILE_WATER_EDGE,
        TILE_WATER,TILE_WATER_LIGHT,
        0,0,0,0,
        1,BEHAVIOR_WATER,0
    );

    setMeta(
        META_WATER_BOTTOM,
        TILE_WATER,TILE_WATER_LIGHT,
        TILE_WATER_EDGE,TILE_WATER_EDGE,
        0,0,0,0,
        1,BEHAVIOR_WATER,0
    );

    setMeta(
        META_WATER_LEFT,
        TILE_WATER,TILE_WATER_LIGHT,
        TILE_WATER_LIGHT,TILE_WATER,
        0,0,0,0,
        1,BEHAVIOR_WATER,0
    );

    setMeta(
        META_WATER_RIGHT,
        TILE_WATER_LIGHT,TILE_WATER,
        TILE_WATER,TILE_WATER_LIGHT,
        0,0,0,0,
        1,BEHAVIOR_WATER,0
    );
}
