#ifndef FINDSUBIMAGE_H_INCLUDED
#define FINDSUBIMAGE_H_INCLUDED

int RGBfindImageInImage(
                        unsigned char * haystack , unsigned int haystackWidth , unsigned int haystackHeight ,
                        unsigned char * needle   , unsigned int needleWidth   , unsigned int needleHeight   ,
                        unsigned int * resX ,
                        unsigned int * resY ,
                        unsigned int acceptableScore,
                        unsigned int maxScore
                       );

#endif // FINDSUBIMAGE_H_INCLUDED
