#ifndef GWELED_H_INCLUDED
#define GWELED_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif


#include "../MouseMovementDescriptor.h"


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


struct gweledSettings
{
  unsigned int clientX,clientY;
  unsigned int blockX,blockY;
  unsigned int offsetX,offsetY;


  unsigned int threshold;
};


extern struct gweledSettings settings;


int thinkWhatToPlay(unsigned char * screen , unsigned int screenWidth ,unsigned int screenHeight ,
                    unsigned int clientStartX , unsigned int clientStartY ,
                    struct mouseMovements * ourPlan
                    );


int initializeEngine(char * settingsStr);

#ifdef __cplusplus
}
#endif

#endif // GWELED_H_INCLUDED
