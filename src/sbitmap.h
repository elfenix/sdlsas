// Copyright 1994 Brad Pitzel
//
// Feel free to use/distribute/modify as long as credit/copyrights for myself 
// are included.
//

// File   : SBitmap.h[1.0]
// Changed: Friday 11-OCT-2002

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

#ifndef __SDLSAS_SBITMAP__
#define __SDLSAS_SBITMAP__

#include "sasteroids.h"

class SBitmap
{
 public:
  SBitmap() : mySurface(NULL) { };
  SBitmap(char *file) : mySurface(NULL) { LoadImage(file); }
  SBitmap(int w, int h) : mySurface(NULL) { };
  SBitmap(const SBitmap& pb) : mySurface(NULL) { ; };
  
  ~SBitmap() 
    { if(mySurface) SDL_FreeSurface(mySurface); }
  
  // Image functions. :)
  void LoadImage(char* path, bool setTrans = true);
  void copy(SBitmap& b);
  void rotc90();
  void rot(Angle degrees);
  void SetTrans(bool wantTrans);
  void scaleCopy( const Bitmap& b, int w, int h );
  void compile();
    
  // Inline (speed critical) functions bellow:
  inline void put(int x, int y) {
    SDL_Rect b;
    if(!mySurface) return;
    b.x = DMULTCONST(x); b.y = DMULTCONST(y);
    SDL_BlitSurface(mySurface, NULL, Ui::myscreen, &b); 
  }
  
  inline unsigned int width() const
    { 
      if(!mySurface) return 0; 
      return mySurface->h; 
    };

  inline unsigned int height() const
    {
      if(!mySurface) return 0;
      return mySurface->h; 
    };
  
 protected:
  SDL_Surface* mySurface;
};

#endif
