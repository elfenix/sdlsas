// Copyright 1994 Brad Pitzel
//
// Feel free to use/distribute/modify as long as credit/copyrights for myself 
// are included.
//

// File   : SBitmap.h[1.0]
// Name   : gamelib1.0
// Status : proposed
// Changed: Sun Jun 12 22:19:02 1994

// What is this class for?
// This class is derived from the base class 'Bitmap'
//
// A bitmap object is an image that can be displayed on screen.  It is defined
// by: the image data (raw or compiled), and width/height/size attributes.
// The object takes care of allocating/deallocating memory for the image data
// automatically.
//
// SBitmap adds extra functionality using svgalib and the FastMath library.
//
// To display the image in the current graphics context, use the put(x,y)
// member function. (graphics context is a svgalib object. A gc defines the
// active screen. Normally the active screen is the video card's memory)
// The put() function assumes the colormap has been set/etc. (see svgalib
// docs/demo source for info on setting the colormap)
//
// Calling the 'compile' member function will use svgalib's compiled bitmap
// features to possibly speed up the put() function. If your bitmap has lots
// of empty space (0 values) in it, compiling it should help.
// Note if you change the bitmap by copying or rotating, you need to 
// re-compile. If you use this feature, use svgalib 1.11 or greater.
//
// There are a few simple manipulation functions available too: 
// mainly rotation.
//
// Note the rotation routines aren't that quick. It is best to create all
// the rotated images you'll need before entering the actual game play,
// because calling rotate() during the game will probably slow things
// down too much.
//
// Finally: Don't assume degrees=0..359 for rotation. Look in FastMath.h
// to see how Angle and TRIGSIZE are defined.

#ifndef __BPsbitmap__
#define __BPsbitmap__

#include <SDL/SDL.h>
#include "ui.h"
#include "bitmap.h"
#include "fastmath.h"	// sin/cos tables, Fixed Point math stuff

class SBitmap;
typedef SBitmap *SBitmapPtr;

// Note: 'V' prefix indicates class variable
class SBitmap : public Bitmap {
 public:
  SBitmap() : Bitmap(), mysurface(NULL), wantTrans(1) { };
  SBitmap(int w, int h) : Bitmap(w,h), mysurface(NULL), wantTrans(1) { };
  SBitmap(const SBitmap& pb) : Bitmap(pb), mysurface(NULL), wantTrans(1) { ; };
  
  ~SBitmap() 
    { if(mysurface) SDL_FreeSurface(mysurface); }
  
  // copy with scaling. Take bitmap b, scale it to width/height (w/h)
  // are copy result to 'this' object	
  void scaleCopy( const Bitmap& b, int w, int h );
  
  void copy(SBitmap& b) { 
    if (b.Vimage!=NULL) {
      allocMem( b.width(), b.height() );       
      if (Vimage!=NULL)   memcpy(Vimage, b.Vimage, Vsize);
    } else {
      Vimage=NULL;
    }
    setupsurface();	
  }
  
  static void SetPalette(int num, char r, char g, char b);
  void setupsurface();
  
  static char Vpal[256][3];
  
  // display bitmap on screen using svgalib calls
  inline void put(int x, int y) {
    SDL_Rect b;
    if(mysurface) {
      b.x = x*2; b.y = y*2;
      SDL_BlitSurface(mysurface, NULL, Ui::myscreen, &b);  
    } else {
      setupsurface();
      if(mysurface) {
	b.x = x; b.y = y;
	SDL_BlitSurface(mysurface, NULL, Ui::myscreen, &b);  
      }
    }
  }
  
  
  // Note: currently, the size of the bitmaps won't be changed by
  // any of the rotate functions, so data can be rotated 'out' of
  // the image.
  // rotate so many degrees.
  // degrees=0..TRIGSIZE (defined in FastMath.h)
  void rot(Angle degrees);
  
  inline void SetTrans(int i) 
    { wantTrans = i; }
  
 protected:
  SDL_Surface* mysurface;
  int wantTrans;
};


void setpixel(SDL_Surface *screen, int x, int y, char r, char g, char b);

#endif
