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

// Setup the palette 
char SBitmap::Vpal[256][3] = { {0, 0, 0} };

// Setup the palette 
void SBitmap::SetPalette(int num, char r, char g, char b)
{  
  if (num >= 0 && num < 256) {
    Vpal[num][0] = r;
    Vpal[num][1] = g;
    Vpal[num][2] = b;
  }
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



void SBitmap::setupsurface()
{
    int x, y;
    int rx, ry;
    SDL_Surface *temp;

    if (mysurface) {
	SDL_FreeSurface(mysurface);
	mysurface = 0;
    }

    if (Vimage) {
	mysurface =
	    SDL_CreateRGBSurface(SDL_SWSURFACE, 
				 DMULTCONST(Vwidth),
				 DMULTCONST(Vheight),
				 24, 0x000000ff, 0x0000ff00, 0x00ff0000,
				 0x00000000);

	for (y = 0, ry = 0; y < DMULTCONST(Vheight); y += DMULTCONST(1), ry++) {
	    for (x = 0, rx = 0; x < DMULTCONST(Vwidth); x += DMULTCONST(1), rx++) {

		setpixel(mysurface, x, y,
			 Vpal[Vimage[ry * Vwidth + rx]][0],
			 Vpal[Vimage[ry * Vwidth + rx]][1],
			 Vpal[Vimage[ry * Vwidth + rx]][2]);

#ifdef DOUBLE_SIZE
		setpixel(mysurface, x + 1, y,
			 Vpal[Vimage[ry * Vwidth + rx]][0],
			 Vpal[Vimage[ry * Vwidth + rx]][1],
			 Vpal[Vimage[ry * Vwidth + rx]][2]);

		setpixel(mysurface, x, y + 1,
			 Vpal[Vimage[ry * Vwidth + rx]][0],
			 Vpal[Vimage[ry * Vwidth + rx]][1],
			 Vpal[Vimage[ry * Vwidth + rx]][2]);

		setpixel(mysurface, x + 1, y + 1,
			 Vpal[Vimage[ry * Vwidth + rx]][0],
			 Vpal[Vimage[ry * Vwidth + rx]][1],
			 Vpal[Vimage[ry * Vwidth + rx]][2]);
#endif
	    }

	}

	/*
	 * Setup colorkeying, and then make sure we have alloced the
	 * fastest surface possible 
	 */
	if (wantTrans) {
	    SDL_SetColorKey(mysurface, SDL_SRCCOLORKEY,
			    SDL_MapRGB(mysurface->format, 0, 0, 0));

	    temp = mysurface;
	    mysurface = SDL_DisplayFormatAlpha(temp);
	    if (!mysurface)
		mysurface = temp;
	    else
		SDL_FreeSurface(temp);
	} else {
	    temp = mysurface;
	    mysurface = SDL_DisplayFormat(temp);

	    if (!mysurface)
		mysurface = temp;
	    else
		SDL_FreeSurface(temp);
	}
    }
}







void
scaleCopy2(int w1, int h1, unsigned char *map1, int w2,
	   int h2, unsigned char *map2)
{
    memcpy(map2, map1, w2 * h2);
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

    // cout << "W:" << width() << " H:"<< height() <<" Size:" << size() << 
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

    setupsurface();
}
