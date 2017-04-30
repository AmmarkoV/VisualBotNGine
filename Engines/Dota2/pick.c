#include <stdio.h>
#include <string.h>
#include "pick.h"
#include "heroes.h"

#include "../../ImageOperations/patternSets.h"
#include "../../ImageOperations/ocr.h"
#include "../../ImageOperations/imageOps.h"
#include "../../Codecs/codecs.h"


#define MAXIMUM_ACCEPTED_SCORE 200000

// 96x67
// RADIANT 175 299 423 547 672 DIRE 1156 1276 1400 1524 1648

int totalSeeTimes=0;


struct PatternSet selectionSet={0};
struct PatternSet pickedSet={0};
struct Image * emptyPick=0;


int initializeSeeingPicks()
{
 emptyPick = readImage( "tiles/empty.pnm",PNM_CODEC, 0 );

 char heroPath[128];
 unsigned int i=0;
 for (i=0; i<d2_number_of_heroes; i++)
  {
   snprintf(heroPath,128,"tiles/%s_s.pnm",dota2InternalHeroNames[i]);
   addToPatternSet(&selectionSet,heroPath,i,MAXIMUM_ACCEPTED_SCORE);
  }
  dumpPatternSet(&selectionSet,"selections");

 for (i=0; i<d2_number_of_heroes; i++)
  {
   snprintf(heroPath,128,"tiles/%s_p.pnm",dota2InternalHeroNames[i]);
   addToPatternSet(&pickedSet,heroPath,i,MAXIMUM_ACCEPTED_SCORE);
  }
  dumpPatternSet(&pickedSet,"picks");

 exit(0);

 return 1;
}




int compareTableTile(struct PatternSet * pattSet ,
                     unsigned char * screen , unsigned int screenWidth ,unsigned int screenHeight ,
                     unsigned int sX,unsigned int sY , unsigned int width ,unsigned int height ,
                     unsigned int * pick)
{
   #if NO_PATCH_COMPARISON
     return 0; //We may not want patch comparison
   #endif // NO_PATCH_COMPARISON

  unsigned int patternNum , tileNum;

/*
   if ( colorVariance( screen, screenWidth ,screenHeight , sX,  sY, width , height) < 10150 )
   {
       *pick=0;
       return 1;
   }
*/

  fprintf(stderr,"\ncompareTableTile called \n");

   if (
        compareToPatternSet( pattSet ,
                             screen , screenWidth , screenHeight ,
                             sX,sY , width ,height ,
                             MAXIMUM_ACCEPTED_SCORE ,
                             pick ,
                             &patternNum ,
                             &tileNum)
      )
   {
       if ( *pick!= d2_unknown )
       {
           return 1;
       }
   }

 return 0;
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
       if
        (
          compareTableTile(&selectionSet,
                           view->pixels , view->width , view->height,
                           0 + picks.avatarX[i],
                           0 + picks.avatarY[i],
                           picks.avatarWidth,
                           picks.avatarHeight,
                           &team->playersHeroes[i])
        )
        {
          fprintf(stderr,"Selected[%u] got assigned via tiles\n",i);
          if (team->playersPicked==0)
            { saySomethingInDota2("Player %u highlighted a selection of hero %u ",team->playersHeroes[i]); }

          team->playersPicked[i]=1;
        }
        else
       if
        (
          compareTableTile(&pickedSet,
                           view->pixels , view->width , view->height,
                           0 + picks.avatarX[i],
                           0 + picks.avatarY[i],
                           picks.avatarWidth,
                           picks.avatarHeight,
                           &team->playersHeroes[i])
        )
        {
          fprintf(stderr,"Picked[%u] got assigned via tiles\n",i);
          if (team->playersPicked==0)
            { saySomethingInDota2("Player %u picked a selection of hero %u ",team->playersHeroes[i]); }
          team->playersPicked[i]=2;
        }
        else
        {
         //store unknown thing..!
         fprintf(stderr,"Unknown thing @ %u \n",i);
         snprintf(img,256,"avatars/av_%u_%u",i,totalSeeTimes);
         bitBltRGBToFile( img,
                    0,
                    view->pixels ,
                    0 + picks.avatarX[i],
                    0 + picks.avatarY[i],
                    view->width, view->height,
                    picks.avatarWidth+1,picks.avatarHeight+1);
        }



     } else
     {
        team->playersHeroes[i]=0;
        team->playersPicked[i]=0;
     }

  }


  ++totalSeeTimes;
 return 1;
}
