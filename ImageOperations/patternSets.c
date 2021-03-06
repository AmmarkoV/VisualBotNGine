#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "patternSets.h"
#include "../Codecs/codecs.h"


#define DUMP_PATTERN_FAILED_PATCHES 0



#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE "\033[37m" /* White */

int patFileExists(char * filename)
{
  if (filename==0) { return 0; }
  FILE * fp = fopen(filename,"r");
    if (fp!=0)
    {
      fclose(fp);
      return 1;
    }

  return 0;
}


int addToPatternSet(struct PatternSet * set , char * name , unsigned int value , unsigned int score)
{
  unsigned int curSetNum = set->totalPatterns;
  ++set->totalPatterns;

  strncpy(set->pattern[curSetNum].name,name,126);
  set->pattern[curSetNum].value=value;
  set->pattern[curSetNum].acceptScore=score;
  set->pattern[curSetNum].use=1;


  char fName[512];
  unsigned int i=1;
  unsigned int totalTiles = 0;
  while (i<255)
  {
      snprintf(fName,512,"%s%u.pnm",name,i);
      //fprintf(stderr,"Trying to load #%u -> %s \n",i,fName);
      if (patFileExists(fName))
      {
        set->pattern[curSetNum].tile[totalTiles] = readImage( fName , PNM_CODEC , 0 );
        if (set->pattern[curSetNum].tile[totalTiles]!=0)
        {
          fprintf(stderr,GREEN "Loaded %s , we now have %u patterns for set %u\n" NORMAL,fName, totalTiles , curSetNum);

          unsigned int totalPixels = set->pattern[curSetNum].tile[totalTiles]->width * set->pattern[curSetNum].tile[totalTiles]->height;
          unsigned int transPixels =  countOccurancesOfRGBPixel(set->pattern[curSetNum].tile[totalTiles]->pixels,
                                                                                                  set->pattern[curSetNum].tile[totalTiles]->width,
                                                                                                  set->pattern[curSetNum].tile[totalTiles]->height,
                                                                                                  123,123,0);

          set->pattern[curSetNum].nonTransparentPixels[totalTiles] = totalPixels - transPixels;
          set->pattern[curSetNum].totalPixels[totalTiles]= totalPixels ;

          ++totalTiles;
        }
      } else
      {
        if (i==1) { fprintf(stderr,RED "Failed to load any files for #%u -> %s \n" NORMAL,i,fName); }
        set->pattern[curSetNum].totalTiles=totalTiles ;
        break;
      }

      ++i;
  }
  return 1;
}


int dumpPatternSet(struct PatternSet * pattSet ,char * stage)
{
 #if !DUMP_LOADED_PATTERN_SET
  fprintf(stderr,"dumping Pattern sets deactivated\n");
  return 0;
 #endif

 unsigned int tileNum=0;
 unsigned int patternNum=0;
 char fName[512];
 for ( patternNum=0;    patternNum < pattSet->totalPatterns;    patternNum++ )
 {
   fprintf(stderr,"Dumping %s %s :  ",stage,pattSet->pattern[patternNum].name);
   if (pattSet->pattern[patternNum].totalTiles==0)
   {
    fprintf(stderr,"EMPTY");
   } else
   {
     for ( tileNum=0;      tileNum < pattSet->pattern[patternNum].totalTiles;     tileNum++ )
      {
        snprintf(fName,512,"Dump/%sPattern%uTile%u.pnm",stage,patternNum,tileNum);
        fprintf(stderr,"%s ",fName);
        writeImageFile(pattSet->pattern[patternNum].tile[tileNum],PNM_CODEC,fName);
      }
   }
   fprintf(stderr,"\n");

  }
}


int emptyPatternSet(struct PatternSet * pattSet)
{
  unsigned int tileNum=0;
  unsigned int patternNum=0;
  for ( patternNum=0;    patternNum < pattSet->totalPatterns;    patternNum++ )
  {
   for ( tileNum=0;      tileNum < pattSet->pattern[patternNum].totalTiles;     tileNum++ )
   {
        destroyImage( pattSet->pattern[patternNum].tile[tileNum] );
        pattSet->pattern[patternNum].tile[tileNum]=0;
   }
  }
    return 1;
}


unsigned int getPatternSetItemWidth(struct PatternSet * pattSet  , unsigned int patternNum , unsigned int tileNum)
{
    if (patternNum<pattSet->totalPatterns)
    {
      return pattSet->pattern[patternNum].tile[tileNum]->width;
    }
 fprintf(stderr,"getPatternSetItemWidth asking for out of bounds pattern ( number %u / %u )\n",patternNum,pattSet->totalPatterns);
 return 0;
}


int compareToPatternSet(struct PatternSet * pattSet ,
                        unsigned char * screen , unsigned int screenWidth ,unsigned int screenHeight ,
                        unsigned int sX,unsigned int sY , unsigned int width ,unsigned int height ,
                        unsigned int maximumAcceptedScore,
                        unsigned int * pick ,
                        unsigned int * resultPatternNum ,
                        unsigned int * resultTileNum
                        )
{
  //fprintf(stderr,"compareToPatternSet(screen,%u,%u,",screenWidth,screenHeight);
  //fprintf(stderr,"sX=%u,sY=%u,width=%u,height=%u,",sX,sY,width,height);
  //fprintf(stderr,"maxScore=%u)\n",maximumAcceptedScore);

  unsigned int currentScore=maximumAcceptedScore+1;
  unsigned int bestScore=maximumAcceptedScore+1;
  unsigned int bestPick=0;
  unsigned int bestPattern=0;
  unsigned int bestTile=0;
  unsigned int bestCandidates=0;

  unsigned int tileNum=0;
  unsigned int patternNum=0;

  //fprintf(stderr,"Will work on a pattern set with %u Patterns\n",pattSet->totalPatterns);
  for ( patternNum=0;    patternNum < pattSet->totalPatterns;    patternNum++ )
  {


   if (pattSet->pattern[patternNum].totalTiles>0)
   {
    //fprintf(stderr," Pattern set %u has %u tiles\n",patternNum,pattSet->pattern[patternNum].totalTiles);
    for ( tileNum=0;      tileNum < pattSet->pattern[patternNum].totalTiles;     tileNum++ )
    {
       currentScore=maximumAcceptedScore+1;

       //fprintf(stderr," pattern %u / tile %u \n",patternNum,tileNum);
       if (
            compareRGBPatchesIgnoreColor(
                                         /*Main Image*/
                                         screen , sX ,  sY , screenWidth, screenHeight ,
                                         /*Specific Tile*/
                                         pattSet->pattern[patternNum].tile[tileNum]->pixels ,
                                         0,  0 ,
                                         pattSet->pattern[patternNum].tile[tileNum]->width ,
                                         pattSet->pattern[patternNum].tile[tileNum]->height,
                                         /*Ignore R , G , B */
                                         123,123,0,
                                         /*Patch Size*/
                                         width, height ,
                                         /*Return score*/
                                         &currentScore ,
                                         10*maximumAcceptedScore
                                        )
           )
      {
          float differentTransCompensationFactor  = pattSet->pattern[patternNum].totalPixels[tileNum] / pattSet->pattern[patternNum].nonTransparentPixels[tileNum];
          currentScore = currentScore * differentTransCompensationFactor;

       if (currentScore<bestScore)
       {
         //fprintf(stderr,"CMP %u,%u : NEW score %u < best score %u of a %s , compared to %u-%u \n",sX,sY,currentScore,bestScore,getPieceName(bestPick),patternNum,tileNum);
         bestScore = currentScore;
         bestPick = pattSet->pattern[patternNum].value;
         bestPattern=patternNum;
         bestTile=tileNum;
         ++bestCandidates;
       }
      }
     }
    }

   //Done checking with a family of the pattern set
   if (bestScore < pattSet->pattern[patternNum].acceptScore )
    {
      fprintf(stderr,"INSTA-Selected %s with a score of %u \n",pattSet->pattern[patternNum].name,bestScore);
      *pick=bestPick;
      *resultPatternNum=bestPattern;
      *resultTileNum=bestTile;
      return 1;
    }
 }


  fprintf(stderr,"Thinking about best score\n",pattSet->totalPatterns);

   if (bestScore < maximumAcceptedScore )
    {
         unsigned int x=0,y=0;
         char comment[512]={0};
         char nameUsed[512]={0};
         snprintf(nameUsed,512,"Dump/tile%u_%u_like_%u_%u_score_%u",x,y,bestPattern,bestTile,bestScore);
         snprintf(comment,512,"Most like  %u   with score %u", bestPick,bestScore);
         bitBltRGBToFile(  nameUsed ,
                           comment,
                           screen , sX ,  sY , screenWidth, screenHeight, width, height );

         snprintf(nameUsed,512,"Dump/tile%u_%u_like_%u_%u_score_%uB",x,y,bestPattern,bestTile,bestScore);
         bitBltRGBToFile(  nameUsed , comment,
                           pattSet->pattern[bestPattern].tile[bestTile]->pixels ,
                           0 ,  0
                           , pattSet->pattern[bestPattern].tile[bestTile]->width , pattSet->pattern[bestPattern].tile[bestTile]->height
                           , pattSet->pattern[bestPattern].tile[bestTile]->width , pattSet->pattern[bestPattern].tile[bestTile]->height );





      fprintf(stderr,"Not so sure , but selected %u with a score of %u \n",bestPick,bestScore);
      *pick=bestPick;
      *resultPatternNum=bestPattern;
      *resultTileNum=bestTile;
      return 1;
    }


        #if DUMP_PATTERN_FAILED_PATCHES
         fprintf(stderr,"Doing DUMP_PATTERN_FAILED_PATCHES \n");
         unsigned int x=0,y=0;
         char comment[512]={0};
         char nameUsed[512]={0};
         snprintf(nameUsed,512,"Dump/tile%u_%u_like_%u_%u_score_%u",x,y,bestPattern,bestTile,bestScore);
         snprintf(comment,512,"Most like  %u   with score %u", bestPick,bestScore);
         bitBltRGBToFile(  nameUsed , comment,
                           screen ,
                           sX ,  sY ,
                           screenWidth, screenHeight, width, height );

         if (bestCandidates>0)
         {
          snprintf(nameUsed,512,"Dump/tile%u_%u_like_%u_%u_score_%uB",x,y,bestPattern,bestTile,bestScore);
          fprintf(stderr,"Best Pattern is %u \n",bestPattern);
          fprintf(stderr,"Best Tile is %u \n",bestTile);
          bitBltRGBToFile(  nameUsed , comment,
                            pattSet->pattern[bestPattern].tile[bestTile]->pixels ,
                            0 ,  0
                            , pattSet->pattern[bestPattern].tile[bestTile]->width , pattSet->pattern[bestPattern].tile[bestTile]->height
                            , pattSet->pattern[bestPattern].tile[bestTile]->width , pattSet->pattern[bestPattern].tile[bestTile]->height );
         }
        #endif // DUMP_PATCHES

 return 0;
}
