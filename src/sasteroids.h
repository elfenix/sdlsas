// Copyright 1994 Brad Pitzel
//
// Feel free to use/distribute/modify as long as credit/copyrights for myself 
// are included.

#ifndef __SAST__
#define __SAST__

#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "gameconf.h"

#ifdef HAVE_SOUND
#include <SDL/SDL_mixer.h>
#endif


#ifdef _OLD_CPP_HEADERS
#include <iostream.h>
#else
#include <iostream>
using namespace std;
#endif

#ifdef _WIN32
#define M_PI 3.14159265358979323846
#else
#include <termios.h>
#include <glob.h>
#endif

#define NUM_BITMAPS 12
#define SMALLAST 0
#define MEDAST   1
#define BIGAST   2
#define ENEMY    3
#define BULLET	 4
#define BULLET2  5
#define SPINNER  6
#define SHIELD   7
#define P_WMAX   8
#define P_WENG   9
#define P_WTHR   10
#define ESMAST   11
#define SHIP_BUL 254

#define P_TYPE   8


#include "fastmath.h"
#include "vector.h"
#include "ui.h"
#include "sbitmap.h"
#include "fastmath.h"
#include "obj.h"
#include "sasteroids.h"

// load 'file' from Gcf into bitmap b.
void LoadBitmap( SBitmap &b, const char* file);

extern SBitmap Gbit[NUM_BITMAPS];
extern SBitmap extraLives;

#endif
