/*
 * Resources.hpp
 *
 *  Created on: Feb 25, 2012
 *      Author: andrew
 */

#ifndef RESOURCES_HPP_
#define RESOURCES_HPP_

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include "ScreenBitmap.hpp"
#include "vector.h"

static const int SND_BOOM_A = 0;
static const int SND_BOOM_B = 1;
static const int SND_BOOM_C = 2;
static const int SND_FIRE = 3;
static const int SND_WARP = 4;
static const int SND_ENEMY = 5;
static const int SND_POWERUP = 6;
static const int SND_ENGINE = 7;
static const int NUM_SOUNDS = 8;
static const int NUM_BACKS = 5;

extern Vector ScreenLimits;
extern ScreenBitmap extraLives;
extern ScreenBitmap Backdrops[NUM_BACKS];
extern ScreenBitmap titleScreen;
extern Mix_Chunk *soundSamples[NUM_SOUNDS];

extern ScreenBitmap bmp_asteroid_large;
extern ScreenBitmap bmp_asteroid_medium;
extern ScreenBitmap bmp_asteroid_small;
extern ScreenBitmap bmp_asteroid_evil_small;

extern ScreenBitmap bmp_enemy;
extern ScreenBitmap bmp_bullet;
extern ScreenBitmap bmp_evil_bullet;
extern ScreenBitmap bmp_spinner;
extern ScreenBitmap bmp_shield;
extern ScreenBitmap bmp_powerup_wmax;
extern ScreenBitmap bmp_powerup_weng;
extern ScreenBitmap bmp_powerup_wthree;
extern ScreenBitmap bmp_powerup_wshield;
extern ScreenBitmap bmp_powerup_wshield_charge;

extern ScreenBitmap bmp_player;
extern ScreenBitmap bmp_player_thrust;

void LoadBitmaps();
void LoadWavs();
void PlaySound( int soundNumber );

#endif /* RESOURCES_HPP_ */
