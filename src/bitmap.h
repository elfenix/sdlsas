//
// Copyright 1994 Brad Pitzel
//
// Feel free to use/distribute/modify as long as credit/copyrights for myself 
// and contributors are included.
//

// File   : Bitmap.h[1.0]
// Name   : gamelib1.0
// Status : proposed
// Changed: Sun Jun 12 22:19:09 1994


#ifndef __BPbitmap__
#define __BPbitmap__

#include <stdlib.h>	// for NULL
#include <math.h>
#include <string.h>	// for memcpy
#include <SDL/SDL.h>

#include <iostream>
using namespace std;


class Bitmap;
typedef Bitmap *BitmapPtr;

//////////////////////////////////////////////////////////////////////////
// Object to load various image formats.  Used by Bitmap class.
//
// ImageLoader is an abstract base class, i.e. you cannot instantiate
// objects directly from it (except as parameters in a function).
//
// Derive format specific classes from this class.
// eg: derive PcxImageLoader to handle .pcx files,  RawImageLoader for .raw
// files, etc..
//
// The load() function handles loading the image, palette, or both.
// This function is specificly coded for the file format, the rest
// of the class is generic.
//
// Derived classes need to define load() to set Vimage, Vpalette,
// Vwidth, Vheight, etc.., accordingly for a given image file format
//
// load() functions return EXIT_SUCCESS, or EXIT_FAILURE. (from stdlib.h)
// 
// load() can assume Vimage/Vpalette points to nothing upon entry. Must
// ensure Vimage (or Vpalette) is NULL after an image loading (or palette
// loading) error. 
//

// flags for telling load() what to load!
#define LOAD_IMAGE		1
#define LOAD_PALETTE 	2

class ImageLoader
{
	public:
	
		ImageLoader() : Vimage(NULL), Vpalette(NULL)
			{ freeImage(); freePalette(); }

		virtual ~ImageLoader()
			{ freeImage(); freePalette();	}

		virtual int load(const char *file, char flags= (LOAD_IMAGE|LOAD_PALETTE));

		int width() const 
			{ return Vwidth; }
		
		int height() const 
			{ return Vheight; }
		
		unsigned char 	*image() const 
			{ return Vimage; }

		unsigned char *palette() const 
			{ return Vpalette; }
		
		int numColors() const 
			{ return VnumColors; }
		
		int imageByteSize() const 
			{ return VimageByteSize; }
		
		int paletteByteSize() const 
			{ return VpaletteByteSize; }

	// clear all private vars, free up allocated mem.
		void		freeImage();
		void		freePalette();

	// if we assign the image data from the loader to an external object
	// and the external object wants to manage the allocated memory
	// for the image, the ImageLoader shouldn't delete the mem.
	// You can prevent ImageLoader from freeing up the image or
	// palette memory by calling these functions:
		void releaseImageOwnership() { Vimage=NULL; }
		void releasePaletteOwnership() { Vpalette=NULL; }

    protected:
	// image specific load routine 
	// Must be defined by derived classes.
		virtual int	loadFile( const char *file, char flags) = 0;

    	int		Vwidth;
    	int		Vheight;
    	int		VimageByteSize;		// size of image in bytes
   	int		VpaletteByteSize;	// size of palette in bytes
	   int		VnumColors;		// # of colors in palette
    	
		unsigned char 	*Vimage;	// image data
		unsigned char 	*Vpalette;	// palette data
};


////////////////////////////////////////////////////////////////////////////
// What is this class for?
//
// Bitmap describes a simple bitmap object.  Rarely will you directly
// use this class. Rather, you should derive another class that defines more 
// functionality, ie add in put() functions for environment specific code 
// to display the bitmap on screen (or maybe in a window).
//
// A bitmap object is an image that can be displayed on screen.  It is defined
// by: the raw image data, and width/height/size attributes.
// The object takes care of allocating/deallocating memory for the image data
// automatically.
//
// There are functions for copying Bitmap objects, and for simple 90 degree
// rotations.

// Note: 'V' prefix indicates class variable

class Bitmap
{
    protected:

		int Vwidth;
		int Vheight;
		int Vsize;
	
		// bitmap pointer
		unsigned char *Vimage;	// pointer to bitmap

		// need to free it?
		char VallocImage;	

		// allocate memory for the buffer
		void allocMem( int w, int h );	

		// calculate offset to a given x,y coordinate
		int offset(int x, int y); 

	public:
		Bitmap() : Vwidth(0),Vheight(0),Vsize(0),Vimage(NULL),VallocImage(0) {};
		Bitmap(int w, int h) : Vwidth(w), Vheight(h), Vsize(w*h), Vimage(NULL), VallocImage(0) {};
		Bitmap( const Bitmap& b );
		~Bitmap();
			
		// Copy: make 'this' bitmap be a clone of bitmap 'b'
		void copy( const Bitmap& b );
		
		// Note:  The 90 degree rotate functions only work with square
		// bitmaps (width==height).
		void rotc90();						// rotate bitmap 90 clockwise
		void rotcc90();					// rotate bitmap 90 counter-clockwise

		// setMap to p, p should be a pointing to the 8bit visual data
		void setMap( unsigned char *p ); 
		
		// if we set the bitmap from an ImageLoader object, then we are
		// responsible for managing the memory.
		void setMap( ImageLoader &IL );

		// returns a pointer to the bitmap's raw image data
		inline const unsigned char* map() const { return Vimage; }
	
		int  width() const { return Vwidth; }
		int  height() const { return Vheight; }
		int  size() const;
};


#endif
