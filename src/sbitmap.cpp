// Copyright 1994 Brad Pitzel
// 
// Feel free to use/distribute/modify as long as credit/copyrights for
// myself 
// and constributors are included.

// File : SBitmap.c[1.0]
// Name : gamelib1.0
// Status : proposed
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
// Not optimized for speed. Use to create rotated versions of bitmaps
// at game initialization, not for on-the-fly rotation.
// width() and height() do not change, therefore.
// parts of the bitmap could be rotated 'out'.
void SBitmap::rot(Angle degrees)
{

  /*    // cout << "W:" << width() << " H:"<< height() <<" Size:" << size() << 
    // endl;
    unsigned char *buf = new unsigned char[size()];

    if (buf == NULL) {
	cerr << "SBitmap::rot(), unable to allocated memory.\n";
	return;
    }

    if (Vimage == NULL) {
	cerr << "SBitmap::rot(), called with null buffer!." << endl;
	return;
    }


    int x, y, x1, y1;
    double xo, yo;
    unsigned char c;

    // middle of bitmap
    xo = (double) (width() >> 1);
    yo = (double) (height() >> 1);

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
		c = 0;
	    } else {
		c = *(Vimage + offset(x1, y1));
	    }

	    *(buf + offset((int) x, (int) y)) = c;

	}
    // delete original- if we had allocated the mem ourselves
    if (VallocImage)
	delete[]Vimage;

    VallocImage = 1;

    // set bitmap to new rotated bitmap
    Vimage = buf;

    setupsurface(); */
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
}
