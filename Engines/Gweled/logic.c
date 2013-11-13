
#include <stdio.h>
#include <stdlib.h>
#include "Gweled.h"
#include "logic.h"

#define TOO_AMBIGOUS_AFTER_X_UNKNOWN_PIECES 15

unsigned int lastMoveX=4,lastMoveY=2;


#define ABSDIFF(num1,num2) ( (num1-num2) >=0 ? (num1-num2) : (num2 - num1) )





int isSceneTooAmbiguous(unsigned int table[8][8])
{
  unsigned int ambiguous=0;
  unsigned int x,y;
  for (y=0; y<8; y++)
  {
    for (x=0; x<8; x++)
    {
       if (table[x][y]==UNKNOWN_PIECE) { ++ambiguous; }
    }
  }

  if (ambiguous>TOO_AMBIGOUS_AFTER_X_UNKNOWN_PIECES) { return 1;}
  return 0;
}


int mostPopularPiece(unsigned int table[8][8])
{
  unsigned int instancesOfPiece[NUMBER_OF_VALID_PIECES]={0};

  unsigned int x,y;
  for (y=0; y<8; y++)
  {
    for (x=0; x<8; x++)
    {
       if (table[x][y]<NUMBER_OF_VALID_PIECES)
         {
           ++instancesOfPiece [ table[x][y] ];
         }
    }
  }

  unsigned int mostPopularScore = 0;
  unsigned int mostPopularPiece = 0;
   for (x=0; x<NUMBER_OF_VALID_PIECES; x++)
    {
      if (mostPopularScore<instancesOfPiece[x])
      {
          mostPopularScore=instancesOfPiece[x];
          mostPopularPiece=x;
      }

    }

  return mostPopularPiece;
}


unsigned int getFirstPieceOfType(unsigned int table[8][8] , unsigned int pieceType , unsigned int * posX, unsigned int * posY)
{
  unsigned int x,y;
  for (y=0; y<8; y++)
  {
    for (x=0; x<8; x++)
    {
       if (table[x][y]==pieceType)
         {
           *posX = x;
           *posY = y;
           return 1;
         }
    }
  }
 return 0;
}

unsigned int getInstancesOfPiece(unsigned int table[8][8],unsigned int pieceKind)
{
  unsigned int instancesOfPiece=0;

  unsigned int x,y;
  for (y=0; y<8; y++)
  {
    for (x=0; x<8; x++)
    {
       if (table[x][y]==pieceKind)
         {
           ++instancesOfPiece;
         }
    }
  }


  return instancesOfPiece;
}



unsigned int getMostPopularNeighboringPiece(unsigned int table[8][8] ,  unsigned int * posX, unsigned int * posY)
{

  unsigned int cX,cY,sX,sY,eX,eY;
  cX=*posX; cY=*posY;
  sX=*posX;   if (sX > 0) { sX-=1; }
  sY=*posY;   if (sY > 0) { sY-=1; }
  eX=*posX;   if (eX > 0) { eX+=1; }
  eY=*posY;   if (eY > 0) { eY+=1; }

  unsigned int foundSomething=0;
  unsigned int bestScore = 0;
  unsigned int currentScore = 0;
  unsigned int x,y;
  for (y=sY; y<eY; y++)
  {
    for (x=sX; x<eX; x++)
    {
       if (table[x][y]!=UNKNOWN_PIECE)
         {
           currentScore =getInstancesOfPiece(table,table[x][y]);
           if (currentScore>=bestScore) { bestScore = currentScore; *posX=x; *posY=y; foundSomething=1;}
         }
    }
  }
 return foundSomething;
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

        if (table[x][y]==HYPERCUBE_PIECE)
            {
                fprintf(stderr,"Handling Hypercube @ %u,%u \n",x,y);
                fromX = x; fromY = y; //First click is hypercube
                toX=x;  toY=y;
                getMostPopularNeighboringPiece(table,&toX,&toY);
                score=100; //HyperCubes are nice , they have a good score
                addMoveToList(list,fromX,fromY,toX,toY,score);
            } else
        //---------------------------------------------------------------------------
        if (
             (x+2<limitX)&&(y+1<limitY)&&
             (table[x][y]!=NO_PIECE)&&
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
             (table[x][y+1]!=NO_PIECE)&&
             (table[x][y+1]==table[x+1][y+1])&&
             (table[x][y+1]==table[x+2][y])
           )
             { /* A B X
                  X X C */
                fromX = x+2; fromY = y; toX = x+2; toY = y+1;
                fprintf(stderr,"BSolution (%u) %u,%u -> %u,%u \n",table[x][y+1],fromX,fromY,toX,toY);
                score=getScoreForMove(fromX,fromY,toX,toY);
                addMoveToList(list,fromX,fromY,toX,toY,score);
             } else
        //---------------------------------------------------------------------------
        if (
             (x+2<limitX)&&(y+1<limitY)&&
             (table[x][y+1]!=NO_PIECE)&&
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
             (table[x][y]!=NO_PIECE)&&
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
             (table[x][y+1]!=NO_PIECE)&&
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
             (table[x][y]!=NO_PIECE)&&
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
             (table[x+1][y]!=NO_PIECE)&&
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
             (table[x][y]!=NO_PIECE)&&
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
             (table[x][y]!=NO_PIECE)&&
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
             (table[x+1][y]!=NO_PIECE)&&
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
             (table[x+1][y]!=NO_PIECE)&&
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
             (table[x][y]!=NO_PIECE)&&
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
             (table[x][y]!=NO_PIECE)&&
             (table[x][y]==table[x][y+1])&&
             (table[x][y]==table[x][y+3])
           )
             { /* X
                  X
                  A
                  X */
                fromX = x; fromY = y+3; toX = x; toY = y+2;
                fprintf(stderr,"KSolution %u,%u -> %u,%u \n",fromX,fromY,toX,toY);
                score=20+getScoreForMove(fromX,fromY,toX,toY);
                addMoveToList(list,fromX,fromY,toX,toY,score);
             } else
        //---------------------------------------------------------------------------
        if (
             (x<limitX)&&(y+3<limitY)&&
             (table[x][y]!=NO_PIECE)&&
             (table[x][y]==table[x][y+2])&&
             (table[x][y]==table[x][y+3])
           )
             { /* X
                  A
                  X
                  X */
                fromX = x; fromY = y; toX = x; toY = y+1;
                fprintf(stderr,"KSolution %u,%u -> %u,%u \n",fromX,fromY,toX,toY);
                score=20+getScoreForMove(fromX,fromY,toX,toY);
                addMoveToList(list,fromX,fromY,toX,toY,score);
             } else
        //---------------------------------------------------------------------------
        if (
             (x+3<limitX)&&(y<limitY)&&
             (table[x][y]!=NO_PIECE)&&
             (table[x][y]==table[x+2][y])&&
             (table[x][y]==table[x+3][y])
           )
             { /* X A X X */
                fromX = x; fromY = y; toX = x+1; toY = y;
                fprintf(stderr,"LSolution %u,%u -> %u,%u \n",fromX,fromY,toX,toY);
                score=20+getScoreForMove(fromX,fromY,toX,toY);
                addMoveToList(list,fromX,fromY,toX,toY,score);
             } else
        //---------------------------------------------------------------------------
        if (
             (x+3<limitX)&&(y<limitY)&&
             (table[x][y]!=NO_PIECE)&&
             (table[x][y]==table[x+1][y])&&
             (table[x][y]==table[x+3][y])
           )
             { /* X X A X */
                fromX = x+3; fromY = y; toX = x+2; toY = y;
                fprintf(stderr,"MSolution %u,%u -> %u,%u \n",fromX,fromY,toX,toY);
                score=20+getScoreForMove(fromX,fromY,toX,toY);
                addMoveToList(list,fromX,fromY,toX,toY,score);
             }
        //---------------------------------------------------------------------------


              /* Hypercube creation */
              else

        if (
             (x+4<limitX)&&(y+1<limitY)&&
             (table[x][y+1]!=NO_PIECE)&&

             (table[x][y+1]==table[x+1][y+1])&&
             (table[x][y+1]==table[x+2][y])&&
             (table[x][y+1]==table[x+3][y+1])&&
             (table[x][y+1]==table[x+4][y+1])
           )
             {  // A B X D E
                // X X C X X
                fromX = x+2; fromY = y; toX = x+2; toY = y+1;
                fprintf(stderr,"DoHypercubeUp %u,%u -> %u,%u \n",fromX,fromY,toX,toY);
                score=200;
                addMoveToList(list,fromX,fromY,toX,toY,score);
             }
             else

        if (
             (x+4<limitX)&&(y+1<limitY)&&
             (table[x][y]!=NO_PIECE)&&

             (table[x][y]==table[x+1][y])&&
             (table[x][y]==table[x+2][y+1])&&
             (table[x][y]==table[x+3][y])&&
             (table[x][y]==table[x+4][y])
           )
             {  // X X C X X
                // A B X D E
                fromX = x+2; fromY = y+1; toX = x+2; toY = y;
                fprintf(stderr,"DoHypercubeUp %u,%u -> %u,%u \n",fromX,fromY,toX,toY);
                score=200;
                addMoveToList(list,fromX,fromY,toX,toY,score);
             }











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

