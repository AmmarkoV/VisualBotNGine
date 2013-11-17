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

  while (loops<10)
  {
      if (
           compareToPatternSet(font ,
                               screen , screenWidth , screenHeight ,
                               x , y , width , height ,
                               OCR_MAX_SCORE,
                               &characterVal,
                               &patternNum,
                               &tileNum
                               )
          )

          {
             output[characterNum]=(unsigned char) characterVal;

             ++characterNum;
             fprintf(stderr,"Character %u looks like %u \n",characterNum,characterVal);
             x+=getPatternSetItemWidth(font,patternNum,tileNum);
          }

    ++loops ;
  }

  output[characterNum]=0;

 return 1;
}
