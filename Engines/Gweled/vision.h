#ifndef VISION_H_INCLUDED
#define VISION_H_INCLUDED

extern unsigned int singlePixelAssignments;
extern unsigned int patternAssignments;

extern unsigned int singlePixelAssignmentsTotal;
extern unsigned int patternAssignmentsTotal;

int seeTable(unsigned int table[8][8] ,
             unsigned char * screen , unsigned int screenWidth ,unsigned int screenHeight ,
             unsigned int clientStartX , unsigned int clientStartY);

int seeButtons( unsigned char * screen , unsigned int screenWidth ,unsigned int screenHeight , unsigned int * clickX , unsigned int * clickY );

int initVision();
int stopVision();

#endif // VISION_H_INCLUDED
