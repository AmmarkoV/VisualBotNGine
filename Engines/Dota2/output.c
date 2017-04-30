#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "output.h"

void saySomethingInDota2(const char * what2say)
{
  fprintf(stderr,"Say something in Dota2 %s \n",what2say);
  //1048 , 1041
  unsigned int chatPosX=1048;
  unsigned int chatPosY=1041;


  char commandStr[512];
  sprintf(commandStr,"xdotool mousemove --sync %u %u click 1",chatPosX,chatPosY);
  int retres=system(commandStr);

  usleep(100);

  sprintf(commandStr,"xdotool type '%s'",what2say);
  retres=system(commandStr);

  usleep(100);

  sprintf(commandStr,"xdotool key KP_Enter");
  retres=system(commandStr);


}
