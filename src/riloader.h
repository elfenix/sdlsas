// Copyright 1994 Brad Pitzel
//
// Feel free to use/distribute/modify as long as credit/copyrights for myself 
// and contributors are included.

// File   : RawImageLoader.h[1.0]
// Name   : gamelib1.0
// Status : proposed
// Changed: Sun Jun 12 22:19:28 1994

#ifndef __RawImageLoader__
#define __RawImageLoader__

#include <stdlib.h>
#include "bitmap.h"
#include "cfclass.h"

#include <iostream>
using namespace std;


// The RawImageLoader class demonstrates a class derived from ImageLoader
// to load in "version 4 HSI raw image files" stored in a .cf file.

// this is the header for version 4 HSI raw files
struct RawHeader
{
//	public:
	 char	magic[6];
    	 char	version[2];
    	 char	width[2];
	 char	height[2];
    	 char	palSize[2];
    	 char	Hdpi[2];
	 char	Vdpi[2];
    	 char	gamma[2];
    	 char	reserved[12];
    };


class RawImageLoader : public ImageLoader
    {
    public:
	RawImageLoader( cfc &cfile ) : Vcfile( &cfile )
		{};

	// by default, load both image and palette.
	virtual int	loadFile(  const char *file, char flags );

	int	loadHeader( const char *file);
	void	displayHeader();
	
    private:    
    	cfc	*Vcfile;

	
    	union 
    		{
		RawHeader	header;
    		 char	headerBuf[ sizeof(RawHeader) ];
    		};

	static char Vmagic[];

	// to ensure no endian ordering problems, this will convert
	// the 2 byte numbers from the header into integers.
	int	bti( const  char p[2] ) const;

    };

#endif
