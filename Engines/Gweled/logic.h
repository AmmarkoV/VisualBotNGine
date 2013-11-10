#ifndef LOGIC_H_INCLUDED
#define LOGIC_H_INCLUDED

extern unsigned int lastMoveX;
extern unsigned int lastMoveY;


#define MAXSOLUTIONS 256

struct solutionItem
{
    unsigned int fromX;
    unsigned int fromY;
    unsigned int toX;
    unsigned int toY;
    unsigned int score;
};

struct solutionList
{
  struct solutionItem solution[MAXSOLUTIONS+1];
  unsigned int currentSolutions;
};



int getValidMoves(unsigned int table[8][8] , struct solutionList * list);

#endif // LOGIC_H_INCLUDED
