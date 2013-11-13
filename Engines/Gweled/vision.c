#include <stdio.h>
#include <stdlib.h>


#include "vision.h"
#include "Gweled.h"


#include "../../ImageOperations/imageOps.h"
#include "../../Codecs/codecs.h"

#define GOOD_SCORE_BELOW 500
#define MAXIMUM_ACCEPTED_SCORE 6000
#define MAXIMUM_ACCEPTED_SCORE_FOR_BUTTONS 90000

#define DUMP_PATTERN_FAILED_PATCHES 0
#define DUMP_UNKNOWN_PATCHES 0
#define NO_PATCH_COMPARISON 0


unsigned int singlePixelAssignments = 0;
unsigned int patternAssignments = 0;
unsigned int singlePixelAssignmentsTotal = 0;
unsigned int patternAssignmentsTotal = 0;

struct ButtonChecks
{
   struct Image * buttonsImg;
   signed int checkX;
   signed int checkY;
};


unsigned int numberOfButtons=0;
struct ButtonChecks buttons[64]={0};


struct PatternItem
{
   unsigned int totalTiles;
   unsigned int  value;
   unsigned char use;
   unsigned int acceptScore;
   struct Image * tile[256];
   char name[512];
};

struct PatternSet
{
    unsigned int totalPatterns;
    struct PatternItem pattern[256];
};


struct PatternSet set;


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
  unsigned int i=0;
  unsigned int totalTiles = 0;
  while (i<255)
  {
      sprintf(fName,"%s%u.pnm",name,i+1);
      if (patFileExists(fName))
      {
        set->pattern[curSetNum].tile[totalTiles] = readImage( fName , PNM_CODEC , 0 );
        if (set->pattern[curSetNum].tile[totalTiles]!=0)
        {
          fprintf(stderr,"Loaded %s , we now have %u patterns for set %u\n",fName, totalTiles , curSetNum);
          //It appears we correctly loaded a new pattern file
          ++totalTiles;
        }
      } else
      {
        set->pattern[curSetNum].totalTiles=totalTiles ;
        break;
      }

      ++i;
  }
  return 1;
}

int dumpPatternSet(struct PatternSet * pattSet ,char * stage)
{
 fprintf(stderr,"dumping Pattern sets deactivated\n");
 return 0;
 unsigned int tileNum=0;
 unsigned int patternNum=0;
 char fName[512];
 for ( patternNum=0;    patternNum < pattSet->totalPatterns;    patternNum++ )
 {
   fprintf(stderr,"Dumping %s %s\n",stage,pattSet->pattern[patternNum].name);
   for ( tileNum=0;      tileNum < pattSet->pattern[patternNum].totalTiles;     tileNum++ )
   {
        sprintf(fName,"Dump/%sPattern%uTile%u.pnm",stage,patternNum,tileNum);
        writeImageFile(pattSet->pattern[patternNum].tile[tileNum],PNM_CODEC,fName);
   }
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


unsigned int seeFunctionCalls=0;


int compareTableTile(struct PatternSet * pattSet ,
                     unsigned char * screen , unsigned int screenWidth ,unsigned int screenHeight ,
                     unsigned int sX,unsigned int sY , unsigned int width ,unsigned int height , unsigned int * pick)
{
   #if NO_PATCH_COMPARISON
     return 0; //Disabled until it is fixed
   #endif // NO_PATCH_COMPARISON


   unsigned int currentScore=10000000;
   unsigned int bestScore=10000000;
   unsigned int bestPick=NO_PIECE;
   unsigned int bestPattern=0;
   unsigned int bestTile=0;


   if ( colorVariance( screen, screenWidth ,screenHeight , sX,  sY, width , height) < 10150 )
   {
       *pick=NO_PIECE;
       return 1;
   }


  unsigned int tileNum=0;
  unsigned int patternNum=0;
  for ( patternNum=0;    patternNum < pattSet->totalPatterns;    patternNum++ )
  {
    fprintf(stderr,"Checking for %s\n",pattSet->pattern[patternNum].name);
    for ( tileNum=0;      tileNum < pattSet->pattern[patternNum].totalTiles;     tileNum++ )
   {
       currentScore=10000000;

       compareRGBPatchesIgnoreColor(
                                     /*Main Image*/
                                     screen , sX ,  sY , screenWidth, screenHeight ,
                                     /*Specific Tile*/
                                     pattSet->pattern[patternNum].tile[tileNum]->pixels , 0,  0 ,
                                     pattSet->pattern[patternNum].tile[tileNum]->width ,
                                     pattSet->pattern[patternNum].tile[tileNum]->height,
                                     /*Ignore R , G , B */
                                     123,123,0,
                                     /*Patch Size*/
                                     width, height ,
                                     /*Return score*/
                                     &currentScore ,
                                     MAXIMUM_ACCEPTED_SCORE
                                    );



       if (currentScore<bestScore)
       {
         bestScore = currentScore;
         bestPick = pattSet->pattern[patternNum].value;
         bestPattern=patternNum;
         bestTile=tileNum;
       }
   }
   if (bestScore < pattSet->pattern[patternNum].acceptScore )
    {
      fprintf(stderr,"INSTA-Selected %s with a score of %u \n",pattSet->pattern[patternNum].name,bestScore);
      *pick=bestPick;
      return 1;
    }
 }

   if (bestScore < MAXIMUM_ACCEPTED_SCORE )
    {
      fprintf(stderr,"Not so sure , but selected %u with a score of %u \n",bestPick,bestScore);
      *pick=bestPick;
      return 1;
    }

        #if DUMP_PATTERN_FAILED_PATCHES
         char comment[512]={0};
         char nameUsed[512]={0};
         sprintf(nameUsed,"Dump/failedPattern%uTile%u_like%s_score_%u",bestPattern,bestTile,getPieceName(bestPick),bestScore);
         sprintf(comment,"Most like %s ( %u )  with score %u",getPieceName(bestPick),bestPick,bestScore);
         bitBltRGBToFile(  nameUsed ,
                           comment,
                           screen , sX ,  sY , screenWidth, screenHeight, width, height );

         sprintf(nameUsed,"Dump/failedPattern%uTile%u_like%s_score_%uB",bestPattern,bestTile,getPieceName(bestPick),bestScore);
         bitBltRGBToFile(  nameUsed , comment,
                           pattSet->pattern[bestPattern].tile[bestTile]->pixels ,
                           0 ,  0
                           , pattSet->pattern[bestPattern].tile[bestTile]->width , pattSet->pattern[bestPattern].tile[bestTile]->height
                           , pattSet->pattern[bestPattern].tile[bestTile]->width , pattSet->pattern[bestPattern].tile[bestTile]->height );
        #endif // DUMP_PATCHES

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

    char dumpMsg[512];
    sprintf(dumpMsg,"see%u",seeFunctionCalls);
    dumpPatternSet(&set,dumpMsg);


    patternAssignments=0;
    singlePixelAssignments=0;

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

      if
        (
          compareTableTile(&set,
                           screen,screenWidth,screenHeight,
                           settings.clientX + x*settings.blockX,
                           settings.clientY + y*settings.blockY,
                           settings.blockX ,
                           settings.blockY , &table[x][y] )
        )
        {
          fprintf(stderr,"Table[%u][%u] got assigned via tiles\n",x,y);
          ++patternAssignments; ++patternAssignmentsTotal;
        }
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
         ++singlePixelAssignments; ++singlePixelAssignmentsTotal;
        }



        #if DUMP_UNKNOWN_PATCHES
        if (table[x][y]==UNKNOWN_PIECE)
        {
         char nameUsed[512]={0};
         sprintf(nameUsed,"Dump/tile%u_%u_%u",seeFunctionCalls,x,y);
         bitBltRGBToFile(  nameUsed ,
                           0,
                           screen ,
                           settings.clientX + x*settings.blockX,
                           settings.clientY + y*settings.blockY,
                           screenWidth , screenHeight ,
                           settings.blockX,settings.blockY);
        }
        #endif // DUMP_PATCHES


     }
    }
}





int seeButtons( unsigned char * screen , unsigned int screenWidth ,unsigned int screenHeight , unsigned int * clickX , unsigned int * clickY )
{
  unsigned int currentScore=10000000;
  unsigned int buttonNum = 0;
   for ( buttonNum=0;    buttonNum < numberOfButtons;     buttonNum++ )
   {
       fprintf(stderr,"Comparing Button %u \n",buttonNum);
       unsigned int sX = settings.clientX + buttons[buttonNum].checkX;
       unsigned int sY = settings.clientY + buttons[buttonNum].checkY;
       fprintf(stderr,"%u,%u size %ux%u \n", sX ,  sY , buttons[buttonNum].buttonsImg->width , buttons[buttonNum].buttonsImg->height);
       compareRGBPatchesNeighborhoodIgnoreColor(
                                     3,3, //Neighborhood
                                     /*Main Image*/
                                     screen , sX ,  sY , screenWidth, screenHeight ,
                                     /*Specific Tile*/
                                     buttons[buttonNum].buttonsImg->pixels , 0,  0 ,
                                     buttons[buttonNum].buttonsImg->width ,
                                     buttons[buttonNum].buttonsImg->height,
                                     /*Ignore R , G , B */
                                     123,123,0,
                                     /*Patch Size*/
                                     buttons[buttonNum].buttonsImg->width,
                                     buttons[buttonNum].buttonsImg->height ,
                                     /*Return score*/
                                     &currentScore ,
                                     MAXIMUM_ACCEPTED_SCORE_FOR_BUTTONS
                                    );
       fprintf(stderr,"Compared Button %u , score %u \n",buttonNum,currentScore);


      if (currentScore<MAXIMUM_ACCEPTED_SCORE_FOR_BUTTONS)
      {
        fprintf(stderr,"Button %u Matches\n",buttonNum);
        *clickX = (unsigned int) sX + buttons[buttonNum].buttonsImg->width/2;
        *clickY = (unsigned int) sY + buttons[buttonNum].buttonsImg->height/2;
        return 1;
      }
   }
  fprintf(stderr,"No Button Matches\n");

  return 0;
}









int initVision()
{
  set.totalPatterns=0;
  //addToPatternSet(&set,"Engines/Gweled/Pieces/neutral",4,NO_PIECE,5000);
  addToPatternSet(&set,"Engines/Gweled/Pieces/hypercube",HYPERCUBE_PIECE,GOOD_SCORE_BELOW);
  addToPatternSet(&set,"Engines/Gweled/Pieces/yellow",YELLOW_PIECE,GOOD_SCORE_BELOW);
  addToPatternSet(&set,"Engines/Gweled/Pieces/orange",ORANGE_PIECE,GOOD_SCORE_BELOW);
  addToPatternSet(&set,"Engines/Gweled/Pieces/red",RED_PIECE,GOOD_SCORE_BELOW);
  addToPatternSet(&set,"Engines/Gweled/Pieces/blue",BLUE_PIECE,GOOD_SCORE_BELOW);
  addToPatternSet(&set,"Engines/Gweled/Pieces/green",GREEN_PIECE,GOOD_SCORE_BELOW);
  addToPatternSet(&set,"Engines/Gweled/Pieces/pink",PINK_PIECE,GOOD_SCORE_BELOW);
  addToPatternSet(&set,"Engines/Gweled/Pieces/white",WHITE_PIECE,GOOD_SCORE_BELOW);
  dumpPatternSet(&set,"Initialization");
// exit(0);

  numberOfButtons=0;
  buttons[numberOfButtons].buttonsImg=readImage("Engines/Gweled/Menus/xButton.pnm",PNM_CODEC,0);
  //buttons[numberOfButtons].checkX = (signed int) 827-settings.clientX;
  //buttons[numberOfButtons].checkY = (signed int) 441-settings.clientY;
  buttons[numberOfButtons].checkX = (signed int) 827 - 337;
  buttons[numberOfButtons].checkY = (signed int) 441 - 382;
  if (buttons[numberOfButtons].buttonsImg!=0) { ++numberOfButtons; }

  buttons[numberOfButtons].buttonsImg=readImage("Engines/Gweled/Menus/playButton.pnm",PNM_CODEC,0);
  buttons[numberOfButtons].checkX =  (signed int) 251 - 337;
  buttons[numberOfButtons].checkY =  (signed int) 583 - 382;
  if (buttons[numberOfButtons].buttonsImg!=0) { ++numberOfButtons; }

  buttons[numberOfButtons].buttonsImg=readImage("Engines/Gweled/Menus/playNowButton.pnm",PNM_CODEC,0);
  buttons[numberOfButtons].checkX = (signed int) 379 - 337 ;
  buttons[numberOfButtons].checkY = (signed int) 672 - 382;
  if (buttons[numberOfButtons].buttonsImg!=0) { ++numberOfButtons; }

  buttons[numberOfButtons].buttonsImg=readImage("Engines/Gweled/Menus/noThanks.pnm",PNM_CODEC,0);
  buttons[numberOfButtons].checkX = (signed int) 308 - 337 ;
  buttons[numberOfButtons].checkY = (signed int) 697 - 382;
  if (buttons[numberOfButtons].buttonsImg!=0) { ++numberOfButtons; }

  buttons[numberOfButtons].buttonsImg=readImage("Engines/Gweled/Menus/playAgain.pnm",PNM_CODEC,0);
  buttons[numberOfButtons].checkX = (signed int) 311 - 337;
  buttons[numberOfButtons].checkY = (signed int) 790 - 382;
  if (buttons[numberOfButtons].buttonsImg!=0) { ++numberOfButtons; }

  buttons[numberOfButtons].buttonsImg=readImage("Engines/Gweled/Menus/continueButton.pnm",PNM_CODEC,0);
  buttons[numberOfButtons].checkX = (signed int) 580 - 337;
  buttons[numberOfButtons].checkY = (signed int) 682 - 382;
  if (buttons[numberOfButtons].buttonsImg!=0) { ++numberOfButtons; }


  buttons[numberOfButtons].buttonsImg=readImage("Engines/Gweled/Menus/cancelButton.pnm",PNM_CODEC,0);
  buttons[numberOfButtons].checkX = (signed int) 743 - 619;
  buttons[numberOfButtons].checkY = (signed int) 782 - 354;
  if (buttons[numberOfButtons].buttonsImg!=0) { ++numberOfButtons; }

  buttons[numberOfButtons].buttonsImg=readImage("Engines/Gweled/Menus/leaveParty.pnm",PNM_CODEC,0);
  buttons[numberOfButtons].checkX = (signed int) 485 - 619;
  buttons[numberOfButtons].checkY = (signed int) 645 - 354;
  if (buttons[numberOfButtons].buttonsImg!=0) { ++numberOfButtons; }

  return 1;
}


int stopVision()
{
  emptyPatternSet(&set);
}
