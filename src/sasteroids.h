// Copyright 1994 Brad Pitzel
//
// Feel free to use/distribute/modify as long as credit/copyrights for myself 
// are included.

#ifndef __SAST__
#define __SAST__

#include "bitmap.h"
#include "sbitmap.h"
#include "cfclass.h"
#include "fastmath.h"
#include "ui.h"
#include "obj.h"

#define VERSION "SDL Sasteroids"
#define BINDIR "/usr/games/"

// load 'file' from Gcf into bitmap b.
void LoadBitmap( SBitmap &b, const char* file);

#define SMALLAST 0
#define MEDAST   1
#define BIGAST   2
#define ENEMY    3
#define BULLET	 4
#define BULLET2  5
#define SPINNER  6
#define SHIELD   7

extern SBitmap Gbit[8];

#endif
