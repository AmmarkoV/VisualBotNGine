#include <stdio.h>
#include <stdlib.h>


#include "vision.h"
#include "Gweled.h"


#include "../../ImageOperations/patternSets.h"

#include "../../ImageOperations/imageOps.h"
#include "../../Codecs/codecs.h"


#include "../../ImageOperations/ocr.h"

#define MAX_OCR_STRING 512

#define GOOD_SCORE_BELOW 500
#define MAXIMUM_ACCEPTED_SCORE 70000
#define MAXIMUM_ACCEPTED_SCORE_FOR_BUTTONS 90000

#define DUMP_LOADED_PATTERN_SET 0
#define DUMP_PATTERN_FAILED_PATCHES 0
#define DUMP_UNKNOWN_PATCHES 0
#define NO_PATCH_COMPARISON 0

unsigned int seeFunctionCalls=0;


struct ButtonChecks
{
   struct Image * buttonsImg;
   signed int checkX;
   signed int checkY;
};


unsigned int numberOfButtons=0;
struct ButtonChecks buttons[64]={0};



struct PatternSet set;

struct PatternItem scorePlaceholder;
signed int scorePlaceholderX=195-337,scorePlaceholderY=368-382;

struct PatternSet scoreOCR;
signed int scoreOCRX=213-337,scoreOCRY=378-382;
unsigned int scoreOCRSize=15;


int compareTableTile(struct PatternSet * pattSet ,
                     unsigned char * screen , unsigned int screenWidth ,unsigned int screenHeight ,
                     unsigned int sX,unsigned int sY , unsigned int width ,unsigned int height ,
                     unsigned int * pick)
{
   #if NO_PATCH_COMPARISON
     return 0; //We may not want patch comparison
   #endif // NO_PATCH_COMPARISON

  unsigned int patternNum , tileNum;

   if ( colorVariance( screen, screenWidth ,screenHeight , sX,  sY, width , height) < 10150 )
   {
       *pick=NO_PIECE;
       return 1;
   }

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
       if ( *pick!= UNKNOWN_PIECE )
       {
           return 1;
       }
   }

 return 0;
}



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
                                     buttons[buttonNum].buttonsImg->pixels ,
                                      0,  0 ,
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




int seeScore( unsigned char * screen , unsigned int screenWidth ,unsigned int screenHeight ,
              unsigned int x,unsigned int y , unsigned int width , unsigned int height ,
              char * strOutput , unsigned int strOutputMaxLength
            )
{
   if (
        doOCR(
              screen, screenWidth,screenHeight,
              x , y  , width , height  ,
              &scoreOCR ,
              strOutput ,
              strOutputMaxLength
             )
       )
        { printf("OCR String is %s \n",strOutput);  return 1; } else
        { printf("Could not read an OCR String \n"); }
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


//  scorePlaceholder=readImage("Engines/Gweled/Menus/score.pnm",PNM_CODEC,0);
    scoreOCR.totalPatterns=0;
    addToPatternSet(&scoreOCR,"Engines/Gweled/OCR/score0_",0+'0',500);
    addToPatternSet(&scoreOCR,"Engines/Gweled/OCR/score1_",0+'1',500);
    addToPatternSet(&scoreOCR,"Engines/Gweled/OCR/score2_",0+'2',500);
    addToPatternSet(&scoreOCR,"Engines/Gweled/OCR/score3_",0+'3',500);
    addToPatternSet(&scoreOCR,"Engines/Gweled/OCR/score4_",0+'4',500);
    addToPatternSet(&scoreOCR,"Engines/Gweled/OCR/score5_",0+'5',500);
    addToPatternSet(&scoreOCR,"Engines/Gweled/OCR/score6_",0+'6',500);
    addToPatternSet(&scoreOCR,"Engines/Gweled/OCR/score7_",0+'7',500);
    addToPatternSet(&scoreOCR,"Engines/Gweled/OCR/score8_",0+'8',500);
    addToPatternSet(&scoreOCR,"Engines/Gweled/OCR/score9_",0+'9',500);
    addToPatternSet(&scoreOCR,"Engines/Gweled/OCR/scoreComma_",0+',',500);
    addToPatternSet(&scoreOCR,"Engines/Gweled/OCR/scoreSpace_",0+' ',500);

//  195-337 368-382


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
  emptyPatternSet(&scoreOCR);
}
