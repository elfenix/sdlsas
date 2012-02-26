/*
 *
 * Copyright 2012 Andrew Mulbrook
 * Copyright 1994 Brad Pitzel
 *
 * Feel free to use/distribute/modify as long as credit/copyrights for myself
 * are included.
 */

#ifndef _SASTEROIDS
#define _SASTEROIDS

#include <cstdio>
#include <cmath>
#include <ctime>
#include <iostream>

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>

#include <math.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL_image.h>
#include <SDL_joystick.h>

#include "gameconf.h"
#include "Resources.hpp"

#ifdef _WIN32
#define M_PI 3.14159265358979323846
#endif


#define JOY_A 0
#define JOY_B 1
#define JOY_C 2
#define JOY_D 3
#define JOY_E 4
#define JOY_X 0
#define JOY_Y 1


// Constants. (tune game here)
const int   GAME_CLOCK    = 33;           // Number of msec(33) between ticks.

#include "vector.h"

class ScreenBitmap;
class Ship;
class GameEntity;
class Player;

extern int ClassicMode;
extern Player PlayerShip;

extern int score, Glevel, numasts, oldscore;
extern int BackdropOn, wantFullScreen;


extern int eeggU, eeggD, eeggL, eeggR, eeggS;

extern char HiScoreStrings[10][10];
extern int HiScoreNumbers[10];

extern char globalMessage[256];
extern int gMsgTimeLeft;


void MakeGlobalMessage(char* Tstring);
int LevelOdds(int lvlMax, int maxChance, int lvlStep = 1);



#include "fastmath.h"
#include "vector.h"
#include "ui.h"
#include "ScreenBitmap.hpp"
#include "fastmath.h"
#include "GameEntity.hpp"
#include "GraphicsMenu.hpp"



#endif
