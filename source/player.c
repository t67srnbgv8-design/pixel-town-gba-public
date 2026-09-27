#include <gba.h>
#include "player.h"
#include "world.h"
#include "character_select.h"

Player player;
/* Eight consecutive 4-bpp tiles: two tiles per row in OBJ_1D_MAP. */
static u16 playerTiles[128] __attribute__((aligned(4)));
static int drawnDirection=-1,drawnFrame=-1;
enum {DIR_DOWN=0,DIR_UP,DIR_LEFT,DIR_RIGHT};
#define OBJ_Y_MASK 0x00FF
#define OBJ_X_MASK 0x01FF
#define OBJ_HIDE 0x0200
#define OBJ_SHAPE_TALL 0x8000
#define OBJ_SIZE_16X32 0x8000
#define OBJ_PRIORITY_1 0x0400

static void spritePixel(u16*gfx,int x,int y,int color){
 int tileX,tileY,tile,localX,localY,pixelInTile,word,shift;
 if(x<0||x>=16||y<0||y>=32)return;
 tileX=x>>3;tileY=y>>3;tile=tileY*2+tileX;localX=x&7;localY=y&7;
 pixelInTile=localY*8+localX;word=tile*16+(pixelInTile>>2);shift=(pixelInTile&3)*4;
 gfx[word]=(gfx[word]&~(0xF<<shift))|((color&15)<<shift);
}
static void spriteRect(u16*g,int x1,int y1,int x2,int y2,int c){int x,y;for(y=y1;y<=y2;y++)for(x=x1;x<=x2;x++)spritePixel(g,x,y,c);}
static void clearSprite(u16*g){int i;for(i=0;i<128;i++)g[i]=0;}
static int hairColorIndex(void){switch(characterConfig.hairColor){case 0:return 2;case 1:return 3;case 2:return 4;default:return 5;}}
static int clothesColorIndex(void){switch(characterConfig.clothes){case 0:return 6;case 1:return 7;case 2:return 8;default:return 9;}}

static void makePlayerFrame(u16*g,int direction,int frame){
 int hair=hairColorIndex(),top=clothesColorIndex();
 int leftStep=frame ? -1 : 0, rightStep=frame ? 1 : 0;
 int left=direction==DIR_LEFT, right=direction==DIR_RIGHT;
 int back=direction==DIR_UP;
 clearSprite(g);

 /* Boots and separate walking legs. The outline makes the figure readable
    against both the pale road and the dark grass. */
 spriteRect(g,4+leftStep,27,7+leftStep,30,1);
 spriteRect(g,8+rightStep,27,11+rightStep,30,1);
 spriteRect(g,5+leftStep,27,7+leftStep,28,10);
 spriteRect(g,8+rightStep,27,10+rightStep,28,10);
 spritePixel(g,5+leftStep,29,12);
 spritePixel(g,8+rightStep,29,12);

 if(characterConfig.bottomStyle==0){ /* trousers */
  spriteRect(g,4,20,11,23,1);
  spriteRect(g,5+leftStep,23,7+leftStep,27,10);
  spriteRect(g,8+rightStep,23,10+rightStep,27,10);
  spriteRect(g,5,22,6,24,13);
  spritePixel(g,9,24,13);
 } else if(characterConfig.bottomStyle==1){ /* shorts */
  spriteRect(g,4,20,11,24,1);
  spriteRect(g,5,21,7,23,10);spriteRect(g,8,21,10,23,10);
  spriteRect(g,5+leftStep,24,7+leftStep,27,11);
  spriteRect(g,8+rightStep,24,10+rightStep,27,11);
 } else { /* skirt and dress retain separate visible legs */
  spriteRect(g,5+leftStep,25,7+leftStep,27,11);
  spriteRect(g,8+rightStep,25,10+rightStep,27,11);
  spriteRect(g,4,21,11,22,1);
  spriteRect(g,3,23,12,25,1);
  spriteRect(g,4,22,11,24,top);
  spriteRect(g,5,24,10,25,top);
  spritePixel(g,5,23,12);
 }

 /* Fitted torso, sleeves, hands and a narrow neck. */
 spriteRect(g,5,13,10,21,1);
 spriteRect(g,4,15,11,20,1);
 spriteRect(g,3,16,4,21,1);spriteRect(g,11,16,12,21,1);
 spriteRect(g,5,14,10,20,top);
 spriteRect(g,4,16,11,19,top);
 if(characterConfig.gender){spritePixel(g,4,20,top);spritePixel(g,11,20,top);}
 spriteRect(g,3,17,3,20,top);spriteRect(g,12,17,12,20,top);
 spritePixel(g,3,21,11);spritePixel(g,12,21,11);
 spriteRect(g,7,12,8,14,11);
 if(characterConfig.bottomStyle==3){
  spriteRect(g,5,20,10,23,top);
  spriteRect(g,4,21,11,23,top);
 }
 /* Distinct top styles, without covering the outfit silhouette. */
 if(characterConfig.clothes==0){
  spritePixel(g,5,15,12);spritePixel(g,10,15,12);
  spriteRect(g,6,14,9,14,12);
 } else if(characterConfig.clothes==1){
  spriteRect(g,5,13,10,15,top);
  spritePixel(g,6,16,12);spritePixel(g,9,16,12);
  spriteRect(g,6,19,9,19,13);
 } else if(characterConfig.clothes==2){
  spriteRect(g,7,15,8,20,1);
  spritePixel(g,6,16,12);spritePixel(g,9,16,12);
  spritePixel(g,7,18,12);
 } else {
  spriteRect(g,5,19,10,20,13);
  spritePixel(g,5,16,12);spritePixel(g,10,16,12);
 }

 /* Ears, face and rounded hair silhouette. */
 spriteRect(g,5,3,10,13,1);
 spriteRect(g,4,5,11,11,1);
 spriteRect(g,5,5,10,12,11);
 spritePixel(g,4,8,11);spritePixel(g,11,8,11);
 spriteRect(g,6,11,9,12,14);
 spriteRect(g,5,2,10,4,hair);
 spriteRect(g,4,4,11,5,hair);
 spriteRect(g,4,5,5,7,hair);
 spriteRect(g,10,5,11,7,hair);
 spritePixel(g,6,2,12);spritePixel(g,7,2,12);
 if(characterConfig.hairLength){
  spriteRect(g,3,6,4,14,1);spriteRect(g,11,6,12,14,1);
  spriteRect(g,4,7,4,13,hair);spriteRect(g,11,7,11,13,hair);
  spritePixel(g,3,13,hair);spritePixel(g,12,13,hair);
 }
 if(back){
  spriteRect(g,5,5,10,11,hair);
  spriteRect(g,6,12,9,13,hair);
  spritePixel(g,6,4,13);
 } else if(left||right){
  int eye=left?5:10;
  spritePixel(g,eye,8,1);
  spritePixel(g,left?4:11,10,14);
  spritePixel(g,left?6:9,11,13);
  spriteRect(g,left?4:10,6,left?5:11,7,hair);
 } else {
  spritePixel(g,6,8,1);spritePixel(g,9,8,1);
  spritePixel(g,6,9,12);spritePixel(g,9,9,12);
  spritePixel(g,7,11,13);spritePixel(g,8,11,13);
  spritePixel(g,5,9,14);spritePixel(g,10,9,14);
 }
}
static int canMoveTo(int x,int y){return !worldIsBlocked(x+4,y+26)&&!worldIsBlocked(x+11,y+26)&&!worldIsBlocked(x+4,y+29)&&!worldIsBlocked(x+11,y+29);}
void playerInit(void){
 int i;player.x=244;player.y=92;player.direction=DIR_DOWN;player.frame=0;player.animationTimer=0;
 SPRITE_PALETTE[0]=RGB5(0,0,0);SPRITE_PALETTE[1]=RGB5(3,3,4);SPRITE_PALETTE[2]=RGB5(2,2,3);SPRITE_PALETTE[3]=RGB5(12,6,3);
 SPRITE_PALETTE[4]=RGB5(28,22,10);SPRITE_PALETTE[5]=RGB5(21,7,3);SPRITE_PALETTE[6]=RGB5(5,14,29);SPRITE_PALETTE[7]=RGB5(26,5,5);
 SPRITE_PALETTE[8]=RGB5(6,23,9);SPRITE_PALETTE[9]=RGB5(18,7,25);SPRITE_PALETTE[10]=RGB5(8,9,12);SPRITE_PALETTE[11]=RGB5(27,18,13);SPRITE_PALETTE[12]=RGB5(31,31,31);SPRITE_PALETTE[13]=RGB5(15,12,13);SPRITE_PALETTE[14]=RGB5(23,13,9);
 for(i=0;i<128;i++)((u16*)SPRITE_GFX)[i]=0;
 for(i=0;i<128;i++){OAM[i].attr0=OBJ_HIDE;OAM[i].attr1=0;OAM[i].attr2=0;}
 makePlayerFrame(playerTiles,player.direction,player.frame);
 CpuFastSet(playerTiles,SPRITE_GFX,COPY32|64);
 drawnDirection=player.direction;drawnFrame=player.frame;
 REG_DISPCNT&=~0x0080;
}
void playerUpdate(void){
 int newX=player.x,newY=player.y,moving=0;u16 held;scanKeys();held=keysHeld();
 if(held&KEY_UP){newY--;player.direction=DIR_UP;moving=1;}else if(held&KEY_DOWN){newY++;player.direction=DIR_DOWN;moving=1;}
 else if(held&KEY_LEFT){newX--;player.direction=DIR_LEFT;moving=1;}else if(held&KEY_RIGHT){newX++;player.direction=DIR_RIGHT;moving=1;}
 if(moving&&canMoveTo(newX,newY)){player.x=newX;player.y=newY;if(++player.animationTimer>=8){player.animationTimer=0;player.frame^=1;}}
 else {player.animationTimer=0;player.frame=0;}
 if(player.direction!=drawnDirection||player.frame!=drawnFrame){
  makePlayerFrame(playerTiles,player.direction,player.frame);
  CpuFastSet(playerTiles,SPRITE_GFX,COPY32|64);
  drawnDirection=player.direction;drawnFrame=player.frame;
 }
}
void playerDraw(int cameraX,int cameraY){
 int screenX=player.x-cameraX,screenY=player.y-cameraY;
 OAM[0].attr0=(screenY&OBJ_Y_MASK)|OBJ_SHAPE_TALL;
 OAM[0].attr1=(screenX&OBJ_X_MASK)|OBJ_SIZE_16X32;
 OAM[0].attr2=OBJ_PRIORITY_1;
 /* OAM already points to hardware OAM; no self-copy is needed. */
}
