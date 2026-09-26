#include <gba.h>
#include "character_select.h"

#define W 240
#define H 160
#define C_BG RGB5(5,10,18)
#define C_PANEL RGB5(10,16,24)
#define C_WHITE RGB5(31,31,31)
#define C_GREY RGB5(17,19,22)
#define C_GOLD RGB5(31,23,7)
#define C_SKIN RGB5(27,18,13)
#define C_BLACK RGB5(3,3,4)
#define C_BROWN RGB5(12,6,3)
#define C_BLONDE RGB5(28,22,10)
#define C_REDHAIR RGB5(21,7,3)
#define C_BLUE RGB5(5,14,29)
#define C_RED RGB5(26,5,5)
#define C_GREEN RGB5(6,22,9)
#define C_PURPLE RGB5(18,7,25)
#define C_DARK RGB5(6,7,10)

CharacterConfig characterConfig = {0,1,0,0,0};
static volatile u16 *fb=(volatile u16 *)MODE3_FB;

static void px(int x,int y,u16 c){ if(x>=0&&y>=0&&x<W&&y<H) fb[y*W+x]=c; }
static void box(int x,int y,int w,int h,u16 c){int xx,yy;for(yy=0;yy<h;yy++)for(xx=0;xx<w;xx++)px(x+xx,y+yy,c);}

static const unsigned char font[][7]={
{14,17,17,31,17,17,17},{30,17,17,30,17,17,30},{14,17,16,16,16,17,14},{30,17,17,17,17,17,30},
{31,16,16,30,16,16,31},{31,16,16,30,16,16,16},{14,17,16,23,17,17,14},{17,17,17,31,17,17,17},
{14,4,4,4,4,4,14},{7,2,2,2,18,18,12},{17,18,20,24,20,18,17},{16,16,16,16,16,16,31},
{17,27,21,21,17,17,17},{17,25,21,19,17,17,17},{14,17,17,17,17,17,14},{30,17,17,30,16,16,16},
{14,17,17,17,21,18,13},{30,17,17,30,20,18,17},{15,16,16,14,1,1,30},{31,4,4,4,4,4,4},
{17,17,17,17,17,17,14},{17,17,17,17,17,10,4},{17,17,17,21,21,21,10},{17,17,10,4,10,17,17},
{17,17,10,4,4,4,4},{31,1,2,4,8,16,31}};
static void chr(int x,int y,char c,u16 color){int r,col,n;if(c<'A'||c>'Z')return;n=c-'A';for(r=0;r<7;r++)for(col=0;col<5;col++)if(font[n][r]&(1<<(4-col)))px(x+col,y+r,color);}
static void txt(int x,int y,const char*s,u16 c){while(*s){if(*s!=' ')chr(x,y,*s,c);x+=6;s++;}}

static u16 hair(void){switch(characterConfig.hairColor){case 0:return C_BLACK;case 1:return C_BROWN;case 2:return C_BLONDE;default:return C_REDHAIR;}}
static u16 clothesColor(void){switch(characterConfig.clothes){case 0:return C_BLUE;case 1:return C_RED;case 2:return C_GREEN;default:return C_PURPLE;}}
static const char* genderName(void){return characterConfig.gender?"WOMAN":"MAN";}
static const char* hairColorName(void){switch(characterConfig.hairColor){case 0:return"BLACK";case 1:return"BROWN";case 2:return"BLOND";default:return"RED";}}
static const char* hairLengthName(void){return characterConfig.hairLength?"LONG":"SHORT";}
static const char* clothesName(void){switch(characterConfig.clothes){case 0:return"TSHIRT";case 1:return"HOODIE";case 2:return"JACKET";default:return"SWEATER";}}
static const char* bottomName(void){switch(characterConfig.bottomStyle){case 0:return"PANTS";case 1:return"SHORTS";case 2:return"SKIRT";default:return"DRESS";}}

static void preview(void){
 int x=185,y=43; u16 hc=hair(),shirt=clothesColor();
 box(x-12,y+70,24,3,C_DARK);
 /* lower clothing */
 if(characterConfig.bottomStyle==0){box(x-7,y+48,6,18,C_DARK);box(x+1,y+48,6,18,C_DARK);}
 else if(characterConfig.bottomStyle==1){box(x-7,y+48,6,8,C_DARK);box(x+1,y+48,6,8,C_DARK);box(x-6,y+56,5,10,C_SKIN);box(x+1,y+56,5,10,C_SKIN);}
 else if(characterConfig.bottomStyle==2){box(x-10,y+47,20,11,shirt);box(x-7,y+58,5,8,C_SKIN);box(x+2,y+58,5,8,C_SKIN);}
 else {box(x-10,y+42,20,17,shirt);box(x-7,y+59,5,7,C_SKIN);box(x+2,y+59,5,7,C_SKIN);}
 box(x-9,y+64,8,5,C_BLACK);box(x+1,y+64,8,5,C_BLACK);

 if(!characterConfig.gender){box(x-12,y+27,24,21,shirt);box(x-15,y+30,4,15,C_SKIN);box(x+11,y+30,4,15,C_SKIN);}
 else {box(x-9,y+27,18,8,shirt);box(x-7,y+35,14,13,shirt);box(x-12,y+30,4,14,C_SKIN);box(x+8,y+30,4,14,C_SKIN);}
 if(characterConfig.bottomStyle==3){box(x-8,y+40,16,10,shirt);}
 if(characterConfig.clothes==1)box(x-11,y+25,22,5,shirt);
 else if(characterConfig.clothes==2){box(x-2,y+27,4,20,C_DARK);box(x-8,y+30,2,12,C_WHITE);box(x+6,y+30,2,12,C_WHITE);}
 else if(characterConfig.clothes==3){box(x-11,y+29,3,15,shirt);box(x+8,y+29,3,15,shirt);}
 else {box(x-13,y+28,5,7,shirt);box(x+8,y+28,5,7,shirt);}

 box(x-3,y+23,6,6,C_SKIN);
 box(x-9,y+7,18,17,C_SKIN);box(x-7,y+5,14,21,C_SKIN);box(x-11,y+12,3,8,C_SKIN);box(x+8,y+12,3,8,C_SKIN);
 box(x-9,y+3,18,7,hc);box(x-11,y+6,5,10,hc);box(x+6,y+6,5,8,hc);box(x-6,y+1,12,3,hc);
 if(characterConfig.hairLength){box(x-11,y+12,4,17,hc);box(x+7,y+12,4,17,hc);}
 box(x-5,y+15,2,2,C_BLACK);box(x+3,y+15,2,2,C_BLACK);
}

static void arrow(int x,int y){px(x,y,C_GOLD);box(x+1,y-1,2,3,C_GOLD);box(x+3,y-2,2,5,C_GOLD);}
static void drawMenu(int selected){
 box(0,0,W,H,C_BG);box(8,7,224,18,C_PANEL);txt(68,12,"CREATE CHARACTER",C_WHITE);
 box(10,32,143,105,C_PANEL);box(158,32,72,105,C_PANEL);
 txt(25,40,"GENDER",C_WHITE);txt(91,40,genderName(),C_GOLD);
 txt(25,58,"HAIR",C_WHITE);txt(91,58,hairColorName(),C_GOLD);
 txt(25,76,"LENGTH",C_WHITE);txt(91,76,hairLengthName(),C_GOLD);
 txt(25,94,"TOP",C_WHITE);txt(91,94,clothesName(),C_GOLD);
 txt(25,112,"BOTTOM",C_WHITE);txt(91,112,bottomName(),C_GOLD);
 arrow(16,43+selected*18);preview();txt(15,145,"DPAD CHANGE",C_GREY);txt(169,145,"START",C_GOLD);
}
static void redrawHidden(int selected){REG_DISPCNT|=0x0080;drawMenu(selected);VBlankIntrWait();REG_DISPCNT&=~0x0080;}

void characterSelectRun(void){
 int selected=0; SetMode(MODE_3|BG2_ON); redrawHidden(selected);
 while(1){u16 down;VBlankIntrWait();scanKeys();down=keysDown();
  if(down&KEY_UP){if(--selected<0)selected=4;redrawHidden(selected);}
  else if(down&KEY_DOWN){if(++selected>4)selected=0;redrawHidden(selected);}
  else if(down&(KEY_LEFT|KEY_RIGHT)){
   int d=(down&KEY_RIGHT)?1:-1;
   if(selected==0)characterConfig.gender^=1;
   else if(selected==1){characterConfig.hairColor=(characterConfig.hairColor+d+4)%4;}
   else if(selected==2)characterConfig.hairLength^=1;
   else if(selected==3){characterConfig.clothes=(characterConfig.clothes+d+4)%4;}
   else characterConfig.bottomStyle=(characterConfig.bottomStyle+d+4)%4;
   redrawHidden(selected);
  }
  if(down&KEY_START)break;
 }
 do{VBlankIntrWait();scanKeys();}while(keysHeld()&KEY_START);
 REG_DISPCNT|=0x0080;
}
