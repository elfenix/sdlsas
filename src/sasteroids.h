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

#define NUM_BITMAPS 13
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
#define P_SHLD   12
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
int LevelOdds(int lvlMax, int maxChance = 1, int lvlStep = 1);


const int SND_BOOM_A = 0;
const int SND_BOOM_B = 1;
const int SND_BOOM_C = 2;
const int SND_FIRE = 3;
const int SND_WARP = 4;


// Constants. (tune game here)
const int   GAME_CLOCK    = 33;           // Number of msec(33) between ticks.
const float START_DIST    = 70;           // Disance asteroids start from ship
const int   MAXASTEROIDS  = 10;           // Max # Asteroids

const float MassBigAst    = 8.0f;         // Masses for bouncing + collision
const float MassSmallAst  = 4.0f;
const float MassMedAst    = 6.0f;
const float MassShip      = 5.0f / 4.0f;
const float MassEnemy     = 1.0f;
const float MassBullet    = 1.0f / 8.0f;
const float MassSpinner   = 2.0f;

const float BUL_SPEED     = 7.5f;

class SBitmap;
class Ship;

extern SBitmap Gbit[NUM_BITMAPS];
extern SBitmap extraLives;
extern SBitmap Gbackdrop;
extern SBitmap titleScreen;
extern int ClassicMode;
extern Ship PlayerShip;

extern int score, Glevel, numasts, oldscore;
extern int BackdropOn, wantFullScreen;

extern int canShootThree;
extern int smartZapper;
extern int deathTimer;

extern int eeggU, eeggD, eeggL, eeggR, eeggS;

extern char HiScoreStrings[10][10];
extern int HiScoreNumbers[10];

#ifdef HAV_SOUND
extern Mix_Chunk *soundSamples[8];
#endif

void PlaySound(int soundNumber);
void upscore(int inc);
void HitSpinner(int number, int killedBy);
void HitEnemy(int number, int killedBy);
void BounceObjects(int i, int j);
void CleanUpStuff();
void GenerateAsteroids();
int MoveObjects();
void LoadBitmaps();

#endif
