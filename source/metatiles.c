#include "metatiles.h"
#include "tileset.h"

Metatile metatiles[META_COUNT];

static void setMeta(
    int id,

    int b0,int b1,
    int b2,int b3,

    int t0,int t1,
    int t2,int t3,

    int collision,
    int behavior
)
{
    Metatile *m=&metatiles[id];

    m->bottom[0]=b0;
    m->bottom[1]=b1;
    m->bottom[2]=b2;
    m->bottom[3]=b3;

    m->top[0]=t0;
    m->top[1]=t1;
    m->top[2]=t2;
    m->top[3]=t3;

    m->collision=collision;
    m->behavior=behavior;
    m->elevation=0;
}

void metatilesInit(void)
{
    setMeta(
        META_GRASS,

        TILE_GRASS,TILE_GRASS,
        TILE_GRASS,TILE_GRASS,

        0,0,0,0,

        0,
        BEHAVIOR_NORMAL
    );

    setMeta(
        META_GRASS_DETAIL,

        TILE_GRASS,TILE_GRASS_DETAIL,
        TILE_GRASS,TILE_GRASS,

        0,0,0,0,

        0,
        BEHAVIOR_NORMAL
    );

    /*
       FLOWERS ARE BOTTOM LAYER.

       They cannot draw over the player.
    */

    setMeta(
        META_FLOWER,

        TILE_GRASS,TILE_GRASS,
        TILE_FLOWER,TILE_GRASS,

        0,0,0,0,

        0,
        BEHAVIOR_NORMAL
    );

    setMeta(
        META_PATH,

        TILE_PATH,TILE_PATH,
        TILE_PATH,TILE_PATH,

        0,0,0,0,

        0,
        BEHAVIOR_NORMAL
    );

    /*
       32px vertical path with grass edges.
    */

    setMeta(
        META_PATH_V,

        TILE_PATH_EDGE_L,
        TILE_PATH_EDGE_R,

        TILE_PATH_EDGE_L,
        TILE_PATH_EDGE_R,

        0,0,0,0,

        0,
        BEHAVIOR_NORMAL
    );

    setMeta(
        META_PATH_TOP,

        TILE_PATH_EDGE_T,
        TILE_PATH_EDGE_T,

        TILE_PATH,
        TILE_PATH,

        0,0,0,0,

        0,
        BEHAVIOR_NORMAL
    );

    setMeta(
        META_PATH_BOTTOM,

        TILE_PATH,
        TILE_PATH,

        TILE_PATH_EDGE_B,
        TILE_PATH_EDGE_B,

        0,0,0,0,

        0,
        BEHAVIOR_NORMAL
    );

    /*
       ROOF
    */

    setMeta(
        META_ROOF_L,

        TILE_ROOF_TL,
        TILE_ROOF_TM,

        TILE_ROOF_ML,
        TILE_ROOF_MM,

        0,0,0,0,

        1,
        BEHAVIOR_NORMAL
    );

    setMeta(
        META_ROOF_M,

        TILE_ROOF_TM,
        TILE_ROOF_TM,

        TILE_ROOF_MM,
        TILE_ROOF_MM,

        0,0,0,0,

        1,
        BEHAVIOR_NORMAL
    );

    setMeta(
        META_ROOF_R,

        TILE_ROOF_TM,
        TILE_ROOF_TR,

        TILE_ROOF_MM,
        TILE_ROOF_MR,

        0,0,0,0,

        1,
        BEHAVIOR_NORMAL
    );

    /*
       WALL
    */

    setMeta(
        META_WALL,

        TILE_WALL,TILE_WALL,
        TILE_WALL_BASE,TILE_WALL_BASE,

        0,0,0,0,

        1,
        BEHAVIOR_NORMAL
    );

    /*
       Window is 32 px high.
    */

    setMeta(
        META_WINDOW_TOP,

        TILE_WALL,TILE_WALL,
        TILE_WINDOW_T,TILE_WINDOW_T,

        0,0,0,0,

        1,
        BEHAVIOR_NORMAL
    );

    setMeta(
        META_WINDOW_BOTTOM,

        TILE_WINDOW_B,TILE_WINDOW_B,
        TILE_WALL_BASE,TILE_WALL_BASE,

        0,0,0,0,

        1,
        BEHAVIOR_NORMAL
    );

    /*
       Door top:
       solid because it is visually wall.
    */

    setMeta(
        META_DOOR_TOP,

        TILE_DOOR_T,TILE_DOOR_T,
        TILE_DOOR_T,TILE_DOOR_T,

        0,0,0,0,

        1,
        BEHAVIOR_NORMAL
    );

    /*
       Door bottom:
       walkable entrance.

       Still BG0, so player draws over it.
    */

    setMeta(
        META_DOOR_BOTTOM,

        TILE_DOOR_B,TILE_DOOR_B,
        TILE_DOOR_B,TILE_DOOR_B,

        0,0,0,0,

        0,
        BEHAVIOR_DOOR
    );

    /*
       ONE 32x32 TREE.

       Four different metatiles.
       No duplication.
    */

    setMeta(
        META_TREE_TL,

        TILE_GRASS,TILE_GRASS,
        TILE_GRASS,TILE_GRASS,

        TILE_TREE_00,
        TILE_TREE_01,
        TILE_TREE_10,
        TILE_TREE_11,

        1,
        BEHAVIOR_NORMAL
    );

    setMeta(
        META_TREE_TR,

        TILE_GRASS,TILE_GRASS,
        TILE_GRASS,TILE_GRASS,

        TILE_TREE_02,
        TILE_TREE_03,
        TILE_TREE_12,
        TILE_TREE_13,

        1,
        BEHAVIOR_NORMAL
    );

    setMeta(
        META_TREE_BL,

        TILE_GRASS,TILE_GRASS,
        TILE_GRASS,TILE_GRASS,

        TILE_TREE_20,
        TILE_TREE_21,
        TILE_TREE_30,
        TILE_TREE_31,

        1,
        BEHAVIOR_NORMAL
    );

    setMeta(
        META_TREE_BR,

        TILE_GRASS,TILE_GRASS,
        TILE_GRASS,TILE_GRASS,

        TILE_TREE_22,
        TILE_TREE_23,
        TILE_TREE_32,
        TILE_TREE_33,

        1,
        BEHAVIOR_NORMAL
    );

    /*
       Bush is BG0.
    */

    setMeta(
        META_BUSH,

        TILE_GRASS,TILE_GRASS,
        TILE_BUSH,TILE_BUSH,

        0,0,0,0,

        1,
        BEHAVIOR_NORMAL
    );

    setMeta(
        META_WATER,

        TILE_WATER,TILE_WATER,
        TILE_WATER,TILE_WATER,

        0,0,0,0,

        1,
        BEHAVIOR_WATER
    );
}
