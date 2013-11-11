#include <stdio.h>
#include <stdlib.h>


#include "vision.h"
#include "Gweled.h"


#include "../../ImageOperations/imageOps.h"
#include "../../Codecs/codecs.h"

#define DUMP_PATCHES 0
#define NO_PATCH_COMPARISON 0


struct PatternItem
{
   unsigned int totalTiles;
   char * name[128];
   struct Image * tile[256];
   unsigned int  value;
   unsigned char use;
   unsigned int acceptScore;
};

struct PatternSet
{
    unsigned int totalPatterns;
    struct PatternItem pattern[256];
};


struct PatternSet set;


int addToPatternSet(struct PatternSet * set , char * name , unsigned int tiles , unsigned int value , unsigned int score)
{
  unsigned int curSetNum = set->totalPatterns;
  ++set->totalPatterns;

  strncpy(set->pattern[curSetNum].name,name,126);
  set->pattern[curSetNum].totalTiles=tiles;
  set->pattern[curSetNum].value=value;
  set->pattern[curSetNum].acceptScore=score;
  set->pattern[curSetNum].use=1;


  char * fName[512];
  int i=0;
  for (i=0; i<tiles; i++)
  {
      sprintf(fName,"%s%u.pnm",name,i+1);
      set->pattern[curSetNum].tile[i] = readImage( fName , PNM_CODEC , 0 );
  }
}

int emptyPatternSet(struct PatternSet * set)
{
    fprintf(stderr,"TODO free all images here\n");
    return 0;
}


unsigned int seeFunctionCalls=0;


int compareTableTile(struct PatternSet * set ,
                     unsigned char * screen , unsigned int screenWidth ,unsigned int screenHeight ,
                     unsigned int sX,unsigned int sY , unsigned int width ,unsigned int height , unsigned int * pick)
{
   #if NO_PATCH_COMPARISON
     return 0; //Disabled until it is fixed
   #endif // NO_PATCH_COMPARISON


   unsigned int bestScore=100000000;
   unsigned int bestPick=NO_PIECE;

   unsigned int currentScore=0;


   if ( colorVariance( screen, screenWidth ,screenHeight , sX,  sY, width , height) < 10150 )
   {
       *pick=NO_PIECE;
       return 1;
   }

  // return 0; //Disabled until it is fixed

 int tileNum=0;
 int patternNum=0;
 for ( patternNum=0;    patternNum<set->totalPatterns;    patternNum++ )
 {
   fprintf(stderr,"Checking for %s\n",set->pattern[patternNum].name);
   for ( tileNum=0;      tileNum < set->pattern[patternNum].totalTiles;     tileNum++ )
   {
       compareRGBPatchesIgnoreColor( screen , sX ,  sY , screenWidth, screenHeight ,
                                     set->pattern[patternNum].tile[tileNum] , 0,  0 ,
                                     set->pattern[patternNum].tile[tileNum]->width ,
                                     set->pattern[patternNum].tile[tileNum]->height,
                                     123,123,0,
                                     width, height , &currentScore );

       if (currentScore<bestScore)
       {
         bestScore = currentScore;
         bestPick = set->pattern[patternNum].value;
       }
   }
   if (bestScore < set->pattern[patternNum].acceptScore )
    {
      fprintf(stderr,"Selected %s with a score of %u \n",set->pattern[patternNum].name,bestScore);
      *pick=bestPick;
      return 1;
    }

 }

 return 0;
}



       /* GWELED Single Pixel values
       if ( closeToRGB(R,G,B, 0, 166 , 0 ,threshold)     )      { table[x][y]=GREEN_PIECE; }  else
       if ( closeToRGB(R,G,B, 200, 200 , 200 ,threshold) )      { table[x][y]=WHITE_PIECE; }  else
       if ( closeToRGB(R,G,B, 175, 63, 5 ,threshold)     )      { table[x][y]=ORANGE_PIECE; } else
       if ( closeToRGB(R,G,B, 145, 145, 3 ,threshold)    )      { table[x][y]=YELLOW_PIECE; } else
       if ( closeToRGB(R,G,B, 0, 189, 249 ,threshold)    )      { table[x][y]=BLUE_PIECE; } else
       if ( closeToRGB(R,G,B, 213, 95, 115 ,threshold)   )      { table[x][y]=RED_PIECE; } else
       if ( closeToRGB(R,G,B, 175, 0, 174 ,threshold)    )      { table[x][y]=PINK_PIECE; }
      */

int seeTable(unsigned int table[8][8] ,
             unsigned char * screen , unsigned int screenWidth ,unsigned int screenHeight ,
             unsigned int clientStartX , unsigned int clientStartY)
{
    ++seeFunctionCalls;

    unsigned int halfBlockX = (unsigned int) settings.blockX/2;
    unsigned int halfBlockY = (unsigned int) settings.blockY/2;

    unsigned char R,G,B;
    unsigned int x,y;
    for (y=0; y<8; y++)
    {
     for (x=0; x<8; x++)
     {
        #if DUMP_PATCHES
        char nameUsed[512]={0};
        sprintf(nameUsed,"Dump/tile%u_%u_%u",seeFunctionCalls,x,y);
        bitBltRGBToFile(  nameUsed ,
                          screen ,
                          settings.clientX + x*settings.blockX,
                          settings.clientY + y*settings.blockY,
                          screenWidth , screenHeight ,
                          settings.blockX,settings.blockY);
        #endif // DUMP_PATCHES


      //Originally unknown piece
      table[x][y]=UNKNOWN_PIECE;

      if
        (
          compareTableTile(&set,
                           screen,screenWidth,screenHeight,
                           settings.clientX + x*settings.blockX,
                           settings.clientY + y*settings.blockY,
                           settings.blockX ,
                           settings.blockY , &table[x][y] )
        )
        { fprintf(stderr,"Table[%u][%u] got assigned via tiles\n",x,y); }
           else
        {
         getRGBPixel(screen,screenWidth,screenHeight,
                     settings.clientX + settings.offsetX + x*settings.blockX + halfBlockX ,
                     settings.clientY + settings.offsetY + y*settings.blockY + halfBlockY ,
                     &R , &G , &B );
         fprintf(stderr,"Pos(%u,%u) = %u , %u , %u \n",x,y,R,G,B);
         if ( closeToRGB(R,G,B, 24, 176 , 44 , settings.threshold)   )      { table[x][y]=GREEN_PIECE; }  else
         if ( closeToRGB(R,G,B, 238, 238 , 238 ,settings.threshold) )       { table[x][y]=WHITE_PIECE; }  else
         if ( closeToRGB(R,G,B, 216, 79, 24 ,settings.threshold)     )      { table[x][y]=ORANGE_PIECE; } else
         if ( closeToRGB(R,G,B, 254, 254, 39 ,settings.threshold)    )      { table[x][y]=YELLOW_PIECE; } else
         if ( closeToRGB(R,G,B, 13, 139, 254 ,settings.threshold)    )      { table[x][y]=BLUE_PIECE; } else
         if ( closeToRGB(R,G,B, 254, 32, 63 ,settings.threshold)     )      { table[x][y]=RED_PIECE; } else
         if ( closeToRGB(R,G,B, 254, 17, 254,settings.threshold)    )       { table[x][y]=PINK_PIECE; }
        }

     }
    }
}


int initVision()
{

  set.totalPatterns=0;
  //addToPatternSet(&set,"Engines/Gweled/Pieces/neutral",4,NO_PIECE,5000);
  addToPatternSet(&set,"Engines/Gweled/Pieces/hypercube",15,HYPERCUBE_PIECE,15000);
  addToPatternSet(&set,"Engines/Gweled/Pieces/yellow",6,YELLOW_PIECE,23000);
  addToPatternSet(&set,"Engines/Gweled/Pieces/orange",5,ORANGE_PIECE,23000);
  addToPatternSet(&set,"Engines/Gweled/Pieces/red",5,RED_PIECE,23000);
  addToPatternSet(&set,"Engines/Gweled/Pieces/blue",5,BLUE_PIECE,23000);
  addToPatternSet(&set,"Engines/Gweled/Pieces/green",4,GREEN_PIECE,23000);
  addToPatternSet(&set,"Engines/Gweled/Pieces/pink",4,PINK_PIECE,23000);
  addToPatternSet(&set,"Engines/Gweled/Pieces/white",3,WHITE_PIECE,23000);

  return 1;
}


int stopVision()
{
  emptyPatternSet(&set);
}
