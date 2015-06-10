#include "findSubImage.h"
#include "patchComparison.h"
#include <stdio.h>



int RGBfindImageInImage(
                        unsigned char * haystack , unsigned int haystackWidth , unsigned int haystackHeight ,
                        unsigned char * needle   , unsigned int needleWidth   , unsigned int needleHeight   ,
                        unsigned int * resX ,
                        unsigned int * resY ,
                        unsigned int acceptableScore,
                        unsigned int maxScore
                       )
{
  unsigned int bestScore=10000000 , bestX=0 , bestY=0;
  unsigned int score=bestScore , x , y;

  for (y=1; y<haystackHeight-needleHeight-1; y++)
  {
    for (x=1; x<haystackWidth-needleWidth-1; x++)
    {
       if (
             compareRGBPatches( haystack , x ,  y , haystackWidth ,  haystackHeight,
                                needle   , 0 ,  0 , needleWidth   ,  needleHeight ,
                                needleWidth   ,  needleHeight
                                , &score , maxScore  )
           )
           {
              if ( score < bestScore) { bestScore = score;  bestX=x;  bestY = y;  }
              if (score <= acceptableScore ) { return bestScore+1; }
           }
    }
  }

  if (bestScore<=acceptableScore)
   {
    fprintf(stderr,"RGBfindImageInImage found an acceptable needle in the haystack\n");
    *resX = bestX;
    *resY = bestY;
    return bestScore+1;
   } else
   {
     fprintf(stderr,"RGBfindImageInImage unable to find needle in haystack");
   }


  return 0;
}
