/*
 * Resources.cpp
 *
 *  Created on: Feb 25, 2012
 *      Author: andrew
 */

#include "Resources.hpp"
#include "sasteroids.h"

ScreenBitmap Backdrops[NUM_BACKS];

ScreenBitmap titleScreen;
ScreenBitmap extraLives; // 1/2 scale ship for extra men.

Mix_Chunk *soundSamples[NUM_SOUNDS]; // Sound!

ScreenBitmap bmp_asteroid_large;
ScreenBitmap bmp_asteroid_medium;
ScreenBitmap bmp_asteroid_small;
ScreenBitmap bmp_asteroid_evil_small;

ScreenBitmap bmp_enemy;
ScreenBitmap bmp_bullet;
ScreenBitmap bmp_evil_bullet;
ScreenBitmap bmp_spinner;
ScreenBitmap bmp_shield;
ScreenBitmap bmp_powerup_wmax;
ScreenBitmap bmp_powerup_weng;
ScreenBitmap bmp_powerup_wthree;
ScreenBitmap bmp_powerup_wshield;
ScreenBitmap bmp_powerup_wshield_charge;

ScreenBitmap bmp_player;
ScreenBitmap bmp_player_thrust;

//////////////////////////////////////////////////////
// Set up the array of game objects.
void LoadBitmaps()
{
    bmp_asteroid_small.load_image( "graphics/smallast.png" );
    bmp_asteroid_evil_small.load_image( "graphics/esmallast.png" );
    bmp_asteroid_medium.load_image( "graphics/medast.png" );
    bmp_asteroid_large.load_image( "graphics/bigast.png" );

    Backdrops[0].load_image( "graphics/back1.jpg" );
    Backdrops[1].load_image( "graphics/back2.jpg" );
    Backdrops[2].load_image( "graphics/back3.jpg" );
    Backdrops[3].load_image( "graphics/back1.jpg" );
    Backdrops[4].load_image( "graphics/back2.jpg" );

    if( !ClassicMode )
        titleScreen.load_image( "graphics/title.png" );
    else
        titleScreen.load_image( "graphics/title2.png" );

    bmp_enemy.load_image( "graphics/enemy.png" );
    bmp_bullet.load_image( "graphics/bullet.png" );
    bmp_evil_bullet.load_image( "graphics/bullet2.png" );
    bmp_spinner.load_image( "graphics/spinner.png" );
    bmp_shield.load_image( "graphics/shield.png" );
    bmp_powerup_wmax.load_image( "graphics/wMaxPowerUp.png" );
    bmp_powerup_weng.load_image( "graphics/wRechargePowerUp.png" );
    bmp_powerup_wthree.load_image( "graphics/wThreePowerUp.png" );
    bmp_powerup_wshield.load_image( "graphics/wShieldPowerUp.png" );
    bmp_powerup_wshield_charge.load_image( "graphics/wShieldRecharge.png" );

    bmp_player.load_image( "graphics/ship0.png" );
    bmp_player_thrust.load_image( "graphics/ship1.png" );
    bmp_spinner.load_image( "graphics/spinner.png" );

    extraLives.load_image( "graphics/ship0.png" );
}

//////////////////////////////////////////////////////////
// Load ze Wavs!
void LoadWavs()
{
    soundSamples[SND_BOOM_A] = Mix_LoadWAV( "sounds/boom1.wav");
    soundSamples[SND_BOOM_B] = Mix_LoadWAV( "sounds/boom2.wav");
    soundSamples[SND_BOOM_C] = Mix_LoadWAV( "sounds/shipexplode.wav");
    soundSamples[SND_FIRE] = Mix_LoadWAV( "sounds/zap.wav");
    soundSamples[SND_WARP] = Mix_LoadWAV( "sounds/warp.wav");
    soundSamples[SND_ENEMY] = Mix_LoadWAV( "sounds/flash.wav");
    soundSamples[SND_POWERUP] = Mix_LoadWAV( "sounds/powerup.wav");
    soundSamples[SND_ENGINE] = Mix_LoadWAV( "sounds/engine.wav");
}

