#include <stdio.h>
#include <string.h>
#include "pick.h"

#include "../../ImageOperations/patternSets.h"
#include "../../ImageOperations/ocr.h"
#include "../../ImageOperations/imageOps.h"
#include "../../Codecs/codecs.h"



// 96x67
// RADIANT 175 299 423 547 672 DIRE 1156 1276 1400 1524 1648

int totalSeeTimes=0;


struct PatternSet set;
struct Image * emptyPick=0;

/*
int seeTable(unsigned int table[8][8] ,
             unsigned char * screen , unsigned int screenWidth ,unsigned int screenHeight ,
             unsigned int clientStartX , unsigned int clientStartY)
{
    ++seeFunctionCalls;

    char dumpMsg[512];
    sprintf(dumpMsg,"see%u",seeFunctionCalls);
    dumpPatternSet(&set,dumpMsg);



    unsigned int halfBlockX = (unsigned int) settings.blockX/2;
    unsigned int halfBlockY = (unsigned int) settings.blockY/2;

    unsigned char R,G,B;
    unsigned int x,y;
    for (y=0; y<8; y++)
    {
     for (x=0; x<8; x++)
     {
      //Originally unknown piece
      table[x][y]=UNKNOWN_PIECE;

      fprintf(stderr,"%u,%u table has %u,%u coords\n",x,y,clientStartX + x*settings.blockX, clientStartY + y*settings.blockY);
      if
        (
          compareTableTile(&set,
                           screen,screenWidth,screenHeight,
                           clientStartX + x*settings.blockX,
                           clientStartY + y*settings.blockY,
                           settings.blockX ,
                           settings.blockY ,
                           &table[x][y] )
        )
        {
          fprintf(stderr,"Table[%u][%u] got assigned via tiles\n",x,y);
        }



        #if DUMP_UNKNOWN_PATCHES
        if (table[x][y]==UNKNOWN_PIECE)
        {
         char nameUsed[512]={0};
         sprintf(nameUsed,"Dump/tile%u_%u_%u",seeFunctionCalls,x,y);
         bitBltRGBToFile(  nameUsed ,
                           0,
                           screen ,
                           clientStartX + x*settings.blockX,
                           clientStartY + y*settings.blockY,
                           screenWidth , screenHeight ,
                           settings.blockX,settings.blockY);
        }
        #endif // DUMP_PATCHES


     }
    }

  return 1;
}
*/
int initializeSeeingPicks()
{
 emptyPick = readImage( "tiles/empty.pnm",PNM_CODEC, 0 );
}


int isPickEmpty(unsigned int pid , struct pickScreen * picks , struct Image * view)
{

  unsigned int currentScore=1000000;
  compareRGBPatchesIgnoreColor(
                               /*Main Image*/
                               view->pixels , picks->avatarX[pid] ,  picks->avatarY[pid] , view->width , view->height,
                               /*Specific Tile*/
                               emptyPick->pixels ,
                               0, 0 ,
                               emptyPick->width ,
                               emptyPick->height,
                               /*Ignore R , G , B */
                               123,123,0,
                               /*Patch Size*/
                               picks->avatarWidth, picks->avatarHeight,
                               /*Return score*/
                               &currentScore ,
                               10*currentScore
                              );

  fprintf(stderr,"pick%u empty score %u :  ",pid,currentScore);
  if (currentScore<190000) { fprintf(stderr,"EMPTY\n"); return 1;}

  fprintf(stderr,"PICKED\n");
  return 0;
}






int seePicks(struct teams * team ,struct Image * view)
{
// RADIANT 175 299 423 547 672 DIRE 1156 1276 1400 1524 1648

  struct pickScreen picks={0};
  picks.avatarWidth=96; picks.avatarHeight=60;

  //RADIANT
  picks.avatarX[0]=175;  picks.avatarY[0]=7;
  picks.avatarX[1]=299;  picks.avatarY[1]=7;
  picks.avatarX[2]=423;  picks.avatarY[2]=7;
  picks.avatarX[3]=547;  picks.avatarY[3]=7;
  picks.avatarX[4]=672;  picks.avatarY[4]=7;

  //DIRE
  picks.avatarX[5]=1156;  picks.avatarY[5]=7;
  picks.avatarX[6]=1276;  picks.avatarY[6]=7;
  picks.avatarX[7]=1400;  picks.avatarY[7]=7;
  picks.avatarX[8]=1524;  picks.avatarY[8]=7;
  picks.avatarX[9]=1648;  picks.avatarY[9]=7;


  char img[256];
  unsigned int i=0;
  for (i=0; i<10; i++)
  {

    if (!isPickEmpty(i, &picks , view) )
     {


   snprintf(img,256,"avatars/av_%u_%u",i,totalSeeTimes);
   bitBltRGBToFile( img,
                    0,
                    view->pixels ,
                    0 + picks.avatarX[i],
                    0 + picks.avatarY[i],
                    view->width, view->height,
                    picks.avatarWidth+1,picks.avatarHeight+1);


     }

  }


  ++totalSeeTimes;
}
