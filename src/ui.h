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

#ifdef WANT_OPENGL
#   include <GL/gl.h>
#   include <GL/glu.h>
#else
/* TODO: No openGL */
#endif

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
class SBitmap;


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

  static SDL_Surface* get_text(char* msg, char r,char g, char b);
  
  static int fontHeight()  
    { 
      return TTF_FontHeight(myfont); 
    }

  static void inline setpixel(int x, int y, char r, char g, char b, char a = 255)
    {
      if(x < 0 || x >= myscreen->w) return;
      if(y < 0 || y >= myscreen->h) return;

#ifdef WANT_OPENGL
      char byteBuffer[4] = { r, g, b, a };


      glRasterPos2i(x, y);
      glDrawPixels(1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &byteBuffer); 

      /*

      glDisable(GL_TEXTURE_2D);  
      glDisable(GL_BLEND);
      glBegin(GL_QUADS);

      glColor3b(r, g, b);
      glVertex3f(x, y, 0.0f);

      glColor3b(r, g, b);
      glVertex3f(x+1.0f, y, 0.0f);

      glColor3b(r, g, b);
      glVertex3f(x+1.0f, y+1.0f, 0.0f);

      glColor3b(r, g, b);
      glVertex3f(x, y+1.0f, 0.0f);

      glEnd();
      glEnable(GL_TEXTURE_2D);
      glEnable(GL_BLEND);
      glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
      */
#else
      //      (pixelDriver)(myscreen, x, y, r, g, b);
#endif
    }

  static int fontWidth() { return 19; } // Fudged, shouldn't be used...
   
  static int WIDTH() { if(myscreen) return myscreen->w; return 0; }
  static int HEIGHT() { if(myscreen) return myscreen->h; return 0; }
  
  static void clearscreen() {
#ifdef WANT_OPENGL
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
#endif
  };
  
  static void FullScreen() {
    SDL_WM_ToggleFullScreen(myscreen);
  };
  

  static void predraw();

  
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


class IntegerDisplay
{
 public:
  static void initialize();
  static void display_integer(int num, float x, float y);
  static void finish();

 private:
  static SBitmap* numbers;

};

#endif
