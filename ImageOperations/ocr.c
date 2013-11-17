#include "ocr.h"
#include <stdio.h>

#define OCR_MAX_SCORE 60000

int doOCR(
           unsigned char * screen , unsigned int screenWidth , unsigned int screenHeight ,
           unsigned int sX , unsigned int sY  , unsigned int width , unsigned int height,

           struct PatternSet * font ,

           char * output ,
           unsigned int outputMaxLength
          )

{
  unsigned int characterNum=0;
  unsigned int characterVal=0;
  unsigned int done = 0;
  unsigned int x=sX,y=sY;
  unsigned int loops =0 ;

  unsigned int patternNum , tileNum;

  unsigned int score = 0;

  while(x<width)
  {
   score = colorVariance( screen, screenWidth ,screenHeight , x,  y, 2 , height);
   if ( score < 200 )
   {
      x+=2;
     fprintf(stderr,"Block %u,%u -> %u,%u is empty ( %u ) \n",x,y,2,height,score);
   } else
   {
     fprintf(stderr,"Block %u,%u -> %u,%u is NOT empty ( %u ) \n",x,y,2,height,score);
     break;
   }
  }



  unsigned int remainingWidth  = width-x;
  while (loops<10)
  {
      if (
           compareToPatternSet(font ,
                               screen , screenWidth , screenHeight ,
                               x , y , remainingWidth , height ,
                               OCR_MAX_SCORE,
                               &characterVal,
                               &patternNum,
                               &tileNum
                               )
          )

          {
             output[characterNum]=(unsigned char) characterVal;

             ++characterNum;
             fprintf(stderr,"Character %u looks like %c \n",characterNum,characterVal);
             unsigned int xStep = getPatternSetItemWidth(font,patternNum,tileNum);
             x+=xStep;
             if (remainingWidth>xStep) { break; } else
                                       { remainingWidth-=xStep; }
          }

    ++loops ;
  }

  output[characterNum]=0;

 return 1;
}
