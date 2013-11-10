#include <stdio.h>
#include <stdlib.h>

#include "Gweled.h"



#define MAXSOLUTIONS 256

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
  struct solutionItem solution[MAXSOLUTIONS+1];
  unsigned int currentSolutions;
};


unsigned int lastMoveX=4,lastMoveY=2;


struct gweledSettings settings={0};


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

int printTable(unsigned int table[8][8])
{
        //We now have a valid table!
    fprintf(stderr,"Table\n");
    fprintf(stderr,"----------------------------------\n");
    unsigned int x,y;
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

}




int isSceneTooAmbiguous(unsigned int table[8][8])
{
  unsigned int ambiguous=0;
  unsigned int x,y;
  for (y=0; y<8; y++)
  {
    for (x=0; x<8; x++)
    {
       if (table[x][y]==0) { ++ambiguous; }
    }
  }

  if (ambiguous>12) { return 1;}
  return 0;
}

int addMoveToList(struct solutionList * list ,  unsigned int fromX,unsigned int fromY , unsigned int toX, unsigned int toY , unsigned int score)
{
   if (MAXSOLUTIONS<=list->currentSolutions) { fprintf(stderr,"No more space for solutions\n"); return 0; }
   list->solution[list->currentSolutions].fromX=fromX;
   list->solution[list->currentSolutions].fromY=fromY;
   list->solution[list->currentSolutions].toX=toX;
   list->solution[list->currentSolutions].toY=toY;
   list->solution[list->currentSolutions].score=score;

  //fprintf(stderr,"addMoveToList[%u] (%u,%u,%u,%u) ==  %u \n",list->currentSolutions,fromX,fromY,toX,toY,score);

   ++list->currentSolutions;

   return 1;
}







unsigned int getScoreForMove(  unsigned int fromX,unsigned int fromY  , unsigned int toX, unsigned int toY )
{
  //fprintf(stderr,"lastMoves(%u,%u) \n",lastMoveX,lastMoveY);
  unsigned int a =  ABSDIFF(toX,lastMoveX);
  unsigned int b =  ABSDIFF(toY,lastMoveY);
  unsigned int score = a*a +b*b+(unsigned int) toY/2;

  //fprintf(stderr,"getScoreForMove(%u,%u,%u,%u) ==  %u ( a = %u , b = %u ) \n",fromX,fromY,toX,toY,score,a,b);
  return (unsigned int) score;
}


/* qsort struct comparision function  */
int compareSolutions(const void *a, const void *b)
{
    struct solutionItem *ia = (struct solutionItem *)a;
    struct solutionItem *ib = (struct solutionItem *)b;

    if ( ia->score  <  ib->score ) return -1;
    if ( ia->score ==  ib->score ) return 0;
    if ( ia->score  >  ib->score ) return 1;
    return 0;
}



int getValidMoves(unsigned int table[8][8] , struct solutionList * list)
{
    unsigned int fromX=0 , fromY=0;
    unsigned int toX=0 ,toY = 0;

    unsigned int limitX=8;
    unsigned int limitY=8;

    unsigned int score =0;
    unsigned int x,y=0;
    for (y=0; y<8; y++)
    {
     for (x=0; x<8; x++)
     {
        //if (table[x][y]==0) {} else
        //---------------------------------------------------------------------------
        if (
             (x+2<limitX)&&(y+1<limitY)&&
             (table[x][y]==table[x+1][y])&&
             (table[x][y]==table[x+2][y+1])
           )
             { /* X X A
                  B C X */
                fromX = x+2; fromY = y+1; toX = x+2; toY = y;
                fprintf(stderr,"ASolution %u,%u -> %u,%u \n",fromX,fromY,toX,toY);
                score=getScoreForMove(fromX,fromY,toX,toY);
                addMoveToList(list,fromX,fromY,toX,toY,score);
             } else
        //---------------------------------------------------------------------------
        if (
             (x+2<limitX)&&(y+1<limitY)&&
             (table[x][y+1]==table[x+1][y+1])&&
             (table[x][y+1]==table[x+2][y])
           )
             { /* A B X
                  X X C */
                fromX = x+2; fromY = y; toX = x+2; toY = y+1;
                fprintf(stderr,"BSolution %u,%u -> %u,%u \n",fromX,fromY,toX,toY);
                score=getScoreForMove(fromX,fromY,toX,toY);
                addMoveToList(list,fromX,fromY,toX,toY,score);
             } else
        //---------------------------------------------------------------------------
        if (
             (x+2<limitX)&&(y+1<limitY)&&
             (table[x+1][y]==table[x+2][y])&&
             (table[x][y+1]==table[x+2][y])
           )
             { /* A X X
                  X B C */
                fromX = x; fromY = y+1; toX = x; toY = y;
                fprintf(stderr,"CSolution %u,%u -> %u,%u \n",fromX,fromY,toX,toY);
                score=getScoreForMove(fromX,fromY,toX,toY);
                addMoveToList(list,fromX,fromY,toX,toY,score);
             } else
        //---------------------------------------------------------------------------
        if (
             (x+2<limitX)&&(y+1<limitY)&&
             (table[x][y]==table[x+1][y+1])&&
             (table[x][y]==table[x+2][y+1])
           )
             { /* X B C
                  A X X */
                fromX = x; fromY = y; toX = x; toY = y+1;
                fprintf(stderr,"DSolution %u,%u -> %u,%u \n",fromX,fromY,toX,toY);
                score=getScoreForMove(fromX,fromY,toX,toY);
                addMoveToList(list,fromX,fromY,toX,toY,score);
             } else
        //---------------------------------------------------------------------------
        if (
             (x+2<limitX)&&(y+1<limitY)&&
             (table[x][y+1]==table[x+1][y])&&
             (table[x][y+1]==table[x+2][y+1])
           )
             { /* A X C
                  X B X */
                fromX = x+1; fromY = y; toX = x+1; toY = y+1;
                fprintf(stderr,"D2Solution %u,%u -> %u,%u \n",fromX,fromY,toX,toY);
                score=getScoreForMove(fromX,fromY,toX,toY);
                addMoveToList(list,fromX,fromY,toX,toY,score);
             } else
        //---------------------------------------------------------------------------
        if (
             (x+2<limitX)&&(y+1<limitY)&&
             (table[x][y]==table[x+1][y+1])&&
             (table[x][y]==table[x+2][y])
           )
             { /* X B X
                  A X C */
                fromX = x+1; fromY = y+1; toX = x+1; toY = y;
                fprintf(stderr,"D3Solution %u,%u -> %u,%u \n",fromX,fromY,toX,toY);
                score=getScoreForMove(fromX,fromY,toX,toY);
                addMoveToList(list,fromX,fromY,toX,toY,score);
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
                fromX = x+1; fromY = y; toX = x; toY = y;
                fprintf(stderr,"ESolution %u,%u -> %u,%u \n",fromX,fromY,toX,toY);
                score=getScoreForMove(fromX,fromY,toX,toY);
                addMoveToList(list,fromX,fromY,toX,toY,score);
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
                fromX = x; fromY = y; toX = x+1; toY = y;
                fprintf(stderr,"FSolution %u,%u -> %u,%u \n",fromX,fromY,toX,toY);
                score=getScoreForMove(fromX,fromY,toX,toY);
                addMoveToList(list,fromX,fromY,toX,toY,score);
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
                fromX = x+1; fromY = y+1; toX = x; toY = y+1;
                fprintf(stderr,"GSolution %u,%u -> %u,%u \n",fromX,fromY,toX,toY);
                score=getScoreForMove(fromX,fromY,toX,toY);
                addMoveToList(list,fromX,fromY,toX,toY,score);
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
                fromX = x; fromY = y+1; toX = x+1; toY = y+1;
                fprintf(stderr,"HSolution %u,%u -> %u,%u \n",fromX,fromY,toX,toY);
                score=getScoreForMove(fromX,fromY,toX,toY);
                addMoveToList(list,fromX,fromY,toX,toY,score);
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
                fromX = x; fromY = y+2; toX = x+1; toY = y+2;
                fprintf(stderr,"ISolution %u,%u -> %u,%u \n",fromX,fromY,toX,toY);
                score=getScoreForMove(fromX,fromY,toX,toY);
                addMoveToList(list,fromX,fromY,toX,toY,score);
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
                fromX = x+1; fromY = y+2; toX = x; toY = y+2;
                fprintf(stderr,"JSolution %u,%u -> %u,%u \n",fromX,fromY,toX,toY);
                score=getScoreForMove(fromX,fromY,toX,toY);
                addMoveToList(list,fromX,fromY,toX,toY,score);
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
                fromX = x; fromY = y+3; toX = x; toY = y+2;
                fprintf(stderr,"KSolution %u,%u -> %u,%u \n",fromX,fromY,toX,toY);
                score=getScoreForMove(fromX,fromY,toX,toY);
                addMoveToList(list,fromX,fromY,toX,toY,score);
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
                fromX = x; fromY = y; toX = x; toY = y+1;
                fprintf(stderr,"KSolution %u,%u -> %u,%u \n",fromX,fromY,toX,toY);
                score=getScoreForMove(fromX,fromY,toX,toY);
                addMoveToList(list,fromX,fromY,toX,toY,score);
             } else
        //---------------------------------------------------------------------------
        if (
             (x+3<limitX)&&(y<limitY)&&
             (table[x][y]==table[x+2][y])&&
             (table[x][y]==table[x+3][y])
           )
             { /* X A X X */
                fromX = x; fromY = y; toX = x+1; toY = y;
                fprintf(stderr,"LSolution %u,%u -> %u,%u \n",fromX,fromY,toX,toY);
                score=getScoreForMove(fromX,fromY,toX,toY);
                addMoveToList(list,fromX,fromY,toX,toY,score);
             } else
        //---------------------------------------------------------------------------
        if (
             (x+3<limitX)&&(y<limitY)&&
             (table[x][y]==table[x+1][y])&&
             (table[x][y]==table[x+3][y])
           )
             { /* X X A X */
                fromX = x+3; fromY = y; toX = x+2; toY = y;
                fprintf(stderr,"MSolution %u,%u -> %u,%u \n",fromX,fromY,toX,toY);
                score=getScoreForMove(fromX,fromY,toX,toY);
                addMoveToList(list,fromX,fromY,toX,toY,score);
             }
        //---------------------------------------------------------------------------

     }
    }



   if (list->currentSolutions>1)
   {
     //QuickSort solutions
     qsort( list->solution , list->currentSolutions , sizeof(struct solutionItem), compareSolutions);
   }


   //We have a sorted solution list
   return 1;
}



int formulatePlan(unsigned int table[8][8] , struct solutionList * list , struct mouseMovements * ourPlan)
{
  unsigned int halfBlockX = (unsigned int) settings.blockX/2;
  unsigned int halfBlockY = (unsigned int) settings.blockY/2;


  ourPlan->totalMovements=0;
  if (list->currentSolutions==0) { return 0; }

  unsigned int bestScore=0;
  unsigned int i=0;
  unsigned int mC = 0;
  for (i=0; i<list->currentSolutions; i++)
  {
     //fprintf(stderr,"Move %u  score %u\n",i,list->solution[i].score );
     if (list->solution[i].score >= bestScore)
     {
        if (
            (list->solution[i].fromX<8) &&
            (list->solution[i].fromY<8) &&
            (list->solution[i].toX<8) &&
            (list->solution[i].toY<8)
            )
        {
           ourPlan->movement[mC].fromX = settings.clientX + halfBlockX + ( list->solution[i].fromX * settings.blockX);
           ourPlan->movement[mC].fromY = settings.clientY + halfBlockY + ( list->solution[i].fromY * settings.blockY);
           ourPlan->movement[mC].toX =   settings.clientX + halfBlockX + ( list->solution[i].toX * settings.blockY);
           ourPlan->movement[mC].toY =   settings.clientY + halfBlockY + ( list->solution[i].toY * settings.blockY);
           ++mC;
           ++ourPlan->totalMovements;
        } else
        { fprintf(stderr,"Incorrect solution item\n"); }

     }
  }

  fprintf(stderr,"LastMove was %u,%u  , now we go %u,%u -> %u,%u ( score %u ) \n",lastMoveX,lastMoveY,
          ourPlan->movement[mC].fromX , ourPlan->movement[mC].fromY ,
          ourPlan->movement[mC].toX , ourPlan->movement[mC].toY ,
          bestScore
          );
  lastMoveX=ourPlan->movement[mC].toX;
  lastMoveY=ourPlan->movement[mC].toY;

  return ourPlan->totalMovements;
}


int thinkWhatToPlay(unsigned char * screen , unsigned int screenWidth ,unsigned int screenHeight ,
                    unsigned int clientStartX , unsigned int clientStartY ,
                    struct mouseMovements * ourPlan )
{
    unsigned int resX = clientStartX , resY = clientStartY;


    unsigned char R ,  G  , B;
    unsigned int clientX = resX , clientY = resY;
    unsigned int blockX = 48,  blockY = 48;
    unsigned int offsetX=0 , offsetY=0;


    clientX = 337; clientY = 382;
    blockX = 40;  blockY = 40;
    offsetX=0; offsetY=3;

    unsigned int threshold=29;
    unsigned int halfBlockX = (unsigned int) blockX/2;
    unsigned int halfBlockY = (unsigned int) blockY/2;
    unsigned int x,y;

    unsigned int table[8][8]={0};

    //Perform vision to find out table values and populate table array
    seeTable(table , screen ,  screenWidth , screenHeight , clientStartX ,  clientStartY);

    //We now have a valid table!
    printTable(table);


    if ( isSceneTooAmbiguous(table) )
       {
         fprintf(stderr,"Scene is too ambiguous :(\nWaiting..\n");
         return 0;
       }


    struct solutionList list={0};
    list.currentSolutions=0;

    getValidMoves(table , &list );

    unsigned int totalSolutions = formulatePlan(table , &list ,ourPlan);


    return totalSolutions;
}


int initializeEngine(char * settingsStr)
{
    if ( strcasecmp(settingsStr,"bejeweled blitz")==0 )
    {
      settings.clientX = 337;
      settings.clientY = 382;
      settings.blockX = 40;
      settings.blockY = 40;
      settings.offsetX = 0;
      settings.offsetY = 3;
      settings.threshold = 29;
    } else
    if ( strcasecmp(settingsStr,"gweled")==0 )
    {
      settings.clientX = 337;
      settings.clientY = 382;
      settings.blockX = 48;
      settings.blockY = 48;
      settings.offsetX = 0;
      settings.offsetY = 3;
      settings.threshold = 29;
    }

    return 0;
}
