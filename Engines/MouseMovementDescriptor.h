#ifndef MOUSEMOVEMENTDESCRIPTOR_H_INCLUDED
#define MOUSEMOVEMENTDESCRIPTOR_H_INCLUDED

enum clickTypes
{
  CLICK_AND_CLICK=0,
  CLICK_DRAG_UNCLICK
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
