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

void getpixel(SDL_Surface *mysurface, int x, int y, char r, char g, char b)
{
  if(x < 0 || y < 0) return;
  if(x > mysurface->w || y > mysurface->h) return;

  // BIG TODO HERE!!!!!!!!!!!11
}


void setpixel(SDL_Surface * screen, int x, int y, char r, char g, char b)
{
  Uint8 *ubuff8;
  Uint16 *ubuff16;
  Uint32 *ubuff32;
  Uint32 color;
  
  if(x < 0 || y < 0) return;
  if(x > Ui::WIDTH() || y > Ui::HEIGHT()) return; 
  
  // Lock the screen/image, if needed.
  if (SDL_MUSTLOCK(screen)) {
    if (SDL_LockSurface(screen) < 0)
      return;
  }
  
  // Get the color 
  color = SDL_MapRGB(screen->format, r, g, b);
  
  // How we draw the pixel depends on the bitdepth 
  
  
  switch (screen->format->BytesPerPixel) {
  case 1:
    ubuff8 = (Uint8 *) screen->pixels;
    ubuff8 += (y * screen->pitch) + x;
    *ubuff8 = (Uint8) color;
    break;
  case 2:
    ubuff8 = (Uint8 *) screen->pixels;
    ubuff8 +=
      (y * screen->pitch) + (x * screen->format->BytesPerPixel);
    ubuff16 = (Uint16 *) ubuff8;
    *ubuff16 = (Uint16) color;
    break;
  case 3:
    ubuff8 = (Uint8 *) screen->pixels;
    ubuff8 += (y * screen->pitch) + (x * 3);
    r = (color >> screen->format->Rshift) & 0xFF;
    g = (color >> screen->format->Gshift) & 0xFF;
    b = (color >> screen->format->Bshift) & 0xFF;
    ubuff8[0] = r;
    ubuff8[1] = g;
    ubuff8[2] = b;
    break;
  case 4:
    ubuff32 = (Uint32 *) screen->pixels;
    ubuff32 += ((y * screen->pitch) >> 2) + x;
    *ubuff32 = color;
    break;
  default:
    fprintf(stderr, "Error: Unknown bitdepth!\n");
  }
    
  // Unlock the screen if needed 
  if (SDL_MUSTLOCK(screen)) {
    SDL_UnlockSurface(screen);
  }
}


void
scaleCopy2(int w1, int h1, unsigned char *map1, int w2,
	   int h2, unsigned char *map2)
{
    memcpy(map2, map1, w2 * h2);
}


void SBitmap::SetTrans(bool wantTrans)
{
  // BIG TOOD HERE
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


void SBitmap::LoadImage(char* file) 
{
  mysurface = SDL_LoadBMP(file);
  if(!mysurface) {
    cerr << "[Fatal] Could not load bitmap: " << SDL_GetError() << endl;
    exit(-1);
  }
}


void SBitmap::copy(SBitmap& b) 
{ 
  SDL_Surface* surfaceCopy;
  
  if(mysurface) SDL_FreeSurface(mysurface);
  if(!b.mysurface) return;

  surfaceCopy = b.mysurface;
  mysurface = SDL_ConvertSurface(surfaceCopy,
				 surfaceCopy->format,
				 surfaceCopy->flags);

  if(!mysurface) {
    cerr << "[Fatal] Couldn't Create SDL Surface!" << endl;
    exit(-1);
  }
}
