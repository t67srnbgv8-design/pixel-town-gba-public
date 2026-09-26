#include <gba.h>
#include "player.h"
#include "world.h"
#include "character_select.h"

Player player;
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
 int hair=hairColorIndex(),clothes=clothesColorIndex();
 clearSprite(g); spriteRect(g,4,29,11,30,1);

 /* lower garment and legs */
 if(characterConfig.bottomStyle==0){
  if(!frame){spriteRect(g,5,22,7,28,10);spriteRect(g,8,22,10,28,10);}
  else {spriteRect(g,4,22,6,28,10);spriteRect(g,9,22,11,28,10);}
 } else if(characterConfig.bottomStyle==1){
  spriteRect(g,4,22,7,24,10);spriteRect(g,8,22,11,24,10);
  if(!frame){spriteRect(g,5,25,7,28,11);spriteRect(g,8,25,10,28,11);}
  else {spriteRect(g,4,25,6,28,11);spriteRect(g,9,25,11,28,11);}
 } else if(characterConfig.bottomStyle==2){
  spriteRect(g,3,21,12,25,clothes);spriteRect(g,4,24,11,26,clothes);
  spriteRect(g,5,26,7,28,11);spriteRect(g,8,26,10,28,11);
 } else {
  spriteRect(g,4,18,11,25,clothes);spriteRect(g,3,22,12,26,clothes);
  spriteRect(g,5,26,7,28,11);spriteRect(g,8,26,10,28,11);
 }
 if(!frame){spriteRect(g,4,27,7,29,1);spriteRect(g,8,27,11,29,1);}
 else {spriteRect(g,3,27,6,29,1);spriteRect(g,9,27,12,29,1);}

 /* strongly different silhouettes */
 if(characterConfig.gender==0){
  spriteRect(g,4,14,11,22,clothes);spriteRect(g,3,16,4,21,11);spriteRect(g,11,16,12,21,11);
 }else{
  spriteRect(g,5,14,10,17,clothes);spriteRect(g,4,18,11,22,clothes);spriteRect(g,3,16,4,21,11);spriteRect(g,11,16,12,21,11);
 }
 if(characterConfig.bottomStyle==3){spriteRect(g,4,18,11,23,clothes);}

 if(characterConfig.clothes==0){spriteRect(g,3,15,5,18,clothes);spriteRect(g,10,15,12,18,clothes);spritePixel(g,6,15,12);}
 else if(characterConfig.clothes==1){spriteRect(g,3,14,12,18,clothes);spriteRect(g,4,13,11,15,clothes);spritePixel(g,7,16,12);spritePixel(g,8,16,12);}
 else if(characterConfig.clothes==2){spriteRect(g,3,15,12,22,clothes);spriteRect(g,7,15,8,22,1);spritePixel(g,5,17,12);spritePixel(g,10,17,12);}
 else {spriteRect(g,3,15,12,22,clothes);spriteRect(g,3,18,4,22,clothes);spriteRect(g,11,18,12,22,clothes);}

 spriteRect(g,7,12,8,15,11);spriteRect(g,5,4,10,13,11);spriteRect(g,4,6,11,12,11);
 spriteRect(g,3,8,4,11,11);spriteRect(g,11,8,12,11,11);
 spriteRect(g,5,2,10,4,hair);spriteRect(g,4,3,11,7,hair);spritePixel(g,3,5,hair);spritePixel(g,12,5,hair);
 spriteRect(g,3,6,5,9,hair);spriteRect(g,10,6,12,8,hair);
 if(characterConfig.hairLength){spriteRect(g,3,8,4,16,hair);spriteRect(g,11,8,12,16,hair);}
 if(direction==DIR_DOWN){spritePixel(g,6,9,1);spritePixel(g,9,9,1);}
 else if(direction==DIR_UP){spriteRect(g,5,6,10,11,hair);spriteRect(g,4,8,11,10,hair);}
 else if(direction==DIR_LEFT)spritePixel(g,5,9,1); else spritePixel(g,10,9,1);
}
static int canMoveTo(int x,int y){return !worldIsBlocked(x+4,y+26)&&!worldIsBlocked(x+11,y+26)&&!worldIsBlocked(x+4,y+29)&&!worldIsBlocked(x+11,y+29);}
void playerInit(void){
 int i;player.x=244;player.y=92;player.direction=DIR_DOWN;player.frame=0;player.animationTimer=0;
 SPRITE_PALETTE[0]=RGB5(0,0,0);SPRITE_PALETTE[1]=RGB5(3,3,4);SPRITE_PALETTE[2]=RGB5(2,2,3);SPRITE_PALETTE[3]=RGB5(12,6,3);
 SPRITE_PALETTE[4]=RGB5(28,22,10);SPRITE_PALETTE[5]=RGB5(21,7,3);SPRITE_PALETTE[6]=RGB5(5,14,29);SPRITE_PALETTE[7]=RGB5(26,5,5);
 SPRITE_PALETTE[8]=RGB5(6,23,9);SPRITE_PALETTE[9]=RGB5(18,7,25);SPRITE_PALETTE[10]=RGB5(8,9,12);SPRITE_PALETTE[11]=RGB5(27,18,13);SPRITE_PALETTE[12]=RGB5(31,31,31);
 for(i=0;i<128;i++)((u16*)SPRITE_GFX)[i]=0;
 for(i=0;i<128;i++){OAM[i].attr0=OBJ_HIDE;OAM[i].attr1=0;OAM[i].attr2=0;}
 makePlayerFrame((u16*)SPRITE_GFX,player.direction,player.frame);REG_DISPCNT&=~0x0080;
}
void playerUpdate(void){
 int newX=player.x,newY=player.y,moving=0;u16 held;scanKeys();held=keysHeld();
 if(held&KEY_UP){newY--;player.direction=DIR_UP;moving=1;}else if(held&KEY_DOWN){newY++;player.direction=DIR_DOWN;moving=1;}
 else if(held&KEY_LEFT){newX--;player.direction=DIR_LEFT;moving=1;}else if(held&KEY_RIGHT){newX++;player.direction=DIR_RIGHT;moving=1;}
 if(moving&&canMoveTo(newX,newY)){player.x=newX;player.y=newY;if(++player.animationTimer>=8){player.animationTimer=0;player.frame^=1;}}
 else {player.animationTimer=0;player.frame=0;}
 makePlayerFrame((u16*)SPRITE_GFX,player.direction,player.frame);
}
void playerDraw(int cameraX,int cameraY){
 int screenX=player.x-cameraX,screenY=player.y-cameraY;
 OAM[0].attr0=(screenY&OBJ_Y_MASK)|OBJ_SHAPE_TALL;
 OAM[0].attr1=(screenX&OBJ_X_MASK)|OBJ_SIZE_16X32;
 OAM[0].attr2=OBJ_PRIORITY_1;
 CpuFastSet(OAM,(void*)0x07000000,COPY32|(sizeof(OAM)/4));
}
