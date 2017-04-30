#ifndef PICK_H_INCLUDED
#define PICK_H_INCLUDED

#include "../../Codecs/codecs.h"


struct pickScreen
{
  unsigned int avatarWidth,avatarHeight;
  unsigned int avatarX[10];
  unsigned int avatarY[10];

  unsigned int ourPlayer;
};

struct teams
{
  unsigned int playersHeroes[10];
  unsigned int playersPicked[10];

  unsigned int ourPlayer;
};


int initializeSeeingPicks();
int printPicks(struct teams * team );
int seePicks(  struct teams * team ,struct Image * view);

#endif // PICK_H_INCLUDED
