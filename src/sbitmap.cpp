// Copyright 1994 Brad Pitzel
// Modification Copyright 2002 Andrew M. 
//
// Feel free to use/distribute/modify as long as credit/copyrights for
// myself and constributors are included.

// File : SBitmap.c[1.0]
// Changed: Sun Jun 12 22:19:30 1994

#include "sasteroids.h"

void SBitmap::SetTrans(bool wantTrans)
{
  Uint32 key;
  if(!mySurface) return;
  
  key = SDL_MapRGB(mySurface->format, 0, 0, 0);
  if(wantTrans) {
    SDL_SetColorKey(mySurface, SDL_SRCCOLORKEY|SDL_RLEACCEL, key);
  } else {
    SDL_SetColorKey(mySurface, 0, key);
  }

  compile();
}


void SBitmap::compile()
{  
  SDL_Surface* temp; 
  temp = mySurface;
  mySurface = SDL_DisplayFormat(mySurface);
  if(!mySurface) {
    cerr << "[Warning] " << SDL_GetError() << endl;
    mySurface = temp;
  } else 
    SDL_FreeSurface(temp);
}


// scale this bitmap
void SBitmap::scale2x()
{
  int x, y;
  char r, g, b;
  SDL_Surface* newSurface;
  
  if(!mySurface) return;
 
  newSurface = SDL_CreateRGBSurface(mySurface->flags,
				    mySurface->w*2, mySurface->h*2,
				    mySurface->format->BitsPerPixel,
				    mySurface->format->Rmask,
				    mySurface->format->Gmask,
				    mySurface->format->Bmask,
				    mySurface->format->Amask);
  if(!newSurface) {
    cerr << "[Warning] Scaling Failed: " << SDL_GetError() << endl;
    return;
  } 
  
  GraphicsStartDraw(newSurface);
  GraphicsStartDraw(mySurface);

  for( x = 0; x < mySurface->w; x++) {
    for( y = 0; y < mySurface->h; y++) {           
      getpixel(mySurface, x, y, &r, &g, &b);
      setpixel(newSurface, x*2, y*2, r, g, b);
      setpixel(newSurface, x*2+1, y*2+1, r, g, b);
      setpixel(newSurface, x*2, y*2+1, r, g, b);
      setpixel(newSurface, x*2+1, y*2, r, g, b);
    }
  } 

  GraphicsStopDraw(mySurface);
  GraphicsStopDraw(newSurface);

  SDL_FreeSurface(mySurface);
  mySurface = newSurface;
  
  compile();
}



// Simple rotate bitmap routine.
// Rotate bitmap by 90 degrees
void SBitmap::rotc90()
{
  unsigned int x, y;
  char r, g, b;
  SDL_Surface* newSurface;
  
  if(!mySurface) return;
  
  newSurface = SDL_ConvertSurface(mySurface,
				  mySurface->format,
				  mySurface->flags);
  if(!newSurface) {
    cerr << "[Warning] Ship Rotation Failed: " << SDL_GetError() << endl;
    return;
  } 

  GraphicsStartDraw(newSurface);
  GraphicsStartDraw(mySurface);


  for( y = 0; y < mySurface->h/2; y++) {
    for( x = y; x < (mySurface->w-y-1); x++) {

      getpixel(mySurface, y, mySurface->h-1-x,  &r, &g, &b);
      setpixel(newSurface, x, y, r, g, b);

      getpixel(mySurface, mySurface->w-1-x, mySurface->h-1-y,  &r, &g, &b);
      setpixel(newSurface, y, mySurface->h-1-x, r, g, b);

      getpixel(mySurface, mySurface->w-1-y, x,  &r, &g, &b);
      setpixel(newSurface, mySurface->w-1-x, mySurface->h-1-y, r, g, b);

      getpixel(mySurface, x, y, &r, &g, &b);
      setpixel(newSurface, mySurface->w-1-y, x,  r, g, b);
    }
  }

  GraphicsStopDraw(mySurface);
  GraphicsStopDraw(newSurface);

  SDL_FreeSurface(mySurface);
  mySurface = newSurface;
  
  compile();
}





// Simple rotate bitmap routine.
// Not optimized for speed. Use to create rotated versions of bitmaps
// at game initialization, not for on-the-fly rotation.
// width() and height() do not change, therefore.
// parts of the bitmap could be rotated 'out'.
void SBitmap::rot(Angle degrees)
{
  double xo, yo;
  unsigned int x, y, x1, y1;
  char r, g, b;
  SDL_Surface* newSurface;
  
  if(!mySurface) return;
  
  newSurface = SDL_ConvertSurface(mySurface,
				  mySurface->format,
				  mySurface->flags);

  GraphicsStartDraw(newSurface);
  GraphicsStartDraw(mySurface);
  
  if(!newSurface) {
    cerr << "[Warning] Ship Rotation Failed: " << SDL_GetError() << endl;
    return;
  } 


  // find middle of the bitmap
  xo = (double) (mySurface->w / 2);
  yo = (double) (mySurface->h / 2);

  // loop through each pixel in new buffer, and rotate *backwards*
  // into the bitmap to see what color it should have.
  // This method avoids getting holes in the bitmap
  for (y = 0; y < mySurface->h; y++)
    for (x = 0; x < mySurface->w; x++) {
      x1 = (int) (xo + ((double) x - xo) * FastMath::cos(-degrees) -
		  ((double) y - yo) * FastMath::sin(-degrees));
      
      y1 = (int) (yo + ((double) x - xo) * FastMath::sin(-degrees) +
		  ((double) y - yo) * FastMath::cos(-degrees));
      
      
      if ((x1 < 0) || (x1 >= mySurface->w) || (y1 < 0)
	  || (y1 >= mySurface->h)) {
	r = g = b = 0;
      } else {
	getpixel(mySurface, x1, y1, &r, &g, &b);
      }
      
       setpixel(newSurface, x, y, r, g, b);      
    }
  
  GraphicsStopDraw(mySurface);
  GraphicsStopDraw(newSurface);

  SDL_FreeSurface(mySurface);
  mySurface = newSurface;

  compile();
}


void SBitmap::LoadImage(char* file, bool sflag, int sF) 
{
  mySurface = SDL_LoadBMP(file);
  if(!mySurface) {
    cerr << "[Fatal] Could not load bitmap: " << SDL_GetError() << endl;
    exit(-1);
  }

  SetTrans(sflag);
  if(sF == 2) scale2x();
  SetTrans(sflag);
}


void SBitmap::copy(SBitmap& b) 
{ 
  SDL_Surface* surfaceCopy;
  
  if(mySurface) SDL_FreeSurface(mySurface);
  if(!b.mySurface) return;

  surfaceCopy = b.mySurface;
  mySurface = SDL_ConvertSurface(surfaceCopy,
				 surfaceCopy->format,
				 surfaceCopy->flags);

  if(!mySurface) {
    cerr << "[Fatal] Couldn't Create SDL Surface!" << endl;
    exit(-1);
  }
  
  compile();
}
