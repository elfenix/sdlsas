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


// copy with scaling 
void SBitmap::scaleCopy(const Bitmap & b, int w, int h)
{

    // check if scale factor=1
    /*
     * cout << "Scaling to " << w << "x" << h << endl; if ( (b.width()==w) 
     * && (b.height()==h) ) { copy( b ); } else { allocMem(w,h);
     * 
     * if (Vimage!=NULL) {
     * 
     * scaleCopy2( b.width(), b.height(), (unsigned char *)b.map(), w, h, 
     * Vimage); } } 
     */
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


  for( y = 0; y < height()/2; y++) {
    for( x = y; x < (width()-y-1); x++) {

      getpixel(mySurface, y, height()-1-x,  &r, &g, &b);
      setpixel(newSurface, x, y, r, g, b);

      getpixel(mySurface, width()-1-x, height()-1-y,  &r, &g, &b);
      setpixel(newSurface, y, height()-1-x, r, g, b);

      getpixel(mySurface, width()-1-y, x,  &r, &g, &b);
      setpixel(newSurface, width()-1-x, height()-1-y, r, g, b);

      getpixel(mySurface, x, y, &r, &g, &b);
      setpixel(newSurface, width()-1-y, x,  r, g, b);
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
  xo = (double) (width() / 2);
  yo = (double) (height() / 2);

  // loop through each pixel in new buffer, and rotate *backwards*
  // into the bitmap to see what color it should have.
  // This method avoids getting holes in the bitmap
  for (y = 0; y < height(); y++)
    for (x = 0; x < width(); x++) {
      x1 = (int) (xo + ((double) x - xo) * FastMath::cos(-degrees) -
		  ((double) y - yo) * FastMath::sin(-degrees));
      
      y1 = (int) (yo + ((double) x - xo) * FastMath::sin(-degrees) +
		  ((double) y - yo) * FastMath::cos(-degrees));
      
      
      if ((x1 < 0) || (x1 >= width()) || (y1 < 0)
	  || (y1 >= height())) {
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


void SBitmap::LoadImage(char* file, bool sflag) 
{
  mySurface = SDL_LoadBMP(file);
  if(!mySurface) {
    cerr << "[Fatal] Could not load bitmap: " << SDL_GetError() << endl;
    exit(-1);
  }

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
