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

    m->collision =
        collision;

    m->behavior =
        behavior;

    m->elevation = 0;
}

void metatilesInit(void)
{
    /* =====================================================
       GROUND
       ===================================================== */

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
        TILE_GRASS_DETAIL,
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
        TILE_FLOWER,
        TILE_GRASS,

        0,0,0,0,

        0,
        BEHAVIOR_NORMAL
    );

    /* =====================================================
       PATH
       ===================================================== */

    setMeta(
        META_PATH,

        TILE_PATH,
        TILE_PATH,
        TILE_PATH,
        TILE_PATH,

        0,0,0,0,

        0,
        BEHAVIOR_NORMAL
    );

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

    /* =====================================================
       HOUSE ROOF
       ===================================================== */

    setMeta(
        META_ROOF_LEFT,

        TILE_ROOF_SLOPE_L,
        TILE_ROOF_TOP,

        TILE_ROOF_RED_DARK,
        TILE_ROOF_RED,

        0,0,0,0,

        1,
        BEHAVIOR_NORMAL
    );

    setMeta(
        META_ROOF_MIDDLE,

        TILE_ROOF_TOP,
        TILE_ROOF_TOP,

        TILE_ROOF_RED_LIGHT,
        TILE_ROOF_RED,

        0,0,0,0,

        1,
        BEHAVIOR_NORMAL
    );

    setMeta(
        META_ROOF_RIGHT,

        TILE_ROOF_TOP,
        TILE_ROOF_SLOPE_R,

        TILE_ROOF_RED,
        TILE_ROOF_RED_DARK,

        0,0,0,0,

        1,
        BEHAVIOR_NORMAL
    );

    /*
        Projecting roof edge.

        Still solid because the building occupies
        this world position.
    */

    setMeta(
        META_ROOF_EAVE_LEFT,

        TILE_ROOF_EAVE_L,
        TILE_ROOF_EAVE_M,

        TILE_WALL,
        TILE_WALL,

        0,0,0,0,

        1,
        BEHAVIOR_NORMAL
    );

    setMeta(
        META_ROOF_EAVE_MIDDLE,

        TILE_ROOF_EAVE_M,
        TILE_ROOF_EAVE_M,

        TILE_WALL,
        TILE_WALL,

        0,0,0,0,

        1,
        BEHAVIOR_NORMAL
    );

    setMeta(
        META_ROOF_EAVE_RIGHT,

        TILE_ROOF_EAVE_M,
        TILE_ROOF_EAVE_R,

        TILE_WALL,
        TILE_WALL,

        0,0,0,0,

        1,
        BEHAVIOR_NORMAL
    );

    /* =====================================================
       FACADE
       ===================================================== */

    setMeta(
        META_WALL,

        TILE_WALL,
        TILE_WALL,

        TILE_WALL_BASE,
        TILE_WALL_BASE,

        0,0,0,0,

        1,
        BEHAVIOR_NORMAL
    );

    setMeta(
        META_WALL_DETAIL,

        TILE_WALL_DETAIL,
        TILE_WALL_DETAIL,

        TILE_WALL_BASE,
        TILE_WALL_BASE,

        0,0,0,0,

        1,
        BEHAVIOR_NORMAL
    );

    setMeta(
        META_WINDOW,

        TILE_WINDOW_TL,
        TILE_WINDOW_TR,

        TILE_WINDOW_BL,
        TILE_WINDOW_BR,

        0,0,0,0,

        1,
        BEHAVIOR_NORMAL
    );

    /* =====================================================
       DOOR
       ===================================================== */

    setMeta(
        META_DOOR_TOP,

        TILE_DOOR_TL,
        TILE_DOOR_TR,

        TILE_DOOR_BL,
        TILE_DOOR_BR,

        0,0,0,0,

        1,
        BEHAVIOR_NORMAL
    );

    /*
        Bottom entrance tile.

        Grass/path underneath and walkable.
        This becomes the actual entrance position.
    */

    setMeta(
        META_DOOR_BOTTOM,

        TILE_PATH_EDGE_L,
        TILE_PATH_EDGE_R,

        TILE_PATH_EDGE_L,
        TILE_PATH_EDGE_R,

        0,0,0,0,

        0,
        BEHAVIOR_DOOR
    );

    setMeta(
        META_SIGN,

        TILE_SIGN,
        TILE_SIGN,
        TILE_WALL_BASE,
        TILE_WALL_BASE,

        0,0,0,0,

        1,
        BEHAVIOR_NORMAL
    );

    setMeta(
        META_FLOWER_BOX,

        TILE_FLOWER_BOX,
        TILE_FLOWER_BOX,

        TILE_WALL_BASE,
        TILE_WALL_BASE,

        0,0,0,0,

        1,
        BEHAVIOR_NORMAL
    );

    /* =====================================================
       TREE
       ===================================================== */

    setMeta(
        META_TREE_TL,

        TILE_GRASS,
        TILE_GRASS,
        TILE_GRASS,
        TILE_GRASS,

        TILE_TREE_00,
        TILE_TREE_01,
        TILE_TREE_10,
        TILE_TREE_11,

        1,
        BEHAVIOR_NORMAL
    );

    setMeta(
        META_TREE_TR,

        TILE_GRASS,
        TILE_GRASS,
        TILE_GRASS,
        TILE_GRASS,

        TILE_TREE_02,
        TILE_TREE_03,
        TILE_TREE_12,
        TILE_TREE_13,

        1,
        BEHAVIOR_NORMAL
    );

    setMeta(
        META_TREE_BL,

        TILE_GRASS,
        TILE_GRASS,
        TILE_GRASS,
        TILE_GRASS,

        TILE_TREE_20,
        TILE_TREE_21,
        TILE_TREE_30,
        TILE_TREE_31,

        1,
        BEHAVIOR_NORMAL
    );

    setMeta(
        META_TREE_BR,

        TILE_GRASS,
        TILE_GRASS,
        TILE_GRASS,
        TILE_GRASS,

        TILE_TREE_22,
        TILE_TREE_23,
        TILE_TREE_32,
        TILE_TREE_33,

        1,
        BEHAVIOR_NORMAL
    );

    /* =====================================================
       BUSH
       ===================================================== */

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

    /* =====================================================
       WATER
       ===================================================== */

    setMeta(
        META_WATER,

        TILE_WATER,
        TILE_WATER,
        TILE_WATER,
        TILE_WATER,

        0,0,0,0,

        1,
        BEHAVIOR_WATER
    );
}
