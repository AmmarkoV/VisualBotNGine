#include <stdio.h>
#include <stdlib.h>

#include "../Codecs/codecs.h"
#include "../ImageOperations/patchComparison.h"



int main(int argc, char *argv[])
{
    if (argc<4) { fprintf(stderr,"Please supply arguments LargeImage largeImageX largeImageY smallImage\n"); exit(1); }
    char file1[1024];
    char file2[1024];


    struct Image * img1 = readImage( argv[1] , PNM_CODEC , 0 );
    struct Image * img2 = readImage( argv[4] , PNM_CODEC , 0 );

    if (img1==0) {fprintf(stderr,"Could not read image 1\n"); return 1; }
    if (img2==0) {fprintf(stderr,"Could not read image 2\n"); return 2; }

    unsigned int score=0;
    unsigned int failScore=40000;

    compareRGBPatchesIgnoreColor
                     ( img1->pixels , atoi(argv[2]) ,  atoi(argv[3]) , img1->width , img1->height ,
                       img2->pixels , 0 ,  0 , img2->width , img2->height ,
                       123 , 123 , 0 ,
                       img2->width  , img2->height ,
                       &score,
                       failScore
                     );

    fprintf(stderr,"Comparing %s @ %s,%s with %s yields %u\n",argv[1],argv[2],argv[3],argv[4],score);

    return 0;
}
