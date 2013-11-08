#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "Codecs/codecs.h"
#include "ImageOperations/findSubImage.h"
#include "ImageOperations/imageOps.h"

#include "Engines/Gweled/Gweled.h"

int allowSnapshot=1;
int allowMouseControl=1;


unsigned int clickdelay = 40*1000;
unsigned int delay = 50*1000;


void countdownDelay(int seconds)
{
    if (seconds==0) { return; } //No delay do nothing!
    int secCounter=seconds;

    for (secCounter=seconds; secCounter>0; secCounter--)
    {
      fprintf(stderr,"%u\n",secCounter);
      usleep(1000*1000); // Waiting a while for the glitch frames to pass

    }
    usleep(1000*1000); // Waiting a while for the glitch frames to pass
}



int executeClickAndClick( unsigned int fromPixelX,unsigned int fromPixelY , unsigned int toPixelX, unsigned int toPixelY)
{
  if (!allowMouseControl) { return 0; }

  if ( (fromPixelX==toPixelX)  && (fromPixelY==toPixelY) ) { fprintf(stderr,"Doing nothing\n"); usleep(1*1000*1000); return 0; }
  fprintf(stderr,"executeClickAndClick %u,%u -> %u,%u \n",fromPixelX,fromPixelY,toPixelX,toPixelY);

  char commandStr[512];
  sprintf(commandStr,"xdotool mousemove --sync %u %u click 1",fromPixelX,fromPixelY);
  int i = system(commandStr);

  fprintf(stderr,"Step1 Done \n");
  usleep(clickdelay);

  sprintf(commandStr,"xdotool mousemove --sync %u %u click 1",toPixelX,toPixelY);
  i = system(commandStr);
  fprintf(stderr,"Step2 Done \n");


  return 1;
}


int reloadScreen()
{
  if (!allowSnapshot) { return 0; }
  fprintf(stderr,"reloadScreen\n");
  char commandStr[512]={0};
  sprintf(commandStr,"xwd -root -out ramfs/out.xwd && convert ramfs/out.xwd ramfs/screenshot.pnm");
  int i = system(commandStr);

  return (i==0);
}


int main(int argc, char *argv[])
{

  int i=0;
  for (i=0; i<argc; i++)
  {
    if (strcasecmp(argv[i],"-doNotMove")==0) { allowMouseControl=0; fprintf(stderr,"Deactivated mouse control\n"); } else
    if (strcasecmp(argv[i],"-doNotGrab")==0) { allowSnapshot=0; fprintf(stderr,"Deactivated screen grabbing \n"); } else
    if (strcasecmp(argv[i],"-clickDelay")==0) { clickdelay=atoi(argv[i+1])*1000; fprintf(stderr,"Set click delay to %u\n",clickdelay); } else
    if (strcasecmp(argv[i],"-Delay")==0) { delay=atoi(argv[i+1])*1000; fprintf(stderr,"Set delay to %u\n",delay); }
  }


    fprintf(stderr,"Please ready your window\n");
    countdownDelay(0);
    fprintf(stderr,"Starting Up\n");

    reloadScreen();

    struct Image * haystack = readImage("ramfs/screenshot.pnm",PNM_CODEC,0);
    struct Image * needle = readImage("seek.pnm",PNM_CODEC,0);

    unsigned int resX = 0 , resY = 0;

    /*
    if (
         RGBfindImageInImage(
                             haystack->pixels , haystack->width , haystack->height ,
                             needle->pixels   , needle->width   , needle->height   ,
                             &resX ,
                             &resY
                            )
       )
    {
        fprintf(stderr,"Found Image in Image @ %u,%u \n",resX,resY);
        resY+= needle->height
    }*/

    unsigned int iterations=0;
    unsigned int fromX , fromY , toX , toY;
    while (1)
    {
      fprintf(stderr,"Round %u \n",iterations);
      ++iterations;

      thinkWhatToPlay(
                       haystack->pixels,  haystack->width , haystack->height ,
                       resX , resY ,
                       &fromX , &fromY , &toX , &toY
                      );



      fprintf(stderr,"Moving From %u,%u to %u,%u ", fromX , fromY , toX , toY );
      executeClickAndClick( fromX , fromY , toX , toY  );


      usleep(delay);
      reloadScreen();

      destroyImage(haystack);
      haystack = readImage("ramfs/screenshot.pnm",PNM_CODEC,0);
    }

    destroyImage(needle);
    destroyImage(haystack);
    return 0;
}
