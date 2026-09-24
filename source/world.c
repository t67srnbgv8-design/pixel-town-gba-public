#include <gba.h>
#include "world.h"
#include "tileset.h"

MapTile world[MAP_H][MAP_W];

static void setTile(int x, int y, u16 ground, u16 upper, int collision)
{
    if (x < 0 || y < 0 || x >= MAP_W || y >= MAP_H)
        return;

    world[y][x].ground = ground;
    world[y][x].upper = upper;
    world[y][x].collision = collision;
    world[y][x].elevation = 0;
    world[y][x].behavior = BEHAVIOR_NORMAL;
}

static void makeTree(int x, int y)
{
    /* broad 4x4 crown */
    for (int yy=0; yy<4; yy++)
        for (int xx=0; xx<4; xx++) {
            int tile = TILE_TREE_MID;

            if (yy == 0)
                tile = TILE_TREE_LIGHT;
            else if (yy >= 2)
                tile = TILE_TREE_DARK;

            world[y+yy][x+xx].upper = tile;
        }

    /* irregular silhouette */
    world[y][x].upper = TILE_EMPTY;
    world[y][x+3].upper = TILE_EMPTY;

    /* trunks on ground */
    world[y+3][x+1].ground = TILE_TREE_TRUNK;
    world[y+3][x+2].ground = TILE_TREE_TRUNK;

    world[y+3][x+1].collision = 1;
    world[y+3][x+2].collision = 1;
}

static void makeHouse(int x, int y, int w)
{
    int door = x + w/2 - 1;

    /*
       Large roof:
       visually separated from wall and allowed
       to overlap player through upper layer.
    */
    for (int xx=x-1; xx<=x+w; xx++) {
        if (xx >= 0 && xx < MAP_W) {
            world[y][xx].upper = TILE_ROOF_LIGHT;
            world[y+1][xx].upper = TILE_ROOF;
            world[y+2][xx].upper = TILE_ROOF;
            world[y+3][xx].upper = TILE_ROOF_DARK;
            world[y+4][xx].upper = TILE_ROOF_EDGE;
        }
    }

    /* walls */
    for (int yy=y+5; yy<=y+8; yy++) {
        for (int xx=x; xx<x+w; xx++) {
            world[yy][xx].ground =
                (yy == y+8)
                ? TILE_WALL_SHADOW
                : TILE_WALL;

            world[yy][xx].collision = 1;
        }
    }

    /* windows */
    world[y+6][x+1].ground = TILE_WINDOW;
    world[y+6][x+2].ground = TILE_WINDOW;

    world[y+6][x+w-3].ground = TILE_WINDOW;
    world[y+6][x+w-2].ground = TILE_WINDOW;

    /* double door */
    world[y+7][door].ground = TILE_DOOR;
    world[y+7][door+1].ground = TILE_DOOR;

    world[y+8][door].ground = TILE_DOOR;
    world[y+8][door+1].ground = TILE_DOOR;

    world[y+8][door].behavior = BEHAVIOR_DOOR;
    world[y+8][door+1].behavior = BEHAVIOR_DOOR;

    /* entrance */
    for (int yy=y+9; yy<=y+13 && yy<MAP_H; yy++) {
        world[yy][door].ground = TILE_PATH;
        world[yy][door+1].ground = TILE_PATH_LIGHT;

        world[yy][door].upper = TILE_EMPTY;
        world[yy][door+1].upper = TILE_EMPTY;

        world[yy][door].collision = 0;
        world[yy][door+1].collision = 0;
    }
}

static void makePond(int x, int y, int w, int h)
{
    for (int yy=0; yy<h; yy++) {
        for (int xx=0; xx<w; xx++) {
            int wx = x+xx;
            int wy = y+yy;

            /* rounded corners */
            if ((xx==0 || xx==w-1) &&
                (yy==0 || yy==h-1))
                continue;

            world[wy][wx].ground =
                ((xx + yy) % 4 == 0)
                ? TILE_WATER_LIGHT
                : TILE_WATER;

            world[wy][wx].collision = 1;
            world[wy][wx].behavior = BEHAVIOR_WATER;
        }
    }

    for (int xx=1; xx<w-1; xx++)
        world[y][x+xx].ground = TILE_WATER_EDGE;
}

static void makeGarden(int x, int y, int w, int h, int gate)
{
    for (int yy=y+1; yy<y+h-1; yy++)
        for (int xx=x+1; xx<x+w-1; xx++)
            if ((xx+yy)%2 == 0)
                world[yy][xx].ground = TILE_SOIL;

    for (int xx=x; xx<x+w; xx++) {
        world[y][xx].upper = TILE_FENCE_H;
        world[y][xx].collision = 1;

        if (xx != gate && xx != gate+1) {
            world[y+h-1][xx].upper = TILE_FENCE_H;
            world[y+h-1][xx].collision = 1;
        }
    }

    for (int yy=y; yy<y+h; yy++) {
        world[yy][x].upper = TILE_FENCE_V;
        world[yy][x+w-1].upper = TILE_FENCE_V;

        world[yy][x].collision = 1;
        world[yy][x+w-1].collision = 1;
    }

    world[y][x].upper = TILE_FENCE_POST;
    world[y][x+w-1].upper = TILE_FENCE_POST;

    world[y+h-1][x].upper = TILE_FENCE_POST;
    world[y+h-1][x+w-1].upper = TILE_FENCE_POST;

    world[y+h-1][gate].upper = TILE_EMPTY;
    world[y+h-1][gate+1].upper = TILE_EMPTY;

    world[y+h-1][gate].collision = 0;
    world[y+h-1][gate+1].collision = 0;
}

void worldInit(void)
{
    /* base grass */
    for (int y=0; y<MAP_H; y++) {
        for (int x=0; x<MAP_W; x++) {
            int g = TILE_GRASS;

            int r = (x*17 + y*11) % 53;

            if (r == 4)
                g = TILE_GRASS_DETAIL_A;
            else if (r == 17)
                g = TILE_GRASS_DETAIL_B;

            setTile(x,y,g,TILE_EMPTY,0);

            world[y][x].behavior = BEHAVIOR_GRASS;
        }
    }

    /*
       Main horizontal road.
       Narrower than old version.
    */
    for (int x=0; x<MAP_W; x++) {
        world[14][x].ground = TILE_PATH_EDGE_TOP;
        world[15][x].ground = TILE_PATH;
        world[16][x].ground =
            (x%4==0) ? TILE_PATH_LIGHT : TILE_PATH;
        world[17][x].ground = TILE_PATH;
        world[18][x].ground = TILE_PATH_EDGE_BOTTOM;
    }

    /*
       Central road
    */
    for (int y=0; y<MAP_H; y++) {
        world[y][30].ground = TILE_PATH;
        world[y][31].ground =
            (y%4==0) ? TILE_PATH_LIGHT : TILE_PATH;
        world[y][32].ground = TILE_PATH;
    }

    /*
       Buildings
    */
    makeHouse(3,3,9);
    makeHouse(17,3,10);

    makeHouse(36,3,9);
    makeHouse(50,3,10);

    /*
       Gardens between houses
    */
    makeGarden(13,5,4,7,14);
    makeGarden(46,5,4,7,47);

    /*
       North tree line.
       These are intentionally grouped instead of
       being scattered single-tile objects.
    */
    makeTree(0,0);
    makeTree(12,0);
    makeTree(27,1);
    makeTree(45,0);
    makeTree(60,0);

    /*
       Bushes / flower decoration
    */
    int bushes[][2] = {
        {1,12},{11,12},{25,12},
        {37,12},{44,12},{58,12}
    };

    for (unsigned i=0;
         i<sizeof(bushes)/sizeof(bushes[0]);
         i++) {
        int x=bushe
s[i][0];
        int y=bushes[i][1];

        world[y][x].upper = TILE_BUSH;
        world[y][x].collision = 1;
    }

    /*
       Park
    */
    makePond(28,21,8,8);

    makeTree(14,20);
    makeTree(47,20);

    /*
       Flower beds around pond
    */
    for (int y=21; y<=25; y+=2) {
        world[y][26].upper = TILE_FLOWER;
        world[y][37].upper = TILE_FLOWER;
    }

    /*
       Sign
    */
    world[13][27].upper = TILE_SIGN;
    world[13][27].collision = 1;
    world[13][27].behavior = BEHAVIOR_SIGN;

    /*
       Entrance safety pass
    */
    int doors[] = {
        6,7,
        21,22,
        39,40,
        54,55
    };

    for (int i=0; i<8; i++) {
        for (int y=12; y<=14; y++) {
            int x=doors[i];

            world[y][x].ground = TILE_PATH;
            world[y][x].upper = TILE_EMPTY;
            world[y][x].collision = 0;
        }
    }

    /*
       Intersection always open
    */
    for (int y=13; y<=19; y++) {
        for (int x=29; x<=33; x++) {
            world[y][x].ground = TILE_PATH;
            world[y][x].upper = TILE_EMPTY;
            world[y][x].collision = 0;
        }
    }
}

static void writeMap(int base, int x, int y, u16 tile)
{
    if (x < 32)
        ((u16 *)SCREEN_BASE_BLOCK(base))[y*32+x] = tile;
    else
        ((u16 *)SCREEN_BASE_BLOCK(base+1))[y*32+(x-32)] = tile;
}

void worldDraw(void)
{
    for (int y=0; y<MAP_H; y++) {
        for (int x=0; x<MAP_W; x++) {
            writeMap(28,x,y,world[y][x].ground);
            writeMap(30,x,y,world[y][x].upper);
        }
    }
}

int worldIsBlocked(int pixelX, int pixelY)
{
    int tx = pixelX / 8;
    int ty = pixelY / 8;

    if (tx < 0 || ty < 0 || tx >= MAP_W || ty >= MAP_H)
        return 1;

    return world[ty][tx].collision;
}
