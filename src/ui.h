// Copyright 1994 Brad Pitzel
//
// Feel free to use/distribute/modify as long as credit/copyrights for myself 
// are included.

// File   : Ui.h[1.0]
// Changed: FRI OCT-10-2002

// a layer on top of the svgalib library.
// Routines to setup the initialize the screen/svgalib/etc, restore back
// to text when all done, throw up a few messages/prompts, get simple
// string input from the user, etc..


#ifndef __Ui__
#define __Ui__

#include "sasteroids.h"

// call before using setpixel or getpixel. =)
void GraphicsStartDraw(SDL_Surface* visual);
void GraphicsStopDraw(SDL_Surface* visual);

// Unsafe putpixel functions, may be used, but don't check x/y coord's.
void g_setpixelB1(SDL_Surface* visual, int x, int y, char r, char g, char b);
void g_setpixelB2(SDL_Surface* visual, int x, int y, char r, char g, char b);
void g_setpixelB3(SDL_Surface* visual, int x, int y, char r, char g, char b);
void g_setpixelB4(SDL_Surface* visual, int x, int y, char r, char g, char b);
void setpixel(SDL_Surface *screen, int x, int y, char r, char g, char b);

// Unsafe getpixel functions, may be used, but don't check x/y coord's.
void g_getpixelB1(SDL_Surface* v, int x, int y, char *r, char *g, char *b);
void g_getpixelB2(SDL_Surface* v, int x, int y, char *r, char *g, char *b);
void g_getpixelB3(SDL_Surface* v, int x, int y, char *r, char *g, char *b);
void g_getpixelB4(SDL_Surface* v, int x, int y, char *r, char *g, char *b);
void getpixel(SDL_Surface *screen, int x, int y, char *r, char *g, char *b);

// Other integers
extern int wantFullScreen;

class Ui 
{
 public: 
  static void updateScreen();
  static void init();
  static void restore();
  static void resync(int x, int y);
  
  static void message( int y, char *msg ); // message centered at line y
  static void message( char *msg ) { message(0,msg); }
  static void CenterText(char* msg);
  static void CenterXText(int y, char* msg);
  static void ShowText(int x, int y, char* msg );
  static void ShowTextColor(int x, int y, char* msg, char r, char g, char b);
  static char yesNo( const char *msg );
  
  // returns last key pressed (eg. CR or ESC)
  static int input( int x, int y, char *inp, int len );
  static int input( int x, int y, char *prompt, char *inp, int len ) 
    {
      return Ui::input( x, y, inp, len );
    }
  
  static int fontHeight()  
    { 
      return TTF_FontHeight(myfont); 
    }

  static void inline setpixel(int x, int y, char r, char g, char b)
    {
      if(x < 0 || x >= myscreen->w) return;
      if(y < 0 || y >= myscreen->h) return;
      (pixelDriver)(myscreen, x, y, r, g, b);
    }

  static int fontWidth() { return 19; } // Fudged, shouldn't be used...
   
  static int WIDTH() { if(myscreen) return myscreen->w; return 0; }
  static int HEIGHT() { if(myscreen) return myscreen->h; return 0; }
  
  static void clearscreen() {
    SDL_Rect A;
    
    if(!myscreen) return;
    A.x = A.y = 0;
    A.w = myscreen->w; A.h = myscreen->h;
    SDL_FillRect(myscreen, &A, SDL_MapRGB(myscreen->format,0,0,0));
  };
  
  static void FullScreen() {
    SDL_WM_ToggleFullScreen(myscreen);
  };
  
  
  friend class SBitmap;
  friend class Bitmap;
  
  static SDL_Surface* myscreen;

 private:
  static TTF_Font *myfont;
  static void drawCursor(int x, int y);
  static void hideCursor(int x, int y);

  static void (*pixelDriver)(SDL_Surface* visual, 
			     int x, int y, char r, char g, char b);
};

#endif
