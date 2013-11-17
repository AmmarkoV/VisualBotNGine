#include <stdio.h>
#include <stdlib.h>

#include "../Codecs/codecs.h"
#include "../ImageOperations/patchComparison.h"
#include "../ImageOperations/ocr.h"

#define MAX_OCR_STRING 512

struct PatternSet scoreOCR;
signed int scoreOCRX=213-337,scoreOCRY=378-382;
unsigned int scoreOCRSize=15;

int main(int argc, char *argv[])
{
    if (argc<1) { fprintf(stderr,"Please supply arguments image for OCR\n"); exit(1); }

    struct Image * img1 = readImage( argv[1] , PNM_CODEC , 0 );
    if (img1==0) { fprintf(stderr,"Could not read image 1\n"); return 1; }


    scoreOCR.totalPatterns=0;
    addToPatternSet(&scoreOCR,"../Engines/Gweled/OCR/score0_",0+'0',500);
    addToPatternSet(&scoreOCR,"../Engines/Gweled/OCR/score1_",0+'1',500);
    addToPatternSet(&scoreOCR,"../Engines/Gweled/OCR/score2_",0+'2',500);
    addToPatternSet(&scoreOCR,"../Engines/Gweled/OCR/score3_",0+'3',500);
    addToPatternSet(&scoreOCR,"../Engines/Gweled/OCR/score4_",0+'4',500);
    addToPatternSet(&scoreOCR,"../Engines/Gweled/OCR/score5_",0+'5',500);
    addToPatternSet(&scoreOCR,"../Engines/Gweled/OCR/score6_",0+'6',500);
    addToPatternSet(&scoreOCR,"../Engines/Gweled/OCR/score7_",0+'7',500);
    addToPatternSet(&scoreOCR,"../Engines/Gweled/OCR/score8_",0+'8',500);
    addToPatternSet(&scoreOCR,"../Engines/Gweled/OCR/score9_",0+'9',500);
    addToPatternSet(&scoreOCR,"../Engines/Gweled/OCR/scoreComma_",0+',',500);



   char strOutput[MAX_OCR_STRING]={0};

   if (
        doOCR(
              img1->pixels, img1->width , img1->height ,
              13 , 0  , img1->width-13 , img1->height  ,
              &scoreOCR ,

              strOutput ,
              MAX_OCR_STRING
             )
       )
        { fprintf(stderr,"OCR String is %s \n",strOutput); } else
        { fprintf(stderr,"Could not read an OCR String \n"); }

    return 0;
}
