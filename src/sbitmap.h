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
  SBitmap();
  SBitmap(char *file);  
  SBitmap(SDL_Surface* surface);
  SBitmap(const SBitmap& pb);

  ~SBitmap();
  
  void LoadImage(char* path);
  void LoadSurface(SDL_Surface* surface);

  void put(float x, float y, float rotate = 1.0f, float rotate = 1.0f, float layer = 0.0f);
  void putA(float x, float y, float layer = 0.0f, float alpha = 1.0f);
  
  inline float width() const
    { 
      return myWidth;
    };

  inline float height() const
    {
      return myHeight;
    };
  
 protected:
#ifdef WANT_OPENGL
  GLuint myTexture, myTextureOwned;
  GLfloat myWidth, myHeight, myHWidth, myHHeight;
  GLfloat myTexcoord[4];  
#else
  SDL_Surface* myTexture;
  unsigned int myTextureOwned;
  float myWidth, myHeight, myHWidth, myHHeight;
  float myTexcoord[4];
#endif
};

#endif
