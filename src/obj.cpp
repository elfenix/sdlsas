// Copyright 1994 Brad Pitzel
// 
// Feel free to use/distribute/modify as long as credit/copyrights for
// myself 
// are included.

#include "obj.h"
#include "ui.h"
#include "sbitmap.h"
#include "asteroid.h"


// Global
SBitmap morphBitmaps[10];

Vector ScreenLimits(PLAY_X, PLAY_Y);

ScreenObject *ObjectList[MAX_OBJECTS];

// Initialize the list, should only be called at the begining of the
// program
void SetupObjArray()
{
  int i;
  
  for (i = 1; i < MAX_OBJECTS; i++) {
    ObjectList[i] = 0;
  }
  ObjectList[0] = &PlayerShip;
}

///////////////////////////////////////////////////////////////////////////
// Free the list
void FreeObjArray()
{
  int i;
  
  ObjectList[0] = &PlayerShip;
  for (i = 1; i < MAX_OBJECTS; i++) {
    if (ObjectList[i])
	    delete ObjectList[i];
    
	ObjectList[i] = 0;
  }
}


///////////////////////////////////////////////////////////////////////
// Get an open object slot...
int GetOpenObject()
{
  int i;

  for (i = 1; i < MAX_OBJECTS; i++) {
    if (!ObjectList[i])
      return i;
  }
  
  return -1;
}


////////////////////////////////////////////
// Draw objects
void DrawObjects()
{
  int i;  
  for (i = 0; i < MAX_OBJECTS; i++) {
    if (ObjectList[i] && ObjectList[i]->alive()) {
      ObjectList[i]->draw();
    }
  }
}


////////////////////////////////////////////////////////////////////////////
// create asteroids for beginning of each level/game
void GenerateAsteroids()
{
    int i, obj;
    float x, y;
    Vector temp;

    i = GetOpenObject();
    if(i == -1) return;

    for (i = 0; i <= (Glevel / 2 + 1) && i <= MAXASTEROIDS; i++) {
	do {
	    x = (float) (rand() % 320);
	    y = (float) (rand() % 200);
	    temp.SetXY(x, y);
	}
	while (PlayerShip.GetXYVec().length(temp) < START_DIST);

	obj = Asteroid::CreateAsteroid(x, y, 0, 0, BIGAST);

	if(ObjectList[obj]) {
	  ObjectList[obj]->randomDir(0.80f);
	}

	if(!(rand()%3) && !ClassicMode) {
	  do {
	    x = (float) (rand() % 640);
	    y = (float) (rand() % 400);
	    temp.SetXY(x, y);
	  }
	  while (PlayerShip.GetXYVec().length(temp) < START_DIST);
	  obj = Asteroid::CreateAsteroid(x, y, 0, 0, MEDAST);
	  if(ObjectList[obj]) {
	    ObjectList[obj]->randomDir(1.00f);
	  }
	}
	
	if(!(rand()%6) && !ClassicMode) {
	  do {
	    x = (float) (rand() % 320);
	    y = (float) (rand() % 200);
	    temp.SetXY(x, y);
	  }
	  while (PlayerShip.GetXYVec().length(temp) < START_DIST);
	  obj = Asteroid::CreateAsteroid(x, y, 0, 0, SMALLAST);
	  if(ObjectList[obj]) {
	    ObjectList[obj]->randomDir(2.80f);
	  }
	}	
    }
}


///////////////////////////////////////////////////////
// Do the PowerUps
void PowerUpF(int i)
{
  int ptype;
  PowerUp* iobj;

  iobj = (PowerUp*)ObjectList[i];
  if(!iobj) return;

  PlaySound(SND_POWERUP);
  ptype = iobj->effect();
  switch(ptype) {
  case P_WMAX:
    PlayerShip.addMaxPower(20);
    break;
  case P_WENG:
    PlayerShip.addRegPower(2);
    break;
  case P_WTHR:
    PlayerShip.canShootThree = 1;
    break;
  case P_SHLD:
    PlayerShip.shieldAdd(50);
    break;
  case P_SRCG:
    PlayerShip.canRecharge = 1;
    break;
  }
}


//////////////////////////////////////////////////////////////
// What to do when objects hit....
void ObjectsHit(int i, int j, int& touched, int& crash)
{
  int t;

  if(!ObjectList[i]->alive()) return;
  if(!ObjectList[j]->alive()) return;

  if(i == j) return;

  // First, Swap objects so we only need 1 set of code
  switch(ObjectList[i]->type()) {
  case SMALLAST:
  case MEDAST:
  case BIGAST:
  case ESMAST:
    t = i; i = j; j = t;
  }

  switch(ObjectList[j]->type()) {
  case SHIP_BUL:
  case BULLET2:
    t = i; i = j; j = t;
  }

  if(ObjectList[j]->type() == PLAYERSHIP) {
    t = i; i = j; j = t;
  }


  // Perform ze actions!
  switch(ObjectList[i]->type()) {
  case PLAYERSHIP:
    // Check to make sure the ship can actually collide.
    if(touched) break; 
    if(PlayerShip.isDeadStick()) break;
    switch(ObjectList[j]->type()) {
    case SHIP_BUL:
      break;
    case BULLET2:
      ObjectList[j]->die();
      if(!PlayerShip.shielded()) {
	crash = 1; touched++;
      }
      break;
    case SMALLAST:
    case MEDAST:
    case BIGAST:
    case ESMAST:
      if(!PlayerShip.shielded()) {
      	Asteroid::KillAsteroid(j, i);
      	crash = 1; touched++;
      } else {
	BounceObjects(i, j);
      }
      break;
    case SPINNER: 
      if(!PlayerShip.shielded()) { 
	HitSpinner(j, i);
	HitSpinner(j, i);
	HitSpinner(j, i);
	HitSpinner(j, i);
	HitSpinner(j, i);
	crash = 1; touched++;
      } else {
	BounceObjects(i, j); 
      }
      break; 
    case ENEMY:
      if(!PlayerShip.shielded()) {
	crash = 1; touched = 1;
	ObjectList[j]->die();
      } else {
	BounceObjects(i, j);
      }
      break;
    case P_TYPE:
      ObjectList[j]->die();
      PowerUpF(j);
      break;
    }
    break;
  case SHIP_BUL:
  case BULLET2:
    switch (ObjectList[j]->type()) {
    case SMALLAST:
    case BIGAST:
    case MEDAST:
    case ESMAST:
      Asteroid::KillAsteroid(j, i);
      ObjectList[i]->die();
      break;
    case SPINNER:
      HitSpinner(j, i);
      ObjectList[i]->die();
      break;
    case ENEMY:
      if(ObjectList[i]->type() != BULLET2) {
	HitEnemy(j, i);
	ObjectList[i]->die();
      }
    }
    break; 
  case ENEMY:	
    {
      Enemy* t = (Enemy*)ObjectList[i];
      if(ObjectList[j]->type() == BULLET2) break;
      if(ObjectList[j]->type() == P_TYPE) break;
      if(!(t->bounceStat() > 1)) {
	BounceObjects(i, j);
	t->setbounce();
      }
      break;
    }
  case SPINNER:
    {
      Spinner* t = (Spinner*)ObjectList[i];
      if(t->morph()!=-1) break;
      if(ObjectList[j]->type() == P_TYPE) break;
      if(!(t->bounceStat() > 1)) {
	BounceObjects(i, j);
	t->setbounce();
      }
      break;
    }
  }
}


//////////////////////////////////////////////////////////////
// Move all the game objects, check for collisions... etc...
int MoveObjects()
{
  
  int i = 0, crash = 0, touched = 0, j = 0;
  
  // Loop through every object.
  for (i = 0; i < MAX_OBJECTS; i++) {
    if(ObjectList[i] && ObjectList[i]->alive()) {
      ObjectList[i]->tick();
      
      for(j = 1; j < MAX_OBJECTS; j++) {
	if(!ObjectList[j]) continue;
	if(!ObjectList[j]->alive()) continue;

      	if(collide(ObjectList[i], ObjectList[j])) {
      	  ObjectsHit(i, j, touched, crash);
	}
      }
    } else if(ObjectList[i] && i != 0) {
      delete ObjectList[i];
      ObjectList[i] = 0;
    }
  }

  return crash;
}


///////////////////////////////////////////////////////
// Utility Function.
void CleanUpStuff() 
{
  int i;
  for(i = 1; i < MAX_OBJECTS; i++) {
    if(!ObjectList[i]) continue;
    if(ObjectList[i]->type() == MEDAST ||
       ObjectList[i]->type() == SMALLAST ||
       ObjectList[i]->type() == BIGAST) {
      if(numasts > 1) { 
	Asteroid::KillAsteroid(i, 0);
      } 
    }
  } 
  
  /* Give every power up */
  PlayerShip.canShootThree = 1;
  PlayerShip.canRecharge = 1;
  PlayerShip.canShootBack = 1;
  PlayerShip.canFireShield = 1;
 

  deathTimer = 128;
}


//////////////////////////////////////////////////////
// Set up the array of game objects.
void LoadBitmaps()
{
  Gbit[SMALLAST].LoadImage("graphics/smallast.png");
  Gbit[ESMAST].LoadImage  ("graphics/esmallast.png");
  Gbit[MEDAST].LoadImage  ("graphics/medast.png");
  Gbit[BIGAST].LoadImage  ("graphics/bigast.png");

  Backdrops[0].LoadImage("graphics/back1.jpg");
  Backdrops[1].LoadImage("graphics/back2.jpg");
  Backdrops[2].LoadImage("graphics/back3.jpg");
  Backdrops[3].LoadImage("graphics/back1.jpg");
  Backdrops[4].LoadImage("graphics/back2.jpg");
  

  if(!ClassicMode)
    titleScreen.LoadImage("graphics/title.png");
  else
    titleScreen.LoadImage("graphics/title2.png");

  Gbit[ENEMY  ].LoadImage("graphics/enemy.png");
  Gbit[BULLET ].LoadImage("graphics/bullet.png");
  Gbit[BULLET2].LoadImage("graphics/bullet2.png");
  Gbit[SPINNER].LoadImage("graphics/spinner.png");
  Gbit[SHIELD ].LoadImage("graphics/shield.png");
  Gbit[P_WMAX ].LoadImage("graphics/wMaxPowerUp.png");
  Gbit[P_WENG ].LoadImage("graphics/wRechargePowerUp.png");
  Gbit[P_WTHR ].LoadImage("graphics/wThreePowerUp.png");
  Gbit[P_SHLD ].LoadImage("graphics/wShieldPowerUp.png");
  Gbit[P_SRCG ].LoadImage("graphics/wShieldRecharge.png");
  
  Gbit[BIT_PLAYER ].LoadImage( "graphics/ship0.png");
  Gbit[BIT_PLAYERT].LoadImage( "graphics/ship1.png");
  Gbit[BIT_SPINNER].LoadImage( "graphics/spinner.png");

  extraLives.LoadImage("graphics/ship0.png");
}


//////////////////////////////////////////////////////////
// Load ze Wavs!
void LoadWavs()
{
#ifdef HAVE_SOUND
  soundSamples[SND_BOOM_A]  = Mix_LoadWAV( "sounds/boom1.wav");
  soundSamples[SND_BOOM_B]  = Mix_LoadWAV( "sounds/boom2.wav");
  soundSamples[SND_BOOM_C]  = Mix_LoadWAV( "sounds/shipexplode.wav");
  soundSamples[SND_FIRE]    = Mix_LoadWAV( "sounds/zap.wav");
  soundSamples[SND_WARP]    = Mix_LoadWAV( "sounds/warp.wav");
  soundSamples[SND_ENEMY]   = Mix_LoadWAV( "sounds/flash.wav");
  soundSamples[SND_POWERUP] = Mix_LoadWAV( "sounds/powerup.wav");
  soundSamples[SND_ENGINE]  = Mix_LoadWAV( "sounds/engine.wav");
#endif
}










//////////////////////////////////////////////////////
// Bounce Objects? Why Yes!
// This is the commonly used algo, you can pick up from
// any good physics tutorial. :) This does the exact
// same thing as the original, only it's a lot more 
// readable.
void BounceObjects(int i, int j) 
{
  float ti, tj;
  Vector vi, vj;
  Vector t1, t2;

  //  rcollide(ObjectList[i], ObjectList[j]);

  Vector ij(ObjectList[j]->GetX()-ObjectList[i]->GetX(),
	      ObjectList[j]->GetY()-ObjectList[i]->GetY());
  ij /= ij.length();
  
  if(isnan(ij.length())){
    return;
  }
  
  vi = ObjectList[i]->GetVel();
  vj = ObjectList[j]->GetVel();

  ti = vi * ij; 
  tj = vj * ij;

  float t3 = (2.0f*(ti-tj))/
    (ObjectList[i]->getMass()+ObjectList[j]->getMass());
  
  if(t3 < 0) t3 = -t3;
  
  t1 = vi;
  t1 -= (t3 * ObjectList[j]->getMass()) * ij;
  
  t2 = vj;
  t2 += (t3 * ObjectList[i]->getMass()) * ij;

  // Make sure we put in only valid numbers.
  if(isnan(t1.length()) || isnan(t2.length())) return;
    
  ObjectList[i]->SetVel(t1); 
  ObjectList[i]->trim();
  ObjectList[i]->setbounce();
  ObjectList[j]->SetVel(t2); 
  ObjectList[j]->trim();
  ObjectList[j]->setbounce();
}

