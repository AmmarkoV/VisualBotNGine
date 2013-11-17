#ifndef VISION_H_INCLUDED
#define VISION_H_INCLUDED

extern signed int scoreOCRX ,scoreOCRY ;
extern unsigned int scoreOCRSize;

int seeTable(unsigned int table[8][8] ,
             unsigned char * screen , unsigned int screenWidth ,unsigned int screenHeight ,
             unsigned int clientStartX , unsigned int clientStartY);

int seeButtons( unsigned char * screen , unsigned int screenWidth ,unsigned int screenHeight , unsigned int * clickX , unsigned int * clickY );


int seeScore( unsigned char * screen , unsigned int screenWidth ,unsigned int screenHeight ,
              unsigned int x,unsigned int y , unsigned int width , unsigned int height ,
              char * strOutput , unsigned int strOutputMaxLength
            );

int initVision();
int stopVision();

#endif // VISION_H_INCLUDED
