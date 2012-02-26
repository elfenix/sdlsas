#ifndef __MENU_H__
#define __MENU_H__

#include "sasteroids.h"


typedef void (*GraphicsMenuAction)(int* done);


class GraphicsMenu
{
 public:
  GraphicsMenu(SBitmap* mouse, SBitmap* backdrop, char** textEntries);
  void RunMenu();

  void setAction(int j, GraphicsMenuAction action);

  void setTopPad(float y);
  void enableSound(int num);

  void setMinSelectable(int min);
  void setMaxSelectable(int max);

 private:
  SBitmap* mouse;
  SBitmap* menu_strings;
  SBitmap* highlight_strings;
  GraphicsMenuAction* actions;

  SBitmap* backdrop;

  SDL_Rect* activeRegions;

  float topPad;

  int minSelect;
  int maxSelect;

  int num_entries;
  int soundOn;
  float menu_height;
};

#endif
