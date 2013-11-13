#include <stdio.h>
#include <stdlib.h>

#include "Gweled.h"
#include "logic.h"
#include "vision.h"

#define PLAY_X_BEST_SOLUTIONS 3

#define AUTOPLAY_AFTER_X_UNCERTAINTY 35
unsigned int continuousUncertainty=0;


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


char * getPieceName(int pieceVal)
{
  switch (pieceVal)
  {
    case GREEN_PIECE : return " GREEN_PIECE";   break;
    case WHITE_PIECE : return " WHITE_PIECE";   break;
    case ORANGE_PIECE : return " ORANGE_PIECE";   break;
    case YELLOW_PIECE : return " YELLOW_PIECE";   break;
    case BLUE_PIECE : return " BLUE_PIECE";   break;
    case RED_PIECE : return " RED_PIECE";   break;
    case PINK_PIECE : return " PINK_PIECE";   break;
    case HYPERCUBE_PIECE : return " HYPERCUBE_PIECE";   break;
    case EMPTY_PIECE : return " EMPTY_PIECE";   break;
    default :
     return " ?";
    break;
  }
  return "Nothing ";
}

int writePieceChar(int pieceVal)
{
  switch (pieceVal)
  {
    case GREEN_PIECE : printf(GREEN " G" NORMAL);  break;
    case WHITE_PIECE : printf(WHITE " W" NORMAL);  break;
    case ORANGE_PIECE : printf(YELLOW " O" NORMAL);  break;
    case YELLOW_PIECE : printf(YELLOW " Y" NORMAL);  break;
    case BLUE_PIECE : printf(CYAN " B" NORMAL);  break;
    case RED_PIECE : printf(RED " R" NORMAL);  break;
    case PINK_PIECE : printf(MAGENTA " P" NORMAL);  break;
    case HYPERCUBE_PIECE : printf(MAGENTA " #" NORMAL);  break;
    case EMPTY_PIECE : printf(MAGENTA " ." NORMAL);  break;
    default :
     printf(" ?" );
    break;
  }
  return 0;
}

int printTable(unsigned int table[8][8])
{
        //We now have a valid table!
    printf("Table\n");
    printf("----------------------------------\n");
    unsigned int x,y;
     for (y=0; y<8; y++)
     {
       printf("        |");
       for (x=0; x<8; x++)
       {
        writePieceChar(table[x][y]);
       }
       printf("|\n");
     }
    printf("----------------------------------\n");

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
     printf("Single Pixel Assignments : %u ( %u ) - Pattern Assignments : %u ( %u )\n",
                    singlePixelAssignments , singlePixelAssignmentsTotal ,
                    patternAssignments , patternAssignmentsTotal);


    if ( isSceneTooAmbiguous(table) )
       {
         ourPlan->totalMovements=0;
         ++continuousUncertainty;
         printf("Scene is too ambiguous :( ( %u / %u ) \n", continuousUncertainty , AUTOPLAY_AFTER_X_UNCERTAINTY);

         if (continuousUncertainty>AUTOPLAY_AFTER_X_UNCERTAINTY)
         {

           ourPlan->movement[1].mode = MOVE_TO_NEUTRAL;
           ++ourPlan->totalMovements;

           unsigned int clickX=0 , clickY=0;
           if ( seeButtons( screen ,  screenWidth , screenHeight  , &clickX , &clickY) )
              {
                 ourPlan->movement[ourPlan->totalMovements].fromX = clickX;
                 ourPlan->movement[ourPlan->totalMovements].fromY = clickY;
                 ourPlan->movement[ourPlan->totalMovements].toX =   clickX+1;
                 ourPlan->movement[ourPlan->totalMovements].toY =   clickY+1;
                 ourPlan->movement[ourPlan->totalMovements].mode = SINGLE_MOVE_CLICK;
                 ++ourPlan->totalMovements;

                 fprintf(stderr,"Grinding play , Found Button @ %u , %u \n",clickX,clickY);
                 return 1;
              }
         }


         return 0;
       }


    struct solutionList list={0};
    list.currentSolutions=0;

    getValidMoves(table , &list );

    unsigned int totalSolutions = formulatePlan(table , &list ,ourPlan);

    continuousUncertainty=0;
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
