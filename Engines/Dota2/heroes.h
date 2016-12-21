#ifndef HEROES_H_INCLUDED
#define HEROES_H_INCLUDED

#define HERO_NUMBER 244

struct hero
{
  char name[128];
  float KDA;
  float globalWinRate;
  float globalPickRate;
};


struct allHeroes
{
  struct hero[HERO_NUMBER]
};

#endif // HEROES_H_INCLUDED
