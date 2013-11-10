#include <stdio.h>
#include <stdlib.h>


#include "vision.h"
#include "Gweled.h"


#include "../../ImageOperations/imageOps.h"
#include "../../Codecs/codecs.h"



struct Image * neutral[4];
struct Image * hypercube[4];




int compareTableTile(unsigned char * screen , unsigned int screenWidth ,unsigned int screenHeight ,
                     unsigned int sX,unsigned int sY , unsigned int width ,unsigned int height , unsigned int * pick)
{
   return 0; //Disabled until it is fixed
   unsigned int bestScore=100000000;

   unsigned int currentScore=0;


   if ( colorVariance( screen, screenWidth ,screenHeight , sX,  sY, width , height) < 10000 )
   {
       *pick=EMPTY_PIECE;
       return 1;
   }

/*
   int i=0;
   for (i=0; i<4; i++)
   {
       compareRGBPatches( screen , sX ,  sY , screenWidth, screenHeight ,
                          neutral[i]->pixels , 0,  0 , neutral[i]->width , neutral[i]->height  ,
                          width, height , &currentScore );

       if (currentScore<bestScore)
       {
         bestScore = currentScore;
       }
   }

   if (bestScore < 10000) { *pick=EMPTY_PIECE; return 1; }
   */


   return 0;
}




int seeTable(unsigned int table[8][8] ,
             unsigned char * screen , unsigned int screenWidth ,unsigned int screenHeight ,
             unsigned int clientStartX , unsigned int clientStartY)
{


    unsigned int halfBlockX = (unsigned int) settings.blockX/2;
    unsigned int halfBlockY = (unsigned int) settings.blockY/2;

    unsigned int R,G,B;
    unsigned int x,y;
    for (y=0; y<8; y++)
    {
     for (x=0; x<8; x++)
     {
       /*
       if ( closeToRGB(R,G,B, 0, 166 , 0 ,threshold)     )      { table[x][y]=GREEN_PIECE; }  else
       if ( closeToRGB(R,G,B, 200, 200 , 200 ,threshold) )      { table[x][y]=WHITE_PIECE; }  else
       if ( closeToRGB(R,G,B, 175, 63, 5 ,threshold)     )      { table[x][y]=ORANGE_PIECE; } else
       if ( closeToRGB(R,G,B, 145, 145, 3 ,threshold)    )      { table[x][y]=YELLOW_PIECE; } else
       if ( closeToRGB(R,G,B, 0, 189, 249 ,threshold)    )      { table[x][y]=BLUE_PIECE; } else
       if ( closeToRGB(R,G,B, 213, 95, 115 ,threshold)   )      { table[x][y]=RED_PIECE; } else
       if ( closeToRGB(R,G,B, 175, 0, 174 ,threshold)    )      { table[x][y]=PINK_PIECE; }
      */
      if
        (
          compareTableTile(screen,screenWidth,screenHeight,
                           settings.clientX + settings.offsetX + x*settings.blockX,
                           settings.clientY + settings.offsetY + y*settings.blockY,
                           settings.blockX ,
                           settings.blockY , &table[x][y] )
        )
        { fprintf(stderr,"Table[%u][%u] got auto assigned\n",x,y); }
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
  neutral[0] = readImage("Engines/Gweled/Pieces/neutral1.pnm",PNM_CODEC,0);
 // writeImageFile(neutral[0],PNM_CODEC,"test0.pnm");

  neutral[1] = readImage("Engines/Gweled/Pieces/neutral2.pnm",PNM_CODEC,0);
//  writeImageFile(neutral[1],PNM_CODEC,"test1.pnm");

  neutral[2] = readImage("Engines/Gweled/Pieces/neutral3.pnm",PNM_CODEC,0);
//  writeImageFile(neutral[2],PNM_CODEC,"test2.pnm");

  neutral[3] = readImage("Engines/Gweled/Pieces/neutral4.pnm",PNM_CODEC,0);
//  writeImageFile(neutral[3],PNM_CODEC,"test3.pnm");



  hypercube[0] = readImage("Engines/Gweled/Pieces/hypercube1.pnm",PNM_CODEC,0);
  hypercube[1] = readImage("Engines/Gweled/Pieces/hypercube2.pnm",PNM_CODEC,0);
  hypercube[2] = readImage("Engines/Gweled/Pieces/hypercube3.pnm",PNM_CODEC,0);


  unsigned int i=0;
  for (i=0; i<4; i++) { if (neutral[i]==0) { fprintf(stderr,"Could not open neutral[%u]\n",i); return 0; } }
  for (i=0; i<3; i++) { if (hypercube[i]==0) { fprintf(stderr,"Could not open hypercube[%u]\n",i); return 0; } }


  return 1;
}


int stopVision()
{

}
