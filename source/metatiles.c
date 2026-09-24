#include "metatiles.h"
#include "tileset.h"

Metatile metatiles[META_COUNT];

static void setMeta(
    int id,

    int b0,
    int b1,
    int b2,
    int b3,

    int t0,
    int t1,
    int t2,
    int t3,

    int collision,
    int behavior
)
{
    Metatile *m =
        &metatiles[id];

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
    m->elevation = 0;
}

void metatilesInit(void)
{
    /*
       GROUND

       IMPORTANT:
       TOP is always EMPTY.
    */

    setMeta(
        META_GRASS,

        TILE_GRASS,
        TILE_GRASS,
        TILE_GRASS,
        TILE_GRASS,

        0,0,0,0,

        0,
        BEHAVIOR_NORMAL
    );

    setMeta(
        META_GRASS_DETAIL,

        TILE_GRASS,
        TILE_GRASS_DARK,
        TILE_GRASS,
        TILE_GRASS,

        0,0,0,0,

        0,
        BEHAVIOR_NORMAL
    );

    setMeta(
        META_FLOWER,

        TILE_GRASS,
        TILE_GRASS,
        TILE_GRASS_FLOWER,
        TILE_GRASS,

        0,0,0,0,

        0,
        BEHAVIOR_NORMAL
    );

    /*
       PATH
    */

    setMeta(
        META_PATH,

        TILE_PATH,
        TILE_PATH_DARK,
        TILE_PATH,
        TILE_PATH,

        0,0,0,0,

        0,
        BEHAVIOR_NORMAL
    );

    setMeta(
        META_PATH_EDGE,

        TILE_PATH_EDGE,
        TILE_PATH_EDGE,
        TILE_PATH,
        TILE_PATH,

        0,0,0,0,

        0,
        BEHAVIOR_NORMAL
    );

    /*
       ROOF
    */

    setMeta(
        META_ROOF_LEFT,

        TILE_ROOF_CORNER_L,
        TILE_ROOF_RED_LIGHT,
        TILE_ROOF_RED,
        TILE_ROOF_RED,

        0,0,0,0,

        1,
        BEHAVIOR_NORMAL
    );

    setMeta(
        META_ROOF_MIDDLE,

        TILE_ROOF_RED_LIGHT,
        TILE_ROOF_RED_LIGHT,
        TILE_ROOF_RED,
        TILE_ROOF_RED,

        0,0,0,0,

        1,
        BEHAVIOR_NORMAL
    );

    setMeta(
        META_ROOF_RIGHT,

        TILE_ROOF_RED_LIGHT,
        TILE_ROOF_CORNER_R,
        TILE_ROOF_RED,
        TILE_ROOF_RED,

        0,0,0,0,

        1,
        BEHAVIOR_NORMAL
    );

    setMeta(
        META_ROOF_LOW_LEFT,

        TILE_ROOF_RED,
        TILE_ROOF_RED,
        TILE_ROOF_EDGE,
        TILE_ROOF_EDGE,

        0,0,0,0,

        1,
        BEHAVIOR_NORMAL
    );

    setMeta(
        META_ROOF_LOW_MIDDLE,

        TILE_ROOF_RED,
        TILE_ROOF_RED,
        TILE_ROOF_EDGE,
        TILE_ROOF_EDGE,

        0,0,0,0,

        1,
        BEHAVIOR_NORMAL
    );

    setMeta(
        META_ROOF_LOW_RIGHT,

        TILE_ROOF_RED,
        TILE_ROOF_RED,
        TILE_ROOF_EDGE,
        TILE_ROOF_EDGE,

        0,0,0,0,

        1,
        BEHAVIOR_NORMAL
    );

    /*
       WALL
    */

    setMeta(
        META_WALL,

        TILE_WALL,
        TILE_WALL,
        TILE_WALL_SHADOW,
        TILE_WALL_SHADOW,

        0,0,0,0,

        1,
        BEHAVIOR_NORMAL
    );

    setMeta(
        META_WINDOW,

        TILE_WALL,
        TILE_WALL,
        TILE_WINDOW,
        TILE_WINDOW,

        0,0,0,0,

        1,
        BEHAVIOR_NORMAL
    );

    setMeta(
        META_DOOR,

        TILE_WALL,
        TILE_WALL,
        TILE_DOOR,
        TILE_DOOR,

        0,0,0,0,

        0,
        BEHAVIOR_DOOR
    );

    /*
       TREES

       Only these use TOP.

       The bottom contains ordinary
       grass / trunk.

       The canopy goes on BG1.
    */

    setMeta(
        META_TREE_TOP_LEFT,

        TILE_GRASS,
        TILE_GRASS,
        TILE_GRASS,
        TILE_GRASS,

        TILE_TREE_TL,
        TILE_TREE_TR,
        TILE_TREE_ML,
        TILE_TREE_MR,

        1,
        BEHAVIOR_NORMAL
    );

    setMeta(
        META_TREE_TOP_RIGHT,

        TILE_GRASS,
        TILE_GRASS,
        TILE_GRASS,
        TILE_GRASS,

        TILE_TREE_TL,
        TILE_TREE_TR,
        TILE_TREE_ML,
        TILE_TREE_MR,

        1,
        BEHAVIOR_NORMAL
    );

    setMeta(
        META_TREE_LOW_LEFT,

        TILE_GRASS,
        TILE_GRASS,
        TILE_GRASS,
        TILE_TREE_TRUNK,

        TILE_TREE_BL,
        TILE_TREE_BR,
        0,
        0,

        1,
        BEHAVIOR_NORMAL
    );

    setMeta(
        META_TREE_LOW_RIGHT,

        TILE_GRASS,
        TILE_GRASS,
        TILE_TREE_TRUNK,
        TILE_GRASS,

        TILE_TREE_BL,
        TILE_TREE_BR,
        0,
        0,

        1,
        BEHAVIOR_NORMAL
    );

    /*
       Bush stays entirely on BG0.
       It can NEVER cover the player.
    */

    setMeta(
        META_BUSH,

        TILE_GRASS,
        TILE_GRASS,
        TILE_BUSH,
        TILE_BUSH,

        0,0,0,0,

        1,
        BEHAVIOR_NORMAL
    );

    /*
       Fence also stays BG0.
    */

    setMeta(
        META_FENCE,

        TILE_GRASS,
        TILE_GRASS,
        TILE_FENCE,
        TILE_FENCE_POST,

        0,0,0,0,

        1,
        BEHAVIOR_NORMAL
    );

    /*
       WATER
    */

    setMeta(
        META_WATER,

        TILE_WATER,
        TILE_WATER_LIGHT,
        TILE_WATER_LIGHT,
        TILE_WATER,

        0,0,0,0,

        1,
        BEHAVIOR_WATER
    );

    setMeta(
        META_WATER_EDGE,

        TILE_WATER_EDGE,
        TILE_WATER_EDGE,
        TILE_WATER,
        TILE_WATER_LIGHT,

        0,0,0,0,

        1,
        BEHAVIOR_WATER
    );
}
