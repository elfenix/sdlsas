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

#include "sasteroids.h"

class SBitmap;
typedef SBitmap *SBitmapPtr;

// Note: 'V' prefix indicates class variable
class SBitmap
{
 public:
  SBitmap() : mysurface(NULL) { };
  SBitmap(char *file) : mysurface(NULL) { LoadImage(file); }
  SBitmap(int w, int h) : mysurface(NULL) { };
  SBitmap(const SBitmap& pb) : mysurface(NULL) { ; };
  
  ~SBitmap() 
    { if(mysurface) SDL_FreeSurface(mysurface); }
  
  // Image functions. :)
  void LoadImage(char* path);
  void copy(SBitmap& b);
  
  // copy with scaling. Take bitmap b, scale it to width/height (w/h)
  // are copy result to 'this' object	
  void scaleCopy( const Bitmap& b, int w, int h );
    
  // display bitmap on screen using svgalib calls
  inline void put(int x, int y) {
    SDL_Rect b;
    if(mysurface) {
      b.x = DMULTCONST(x); b.y = DMULTCONST(y);
      SDL_BlitSurface(mysurface, NULL, Ui::myscreen, &b);  
    } 
  }
  
  void rot(Angle degrees);
  void SetTrans(bool wantTrans);

  inline unsigned int width() const
    { 
      if(!mysurface) return 0; 
      return mysurface->h; 
    };

  inline unsigned int height() const
    {
      if(!mysurface) return 0;
      return mysurface->h; 
    };
  
 protected:
  SDL_Surface* mysurface;
};


void setpixel(SDL_Surface *screen, int x, int y, char r, char g, char b);

#endif
