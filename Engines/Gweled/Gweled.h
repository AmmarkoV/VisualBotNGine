#ifndef GWELED_H_INCLUDED
#define GWELED_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

int thinkWhatToPlay(unsigned char * screen , unsigned int screenWidth ,unsigned int screenHeight ,
                    unsigned int clientStartX , unsigned int clientStartY ,
                    unsigned int * fromX, unsigned int * fromY , unsigned int * toX, unsigned int * toY
                    );



#ifdef __cplusplus
}
#endif

#endif // GWELED_H_INCLUDED
