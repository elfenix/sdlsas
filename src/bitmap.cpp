// Copyright 1994 Brad Pitzel
// 
// Feel free to use/distribute/modify as long as proper credits
// are included.

#include "sasteroids.h"

// #include "ui.h"
// #include "bitmap.h"

using namespace std;

int Bitmap::offset(int x, int y)
{
  return y * Vwidth + x;
}


int Bitmap::size() const
{
  return Vsize;
}


void Bitmap::setMap(ImageLoader & IL)
{
  if (VallocImage)
    delete[]Vimage;
  Vimage = IL.image();
  Vwidth = IL.width();
  Vheight = IL.height();
  Vsize = Vwidth * Vheight;
  VallocImage = 0;
  
  if (Vimage != NULL) {
    // Bitmap takes over memory management of image data
    VallocImage = 1;
    IL.releaseImageOwnership();
  }
}


void Bitmap::setMap(unsigned char *p)
{
  if (VallocImage)
    delete[]Vimage;
  VallocImage = 0;
  Vimage = p;
}


Bitmap::Bitmap(const Bitmap & b)
{
  copy(b);
}


Bitmap::~Bitmap()
{
  if (VallocImage)
    delete[]Vimage;
}


void Bitmap::copy(const Bitmap & b)
{
  if (b.Vimage != NULL) {
    allocMem(b.width(), b.height());
    if (Vimage != NULL)
      memcpy(Vimage, b.Vimage, Vsize);
  } else {
    Vimage = NULL;
  }
}


// rotate 90 degress counter-clockwise
void Bitmap::rotcc90()
{
  // only works on square bitmaps
  if (width() == height()) {
    int x, y;
    unsigned char *ul, *ur, *ll, *lr, swap;
    
    for (y = 0; y < (height() >> 1); y++)
      for (x = y; x < (width() - y - 1); x++) {
	ul = (Vimage + offset(x, y));
	ur = (Vimage + offset(width() - 1 - y, x));
	ll = (Vimage + offset(y, height() - 1 - x));
	lr = (Vimage + offset(width() - 1 - x, height() - 1 - y));
		
	swap = *ur;
	*ur = *lr;
	*lr = *ll;
	*ll = *ul;
	*ul = swap;
      }
  } else {
    cout << "Couldn't do it!" << endl;
  }
}

// rotate 90 degress clockwise
void Bitmap::rotc90()
{
  // only works on square bitmaps
  if (width() == height()) {
    int x, y;
    unsigned char *ul, *ur, *ll, *lr, swap;
    
    for (y = 0; y < (height() >> 1); y++)
      for (x = y; x < (width() - y - 1); x++) {
	ul = (Vimage + offset(x, y));
	ur = (Vimage + offset(width() - 1 - y, x));
	ll = (Vimage + offset(y, height() - 1 - x));
	lr = (Vimage + offset(width() - 1 - x, height() - 1 - y));
	
	swap = *ul;
	*ul = *ll;
	*ll = *lr;
	*lr = *ur;
		*ur = swap;
      }
  }
}

// allocate memory for the bitmap
void Bitmap::allocMem(int w, int h)
{
  if (VallocImage == 1) {
    // if new size same as old, just reuse mem
    if ((width() == w) && (height() == h)) {
      return;
    }
    
    delete[]Vimage;
  }
  
  Vimage = new unsigned char[w * h];
  
  if (Vimage != NULL) {
    VallocImage = 1;
    Vwidth = w;
    Vheight = h;
    Vsize = w * h;
  } else {
    VallocImage = 0;
    Vwidth = 0;
    Vheight = 0;
    Vsize = 0;
  }
}




// ///////////////////////////////////////////////////////////////////////////
// Class : ImageLoader

int ImageLoader::load(const char *file, char flags)
{
  if (flags & LOAD_PALETTE)
    freePalette();
  if (flags & LOAD_IMAGE)
    freeImage();
  return loadFile(file, flags);
}


void ImageLoader::freeImage()
{
  if (Vimage)
	delete[]Vimage;
  Vimage = NULL;
  Vwidth = 0;
  Vheight = 0;
  VimageByteSize = 0;
}


// reset palette data
void ImageLoader::freePalette()
{
  if (Vpalette)
    delete[]Vpalette;
  Vpalette = NULL;
  VpaletteByteSize = 0;
  VnumColors = 0;
}
