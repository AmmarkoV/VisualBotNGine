#include <stdio.h>
#include <stdlib.h>

#include "Gweled.h"
#include "logic.h"
#include "vision.h"

#define PLAY_X_BEST_SOLUTIONS 3



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
    case HYPERCUBE_PIECE : fprintf(stderr,MAGENTA " #" NORMAL);  break;
    case EMPTY_PIECE : fprintf(stderr,MAGENTA " ." NORMAL);  break;
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





int formulatePlan(unsigned int table[8][8] , struct solutionList * list , struct mouseMovements * ourPlan)
{
  unsigned int halfBlockX = (unsigned int) settings.blockX/2;
  unsigned int halfBlockY = (unsigned int) settings.blockY/2;

  ourPlan->totalMovements=0;
  if (list->currentSolutions==0) { return 0; }

  //Keep only X best first solutions
  if (list->currentSolutions>PLAY_X_BEST_SOLUTIONS) { list->currentSolutions=PLAY_X_BEST_SOLUTIONS; }

 fprintf(stderr,"Old LastMove was %u,%u  / ",lastMoveX,lastMoveY);

  unsigned int i=0;
  unsigned int mC = 0;
  for (i=0; i<list->currentSolutions; i++)
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
           ourPlan->movement[mC].mode = MOVE_CLICK_AND_MOVE_CLICK;

           lastMoveX=list->solution[i].toX;
           lastMoveY=list->solution[i].toY;
           ++mC;
           ++ourPlan->totalMovements;
        } else
        { fprintf(stderr,"Incorrect solution item\n"); }
  }

  fprintf(stderr,"New LastMove is %u,%u \n",lastMoveX,lastMoveY );

  return ourPlan->totalMovements;
}


int thinkWhatToPlay(unsigned char * screen , unsigned int screenWidth ,unsigned int screenHeight ,
                    unsigned int clientStartX , unsigned int clientStartY ,
                    struct mouseMovements * ourPlan )
{
    unsigned int resX = clientStartX , resY = clientStartY;
    //settings.clientX = clientStartX;
    //settings.clientY = clientStartY;


    unsigned int table[8][8]={0};

    //Perform vision to find out table values and populate table array
    seeTable(table , screen ,  screenWidth , screenHeight , clientStartX ,  clientStartY);

    //We now have a valid table , lets print it to the console!
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

    return initVision();
}



int stopEngine()
{
    stopVision();
}
