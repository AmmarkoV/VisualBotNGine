#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "Codecs/codecs.h"
#include "ImageOperations/findSubImage.h"
#include "ImageOperations/imageOps.h"

#include "Engines/MouseMovementDescriptor.h"
#include "Engines/Gweled/Gweled.h"
#include "xwd-1.0.5/XwdLib.h"

#define XWDLIB_BRIDGE 1
#define AUTO_LOCALIZE 0
#define NEW_AUTO_LOCALIZE 0

#define SECOND_USLEEP 1000*1000

int allowSnapshot=1;
int allowMouseControl=1;
int totalNotMoves=0;

//Various delays , confiurable via commandline parameters
unsigned int clickdelay = 40*1000;
unsigned int delay = 5*1000;
unsigned int dontknowdelay = 100*1000;

//These are the coordinates that we want the mouse to go at
//when we dont want it to interact with our game
signed int mouseNeutralX = -50;
signed int mouseNeutralY = -50;


//sissy foss : 619 354 ,
//Jazz : 620 468
//Ammar Ubuntu 13.10
//unsigned int resX = 337 , resY = 382;
//unsigned int resX = 337 , resY = 383-1;

//51 326 arxi tou flash player
unsigned int oX = 56 , oY = 326;
unsigned int resX=0 , resY =0; //This will get initialized in main


void countdownDelay(int seconds)
{
    if (seconds==0) { return; } //No delay do nothing!
    int secCounter=seconds;

    for (secCounter=seconds; secCounter>0; secCounter--)
    {
      fprintf(stderr,"%u\n",secCounter);
      usleep(SECOND_USLEEP);
    }
}




void mouseToNeutral(unsigned int clientX,unsigned int clientY)
{
  char commandStr[512];
  sprintf(commandStr,"xdotool mousemove --sync %u %u click 1",clientX+mouseNeutralX,clientY+mouseNeutralY);
  int retres=system(commandStr);
  usleep(clickdelay);
}



int executePlan(struct mouseMovements * plan)
{
  if (!allowMouseControl) { return 0; }

  //fprintf(stderr,"executeClickAndClick %u,%u -> %u,%u \n",fromPixelX,fromPixelY,toPixelX,toPixelY);

  char commandStr[512];
  int i=0 , retres=0;

  for (i=0; i<plan->totalMovements; i++)
  {
   if ( (plan->movement[i].fromX==plan->movement[i].toX)  &&
        (plan->movement[i].fromY==plan->movement[i].toY) )
            { fprintf(stderr,"Movement is null\n"); }
    else
    {
      switch (plan->movement[i].mode)
      {
        case MOVE_CLICK_AND_MOVE_CLICK :
          sprintf(commandStr,"xdotool mousemove --sync %u %u click 1",plan->movement[i].fromX,plan->movement[i].fromY);
          retres=system(commandStr);
          fprintf(stderr,"Moving : From OK .. ");

          usleep(clickdelay);

          sprintf(commandStr,"xdotool mousemove --sync %u %u click 1",plan->movement[i].toX,plan->movement[i].toY);
          retres=system(commandStr);
          fprintf(stderr,"To OK \n");
        break;
        case SINGLE_MOVE_CLICK :
          sprintf(commandStr,"xdotool mousemove --sync %u %u click 1",plan->movement[i].fromX,plan->movement[i].fromY);
          retres=system(commandStr);
          fprintf(stderr,"Moving : Single Click OK .. ");
          usleep(clickdelay);
        break;
        case MOVE_TO_NEUTRAL :
          mouseToNeutral(resX,resY);
          usleep(clickdelay*5);
        break;
      }
    }
  }

  return 1;
}


struct Image * reloadScreen(struct Image * lastImg)
{
  destroyImage(lastImg);

  if (!allowSnapshot) { return readImage("ramfs/screenshot.pnm",PNM_CODEC,0); }

  #if XWDLIB_BRIDGE
    struct Image * newImg = createImage(1920,1080,3,8);
    getScreen(newImg->pixels , &newImg->width, &newImg->height);
    //writeImageFile(newImg,PNM_CODEC,"out.pnm");
    //destroyImage(newImg);
    return newImg;
  #else
   fprintf(stderr,"reloadScreen\n");
   char commandStr[512]={0};
   sprintf(commandStr,"xwd -root -out ramfs/out.xwd && convert ramfs/out.xwd ramfs/screenshot.pnm");
   int i = system(commandStr);

   return readImage("ramfs/screenshot.pnm",PNM_CODEC,0);
  #endif // XWDLIB_BRIDGE

}


int main(int argc, char *argv[])
{
  int i=0;
  for (i=0; i<argc; i++)
  {
    if (strcasecmp(argv[i],"-clickdelay")==0) { clickdelay=1000*atoi(argv[i+1]);  fprintf(stderr,"clickdelay set to %u milliseconds \n",clickdelay); } else
    if (strcasecmp(argv[i],"-delay")==0) { delay=1000*atoi(argv[i+1]);  fprintf(stderr,"delay set to %u milliseconds \n",delay); } else
    if (strcasecmp(argv[i],"-clientXY")==0) { resX=atoi(argv[i+1]); resY=atoi(argv[i+2]);  fprintf(stderr,"client set to %u %u \n",resX,resY); } else
    if (strcasecmp(argv[i],"-doNotMove")==0) { allowMouseControl=0; fprintf(stderr,"Deactivated mouse control\n"); } else
    if (strcasecmp(argv[i],"-doNotGrab")==0) { allowSnapshot=0; fprintf(stderr,"Deactivated screen grabbing \n"); } else
    if (strcasecmp(argv[i],"-clickDelay")==0) { clickdelay=atoi(argv[i+1])*1000; fprintf(stderr,"Set click delay to %u\n",clickdelay); } else
    if (strcasecmp(argv[i],"-Delay")==0) { delay=atoi(argv[i+1])*1000; fprintf(stderr,"Set delay to %u\n",delay); }
  }

  #if XWDLIB_BRIDGE
    initXwdLib(argc,argv);
  #endif // XWDLIB_BRIDGE

    fprintf(stderr,"Please ready your window\n");
    countdownDelay(0);
    fprintf(stderr,"Starting Up\n");

    struct Image * haystack = reloadScreen(0);

    #if NEW_AUTO_LOCALIZE
    oX=0; oY=0;
    struct Image * needle = readImage("Engines/Gweled/Menus/startOfClient.pnm",PNM_CODEC,0);
     if (
         RGBfindImageInImage(
                             haystack->pixels , haystack->width , haystack->height ,
                             needle->pixels   , needle->width   , needle->height   ,
                             &oX ,
                             &oY ,
                             50,
                             200
                            )
       )
    {
        fprintf(stderr,"Found Image in Image @ %u,%u \n",oX,oY);
        exit(0);
    }

    destroyImage(needle);
    #endif // NEW_AUTO_LOCALIZE
    #if AUTO_LOCALIZE
    struct Image * needle = readImage("Engines/Gweled/Menus/boardStartSmall.pnm",PNM_CODEC,0);
    if (
         RGBfindImageInImage(
                             haystack->pixels , haystack->width , haystack->height ,
                             needle->pixels   , needle->width   , needle->height   ,
                             &resX ,
                             &resY ,
                             1000
                            )
       )
    {
        fprintf(stderr,"Found Image in Image @ %u,%u \n",resX,resY);
        resY+= needle->height;
    }

    destroyImage(needle);
    #endif // AUTO_LOCALIZE

    unsigned int iterations=0;
    unsigned int fromX , fromY , toX , toY;
    struct mouseMovements ourPlan;

    if (! initializeEngine("bejeweled blitz") )
    {
       fprintf(stderr,"Error initializing engine :(\n");
       exit(1);
    }


    resX = oX+175 , resY = oY+109;
    while (1)
    {
      fprintf(stderr,"Round %u \n",iterations);
      ++iterations;

       unsigned int  possibleMoves= thinkWhatToPlay(
                                                        haystack->pixels,  haystack->width , haystack->height ,
                                                        resX , resY ,  &ourPlan
                                                   );

       if ( possibleMoves==0 )
          {
            fprintf(stderr,"Can't think of a move to play\n");
            ++totalNotMoves;
            if (totalNotMoves%20==0) { mouseToNeutral(resX,resY); }
            usleep(dontknowdelay);
          }
            else
          {
            executePlan(&ourPlan);
          }

      usleep(delay);
      haystack = reloadScreen(haystack);
    }

    destroyImage(haystack);

    stopEngine();
  #if XWDLIB_BRIDGE
    closeXwdLib();
  #endif // XWDLIB_BRIDGE

    return 0;
}
