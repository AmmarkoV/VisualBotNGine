#include <stdio.h>
#include <stdlib.h>

#include "Codecs/codecs.h"
#include "ImageOperations/findSubImage.h"
#include "ImageOperations/imageOps.h"

enum gamePieces
{
  NO_PIECE = 0,
  GREEN_PIECE ,
  WHITE_PIECE ,
  ORANGE_PIECE ,
  YELLOW_PIECE ,
  BLUE_PIECE ,
  RED_PIECE ,
  PINK_PIECE
};

int allowSnapshot=1;
int allowMouseControl=1;

unsigned int clickdelay = 100*1000;
unsigned int delay = 10*1000;

unsigned int lastMoveX=4,lastMoveY=2;

#define NORMAL "\033[0m"
#define BLACK "\033[30m" /* Black */
#define RED "\033[31m" /* Red */
#define GREEN "\033[32m" /* Green */
#define YELLOW "\033[33m" /* Yellow */
#define BLUE "\033[34m" /* Blue */
#define MAGENTA "\033[35m" /* Magenta */
#define CYAN "\033[36m" /* Cyan */
#define WHITE "\033[37m" /* White */


#define ABSDIFF(num1,num2) ( (num1-num2) >=0 ? (num1-num2) : (num2 - num1) )


int writePieceChar(int pieceVal)
{
  switch (pieceVal)
  {
    case GREEN_PIECE : fprintf(stderr,GREEN " G" NORMAL);  break;
    case WHITE_PIECE : fprintf(stderr,WHITE " W" NORMAL);  break;
    case ORANGE_PIECE : fprintf(stderr,YELLOW " O" NORMAL);  break;
    case YELLOW_PIECE : fprintf(stderr,YELLOW " Y" NORMAL);  break;
    case BLUE_PIECE : fprintf(stderr,CYAN " B" NORMAL);  break;
    case RED_PIECE : fprintf(stderr,RED " R" NORMAL);  break;
    case PINK_PIECE : fprintf(stderr,MAGENTA " P" NORMAL);  break;
    default :
     fprintf(stderr," ?" );
    break;
  }

  return 0;
}



struct solutionItem
{
    unsigned int fromX;
    unsigned int fromY;
    unsigned int toX;
    unsigned int toY;
    unsigned int score;
};

struct solutionList
{
  struct solutionItem solution[256];
  unsigned int currentSolutions;
};


int addMoveToList(struct solutionList * list ,  unsigned int fromX,unsigned int fromY , unsigned int toX, unsigned int toY , unsigned int score)
{
   list->solution[list->currentSolutions].fromX=fromX;
   list->solution[list->currentSolutions].fromY=fromY;
   list->solution[list->currentSolutions].toX=toX;
   list->solution[list->currentSolutions].toY=toY;
   list->solution[list->currentSolutions].score=score;

  fprintf(stderr,"addMoveToList[%u] (%u,%u,%u,%u) ==  %u \n",list->currentSolutions,fromX,fromY,toX,toY,score);

   ++list->currentSolutions;

   return 1;
}

int pickBestMoveOfList(struct solutionList * list ,  unsigned int *fromX,unsigned int *fromY , unsigned int *toX, unsigned int *toY)
{
  unsigned int bestScore=0;
  unsigned int i=0;
  for (i=0; i<list->currentSolutions; i++)
  {
      fprintf(stderr,"Move %u  score %u\n",i,list->solution[i].score );
     if (list->solution[i].score > bestScore)
     {
        *fromX=list->solution[list->currentSolutions].fromX;
        *fromY=list->solution[list->currentSolutions].fromY;
        *toX=list->solution[list->currentSolutions].toX;
        *toY=list->solution[list->currentSolutions].toY;
        fprintf(stderr,"Move %u is best with score %u\n",i,bestScore);
     }
  }
  return 1;
}



unsigned int getScoreForMove(  unsigned int fromX,unsigned int fromY  , unsigned int toX, unsigned int toY )
{

  unsigned int a = ABSDIFF(toX,lastMoveX);
  unsigned int b = ABSDIFF(toY,lastMoveY);
  unsigned int score = a*a +b*b;

  fprintf(stderr,"getScoreForMove(%u,%u,%u,%u) ==  %u \n",fromX,fromY,toX,toY,score);
  return (unsigned int) score;
}

int suggestMove(unsigned int table[8][8] , unsigned int *fromX,unsigned int *fromY , unsigned int *toX, unsigned int *toY)
{
    *fromX=0; *fromY=0; *toX=0; *toY=0;

    unsigned int limitX=8;
    unsigned int limitY=8;

    struct solutionList list={0};
    list.currentSolutions=0;

    unsigned int score =0;
    unsigned int x,y=0;
    for (y=0; y<8; y++)
    {
     for (x=0; x<8; x++)
     {
        if (table[x][y]==0) {} else
        //---------------------------------------------------------------------------
        if (
             (x+2<limitX)&&(y+1<limitY)&&
             (table[x][y]==table[x+1][y])&&
             (table[x][y]==table[x+2][y+1])
           )
             { /* X X A
                  B C X */
                *fromX = x+2; *fromY = y+1; *toX = x+2; *toY = y;
                fprintf(stderr,"ASolution %u,%u -> %u,%u \n",*fromX,*fromY,*toX,*toY);
                score=getScoreForMove(*fromX,*fromY,*toX,*toY);
                addMoveToList(&list,*fromX,*fromY,*toX,*toY,score);
             } else
        //---------------------------------------------------------------------------
        if (
             (x+2<limitX)&&(y+1<limitY)&&
             (table[x][y+1]==table[x+1][y+1])&&
             (table[x][y+1]==table[x+2][y])
           )
             { /* A B X
                  X X C */
                *fromX = x+2; *fromY = y; *toX = x+2; *toY = y+1;
                fprintf(stderr,"BSolution %u,%u -> %u,%u \n",*fromX,*fromY,*toX,*toY);
                score=getScoreForMove(*fromX,*fromY,*toX,*toY);
                addMoveToList(&list,*fromX,*fromY,*toX,*toY,score);
             } else
        //---------------------------------------------------------------------------
        if (
             (x+2<limitX)&&(y+1<limitY)&&
             (table[x+1][y]==table[x+2][y])&&
             (table[x][y+1]==table[x+2][y])
           )
             { /* A X X
                  X B C */
                *fromX = x; *fromY = y+1; *toX = x; *toY = y;
                fprintf(stderr,"CSolution %u,%u -> %u,%u \n",*fromX,*fromY,*toX,*toY);
                score=getScoreForMove(*fromX,*fromY,*toX,*toY);
                addMoveToList(&list,*fromX,*fromY,*toX,*toY,score);
             } else
        //---------------------------------------------------------------------------
        if (
             (x+2<limitX)&&(y+1<limitY)&&
             (table[x][y]==table[x+1][y+1])&&
             (table[x][y]==table[x+2][y+1])
           )
             { /* X B C
                  A X X */
                *fromX = x; *fromY = y; *toX = x; *toY = y+1;
                fprintf(stderr,"DSolution %u,%u -> %u,%u \n",*fromX,*fromY,*toX,*toY);
                score=getScoreForMove(*fromX,*fromY,*toX,*toY);
                addMoveToList(&list,*fromX,*fromY,*toX,*toY,score);
             } else
        //---------------------------------------------------------------------------
        if (
             (x+2<limitX)&&(y+1<limitY)&&
             (table[x][y+1]==table[x+1][y])&&
             (table[x][y+1]==table[x+2][y+1])
           )
             { /* A X C
                  X B X */
                *fromX = x+1; *fromY = y; *toX = x+1; *toY = y+1;
                fprintf(stderr,"D2Solution %u,%u -> %u,%u \n",*fromX,*fromY,*toX,*toY);
                score=getScoreForMove(*fromX,*fromY,*toX,*toY);
                addMoveToList(&list,*fromX,*fromY,*toX,*toY,score);
             } else
        //---------------------------------------------------------------------------
        if (
             (x+2<limitX)&&(y+1<limitY)&&
             (table[x][y]==table[x+1][y+1])&&
             (table[x][y]==table[x+2][y])
           )
             { /* X B X
                  A X C */
                *fromX = x+1; *fromY = y+1; *toX = x+1; *toY = y;
                fprintf(stderr,"D3Solution %u,%u -> %u,%u \n",*fromX,*fromY,*toX,*toY);
                score=getScoreForMove(*fromX,*fromY,*toX,*toY);
                addMoveToList(&list,*fromX,*fromY,*toX,*toY,score);
             } else
        //---------------------------------------------------------------------------
        if (
             (x+1<limitX)&&(y+2<limitY)&&
             (table[x+1][y]==table[x][y+1])&&
             (table[x+1][y]==table[x][y+2])
           )
             { /* A X
                  X B
                  X C */
                *fromX = x+1; *fromY = y; *toX = x; *toY = y;
                fprintf(stderr,"ESolution %u,%u -> %u,%u \n",*fromX,*fromY,*toX,*toY);
                score=getScoreForMove(*fromX,*fromY,*toX,*toY);
                addMoveToList(&list,*fromX,*fromY,*toX,*toY,score);
             } else
        //---------------------------------------------------------------------------
        if (
             (x+1<limitX)&&(y+2<limitY)&&
             (table[x][y]==table[x+1][y+1])&&
             (table[x][y]==table[x+1][y+2])
           )
             { /* X A
                  B X
                  C X */
                *fromX = x; *fromY = y; *toX = x+1; *toY = y;
                fprintf(stderr,"FSolution %u,%u -> %u,%u \n",*fromX,*fromY,*toX,*toY);
                score=getScoreForMove(*fromX,*fromY,*toX,*toY);
                addMoveToList(&list,*fromX,*fromY,*toX,*toY,score);
             } else
        //---------------------------------------------------------------------------
        if (
             (x+1<limitX)&&(y+2<limitY)&&
             (table[x][y]==table[x+1][y+1])&&
             (table[x][y]==table[x][y+2])
           )
             { /* X A
                  B X
                  X C */
                *fromX = x+1; *fromY = y+1; *toX = x; *toY = y+1;
                fprintf(stderr,"GSolution %u,%u -> %u,%u \n",*fromX,*fromY,*toX,*toY);
                score=getScoreForMove(*fromX,*fromY,*toX,*toY);
                addMoveToList(&list,*fromX,*fromY,*toX,*toY,score);
             } else
        //---------------------------------------------------------------------------
        if (
             (x+1<limitX)&&(y+2<limitY)&&
             (table[x+1][y]==table[x][y+1])&&
             (table[x+1][y]==table[x+1][y+2])
           )
             { /* A X
                  X B
                  C X */
                *fromX = x; *fromY = y+1; *toX = x+1; *toY = y+1;
                fprintf(stderr,"HSolution %u,%u -> %u,%u \n",*fromX,*fromY,*toX,*toY);
                score=getScoreForMove(*fromX,*fromY,*toX,*toY);
                addMoveToList(&list,*fromX,*fromY,*toX,*toY,score);
             } else
        //---------------------------------------------------------------------------
        if (
             (x+1<limitX)&&(y+2<limitY)&&
             (table[x+1][y]==table[x+1][y+1])&&
             (table[x+1][y]==table[x][y+2])
           )
             { /* A X
                  B X
                  X C */
                *fromX = x; *fromY = y+2; *toX = x+1; *toY = y+2;
                fprintf(stderr,"ISolution %u,%u -> %u,%u \n",*fromX,*fromY,*toX,*toY);
                score=getScoreForMove(*fromX,*fromY,*toX,*toY);
                addMoveToList(&list,*fromX,*fromY,*toX,*toY,score);
             } else
        //---------------------------------------------------------------------------
        if (
             (x+1<limitX)&&(y+2<limitY)&&
             (table[x][y]==table[x][y+1])&&
             (table[x][y]==table[x+1][y+2])
           )
             { /* X A
                  X B
                  C X */
                *fromX = x+1; *fromY = y+2; *toX = x; *toY = y+2;
                fprintf(stderr,"JSolution %u,%u -> %u,%u \n",*fromX,*fromY,*toX,*toY);
                score=getScoreForMove(*fromX,*fromY,*toX,*toY);
                addMoveToList(&list,*fromX,*fromY,*toX,*toY,score);
             } else
        //---------------------------------------------------------------------------
        if (
             (x<limitX)&&(y+3<limitY)&&
             (table[x][y]==table[x][y+1])&&
             (table[x][y]==table[x][y+3])
           )
             { /* X
                  X
                  A
                  X */
                *fromX = x; *fromY = y+3; *toX = x; *toY = y+2;
                fprintf(stderr,"KSolution %u,%u -> %u,%u \n",*fromX,*fromY,*toX,*toY);
                score=getScoreForMove(*fromX,*fromY,*toX,*toY);
                addMoveToList(&list,*fromX,*fromY,*toX,*toY,score);
             } else
        //---------------------------------------------------------------------------
        if (
             (x<limitX)&&(y+3<limitY)&&
             (table[x][y]==table[x][y+2])&&
             (table[x][y]==table[x][y+3])
           )
             { /* X
                  A
                  X
                  X */
                *fromX = x; *fromY = y; *toX = x; *toY = y+1;
                fprintf(stderr,"KSolution %u,%u -> %u,%u \n",*fromX,*fromY,*toX,*toY);
                score=getScoreForMove(*fromX,*fromY,*toX,*toY);
                addMoveToList(&list,*fromX,*fromY,*toX,*toY,score);
             } else
        //---------------------------------------------------------------------------
        if (
             (x+3<limitX)&&(y<limitY)&&
             (table[x][y]==table[x+2][y])&&
             (table[x][y]==table[x+3][y])
           )
             { /* X A X X */
                *fromX = x; *fromY = y; *toX = x+1; *toY = y;
                fprintf(stderr,"LSolution %u,%u -> %u,%u \n",*fromX,*fromY,*toX,*toY);
                score=getScoreForMove(*fromX,*fromY,*toX,*toY);
                addMoveToList(&list,*fromX,*fromY,*toX,*toY,score);
             } else
        //---------------------------------------------------------------------------
        if (
             (x+3<limitX)&&(y<limitY)&&
             (table[x][y]==table[x+1][y])&&
             (table[x][y]==table[x+3][y])
           )
             { /* X X A X */
                *fromX = x+3; *fromY = y; *toX = x+2; *toY = y;
                fprintf(stderr,"MSolution %u,%u -> %u,%u \n",*fromX,*fromY,*toX,*toY);
                score=getScoreForMove(*fromX,*fromY,*toX,*toY);
                addMoveToList(&list,*fromX,*fromY,*toX,*toY,score);
             }
        //---------------------------------------------------------------------------

     }
    }

    return pickBestMoveOfList(&list,fromX,fromY,toX,toY);

}



int executeClickAndClick( unsigned int fromPixelX,unsigned int fromPixelY , unsigned int toPixelX, unsigned int toPixelY)
{
  lastMoveX=toPixelX;
  lastMoveY=toPixelY;

  if (!allowMouseControl) { return 0; }

  if ( (fromPixelX==toPixelX)  && (fromPixelY==toPixelY) ) { fprintf(stderr,"Doing nothing\n"); usleep(1*1000*1000); return 0; }
  fprintf(stderr,"executeClickAndClick %u,%u -> %u,%u \n",fromPixelX,fromPixelY,toPixelX,toPixelY);

  char commandStr[512];
  sprintf(commandStr,"xdotool mousemove --sync %u %u click 1",fromPixelX,fromPixelY);
  int i = system(commandStr);

  fprintf(stderr,"Step1 Done \n");
  usleep(clickdelay);

  sprintf(commandStr,"xdotool mousemove --sync %u %u click 1",toPixelX,toPixelY);
  i = system(commandStr);
  fprintf(stderr,"Step2 Done \n");


  return 1;
}


int reloadScreen()
{
  if (!allowSnapshot) { return 0; }
  fprintf(stderr,"reloadScreen\n");
  char commandStr[512]={0};
  sprintf(commandStr,"xwd -root -out out.xwd && convert out.xwd screenshot.pnm");
  int i = system(commandStr);

  return (i==0);
}



void countdownDelay(int seconds)
{
    if (seconds==0) { return; } //No delay do nothing!
    int secCounter=seconds;

    for (secCounter=seconds; secCounter>0; secCounter--)
    {
      fprintf(stderr,"%u\n",secCounter);
      usleep(1000*1000); // Waiting a while for the glitch frames to pass

    }
    usleep(1000*1000); // Waiting a while for the glitch frames to pass
}




int main(int argc, char *argv[])
{

int i=0;
  for (i=0; i<argc; i++)
  {
    if (strcasecmp(argv[i],"-doNotMove")==0) { allowMouseControl=0; fprintf(stderr,"Deactivated mouse control\n"); } else
    if (strcasecmp(argv[i],"-doNotGrab")==0) { allowSnapshot=0; fprintf(stderr,"Deactivated screen grabbing \n"); } else
    if (strcasecmp(argv[i],"-clickDelay")==0) { clickdelay=atoi(argv[i+1])*1000; fprintf(stderr,"Set click delay to %u\n",clickdelay); } else
    if (strcasecmp(argv[i],"-Delay")==0) { delay=atoi(argv[i+1])*1000; fprintf(stderr,"Set delay to %u\n",delay); }
  }


    fprintf(stderr,"Please ready your window\n");
    countdownDelay(2);
    fprintf(stderr,"Starting Up\n");

    reloadScreen();

    struct Image * haystack = readImage("screenshot.pnm",PNM_CODEC,0);
    struct Image * needle = readImage("seek.pnm",PNM_CODEC,0);

    unsigned int resX = 0 , resY = 0;

    /*
    if (
         RGBfindImageInImage(
                             haystack->pixels , haystack->width , haystack->height ,
                             needle->pixels   , needle->width   , needle->height   ,
                             &resX ,
                             &resY
                            )
       )
    {
        fprintf(stderr,"Found Image in Image @ %u,%u \n",resX,resY);
    }*/

    unsigned char R ,  G  , B;
    unsigned int clientX = resX , clientY = resY + needle->height;
    unsigned int blockX = 48,  blockY = 48;
    unsigned int offsetX=0 , offsetY=0;


    clientX = 337; clientY = 382;
    blockX = 40;  blockY = 40;
    offsetX=0; offsetY=3;

    unsigned int halfBlockX = blockX/2,  halfBlockY = blockY/2;

    unsigned int threshold = 30;
    unsigned int table[8][8]={0};
    unsigned int x,y=0;

    unsigned int iterations=0;
    while (1)
    {
    fprintf(stderr,"Round %u \n",iterations); ++iterations;
    for (y=0; y<8; y++)
    {
     for (x=0; x<8; x++)
     {
       getRGBPixel(haystack->pixels,haystack->width,haystack->height, clientX + offsetX + x*blockX + halfBlockX , clientY + offsetY + y*blockY + halfBlockY , &R , &G , &B );
       fprintf(stderr,"Pos(%u,%u) = %u , %u , %u \n",x,y,R,G,B);
       /*
       if ( closeToRGB(R,G,B, 0, 166 , 0 ,threshold)     )      { table[x][y]=GREEN_PIECE; }  else
       if ( closeToRGB(R,G,B, 200, 200 , 200 ,threshold) )      { table[x][y]=WHITE_PIECE; }  else
       if ( closeToRGB(R,G,B, 175, 63, 5 ,threshold)     )      { table[x][y]=ORANGE_PIECE; } else
       if ( closeToRGB(R,G,B, 145, 145, 3 ,threshold)    )      { table[x][y]=YELLOW_PIECE; } else
       if ( closeToRGB(R,G,B, 0, 189, 249 ,threshold)    )      { table[x][y]=BLUE_PIECE; } else
       if ( closeToRGB(R,G,B, 213, 95, 115 ,threshold)     )      { table[x][y]=RED_PIECE; } else
       if ( closeToRGB(R,G,B, 175, 0, 174 ,threshold)    )      { table[x][y]=PINK_PIECE; }
      */

       if ( closeToRGB(R,G,B, 24, 176 , 44 ,threshold)     )      { table[x][y]=GREEN_PIECE; }  else
       if ( closeToRGB(R,G,B, 238, 238 , 238 ,threshold) )       { table[x][y]=WHITE_PIECE; }  else
       if ( closeToRGB(R,G,B, 216, 79, 24 ,threshold)     )      { table[x][y]=ORANGE_PIECE; } else
       if ( closeToRGB(R,G,B, 254, 254, 39 ,threshold)    )      { table[x][y]=YELLOW_PIECE; } else
       if ( closeToRGB(R,G,B, 13, 139, 254 ,threshold)    )      { table[x][y]=BLUE_PIECE; } else
       if ( closeToRGB(R,G,B, 254, 32, 63 ,threshold)     )      { table[x][y]=RED_PIECE; } else
       if ( closeToRGB(R,G,B, 254, 17, 254,threshold)    )       { table[x][y]=PINK_PIECE; }

     }
    }

    //We now have a valid table!
    fprintf(stderr,"Table\n");
    fprintf(stderr,"----------------------------------\n");
     for (y=0; y<8; y++)
     {
       fprintf(stderr,"        |");
       for (x=0; x<8; x++)
       {
        writePieceChar(table[x][y]);
       }
       fprintf(stderr,"|\n");
     }
    fprintf(stderr,"----------------------------------\n");

    unsigned int fromX, fromY , toX, toY;
    suggestMove(table , &fromX, &fromY , &toX, &toY );

    fromX = clientX + halfBlockX + (fromX*blockX);
    fromY = clientY + halfBlockY + (fromY*blockY);
    toX   = clientX + halfBlockX + (toX*blockX);
    toY   = clientY + halfBlockY + (toY*blockY);

    fprintf(stderr,"Moving From %u,%u to %u,%u ", fromX , fromY , toX , toY );

    executeClickAndClick( fromX , fromY , toX , toY  );

    usleep(delay);
    reloadScreen();

    destroyImage(haystack);
    haystack = readImage("screenshot.pnm",PNM_CODEC,0);

    }

    destroyImage(needle);
    destroyImage(haystack);
    return 0;
}
