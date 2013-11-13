#ifndef MOUSEMOVEMENTDESCRIPTOR_H_INCLUDED
#define MOUSEMOVEMENTDESCRIPTOR_H_INCLUDED

enum clickTypes
{
  INVALID_MOVE = 0,
  MOVE_TO_NEUTRAL = 0,
  SINGLE_MOVE_CLICK,
  MOVE_CLICK_AND_MOVE_CLICK,
  MOVE_CLICK_DRAG_MOVE_UNCLICK
};


struct mouseMovementItem
{
   unsigned int fromX;
   unsigned int fromY;
   unsigned int toX;
   unsigned int toY;
   char mode;
};


struct mouseMovements
{
    unsigned int totalMovements;
    struct mouseMovementItem movement[100];
};



#endif // FINDSUBIMAGE_H_INCLUDED
