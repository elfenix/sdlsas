// Copyright 1994 Brad Pitzel
// 
// Feel free to use/distribute/modify as long as credit/copyrights for
// myself 
// are included.

#include "obj.h"
#include "ui.h"
#include "sbitmap.h"

// Global
SBitmap spinnerBitmaps[SHIPTURNS];
SBitmap morphBitmaps[10];
SBitmap ShipBitmaps[SHIPTURNS];
SBitmap ShipThrusts[SHIPTURNS];

Vector ScreenLimits(PLAY_X, PLAY_Y);

///////////////////////////////////////////////////////////
// Screen Object class w00t!

ScreenObject::ScreenObject()
{
  wrapMoves = false;
  isAlive = false;
  position.SetXY(0, 0);
  velocity.SetXY(0, 0);
  accelleration.SetXY(0, 0);
  mysprite = 0;
  maxspeed = 64.0f;
  objtype = 0;
  mysize = 1;
}


ScreenObject::ScreenObject(int type)
{
  wrapMoves = false;
  isAlive = false;
  position.SetXY(0, 0);
  velocity.SetXY(0, 0);
  accelleration.SetXY(0, 0);
  mysprite = 0;
  maxspeed = 32.0f;
  mysize = 1;
  objtype = type;
}


ScreenObject::~ScreenObject()
{
  // not really anything to do ....
}


// Draw the object, with/without wrapping
void ScreenObject::draw()
{
  if (isAlive)
    mysprite->put((int) position.GetX(), (int) position.GetY());
}


void ScreenObject::tick()
{
    float tx, ty;
    float vlimit;

    // Good enough for a game..
    position += velocity;
    velocity += accelleration;

    vlimit = velocity.lengthSqrd();

    if (vlimit > maxspeed) {
	velocity -= accelleration;
	accelleration.SetXY(0.0f, 0.0f);
	velocity *= sqrt(maxspeed);
	velocity /= sqrt(vlimit);
    }

    // Look to see if we are visible...
    if (position.GetX() > ScreenLimits.GetX() ||
	position.GetY() > ScreenLimits.GetY()) {
	if (!wrapMoves)
	    die();		// If we don't wrap, we die...
	else {
	    tx = position.GetX();
	    ty = position.GetY();
	    if (tx > ScreenLimits.GetX())
		tx = 0.1f-size.GetX();
	    if (ty > ScreenLimits.GetY())
		ty = 0.1f-size.GetY();
	    position.SetXY(tx, ty);
	}
    }


    if (position.GetX() < (0.0f-size.GetX()) || 
	position.GetY() < (0.0f-size.GetY())) {
	if (!wrapMoves)
	    die();
	else {
	    tx = position.GetX();
	    ty = position.GetY();

	    if (tx < (1.0f - size.GetX()))
		tx = ScreenLimits.GetX() - 0.1f;

	    if (ty < (1.0f - size.GetY()))
		ty = ScreenLimits.GetY() - 0.1f;

	    position.SetXY(tx, ty);
	}
    }
}


void ScreenObject::die() 
{
  isAlive = false;
}


void ScreenObject::restore()
{
  isAlive = true;
}

void ScreenObject::SetBitmap(SBitmap* sprite)
{
  mysprite = sprite;
  myCenXRef = float(sprite->width())/2.0f; 
  myCenYRef = float(sprite->height())/2.0f; 
}


void ScreenObject::randomSpeed(float start, float stop)
{
  int myMod, rRet;
  float ttmp, ttmp2;
  myMod = int((stop - start) * 100.0f);
  if(myMod < 0) myMod = -myMod;
  rRet = rand() % myMod;
  ttmp = float(rRet) / (100.0f);
  ttmp2 = velocity.length();
  ttmp2 = 1 / ttmp2;
  velocity = velocity * ttmp;
  velocity = velocity * ttmp2;
}


void ScreenObject::randomDir(float magn)
{
  int angle = rand() % 255;
  velocity.SetXY(FastMath::sin(angle)*magn, 
		 FastMath::cos(angle)*magn);
}



// Rotate the pixmaps for all of the ships, spinners, etc... that rotate
// around. =)
void InitShips()
{
  int i, j, k;
  Angle angle = 0;
  
  k = SHIPTURNS >> 2;		// SHIPTURNS must be multiple of 4
  
  for (j = 0; j < k; j++) {
   ShipBitmaps[j].LoadImage(BINDIR "/graphics/ship0.bmp");
   ShipThrusts[j].LoadImage(BINDIR "/graphics/ship1.bmp");
   spinnerBitmaps[j].LoadImage(BINDIR "/graphics/spinner.bmp");
   
   // since our angles are 0..255, 64 is 90 normal degrees
   angle = j * 64 / k;;
   
   if (angle != 0) {
     ShipBitmaps[j].rot(angle);
     ShipThrusts[j].rot(angle);
     spinnerBitmaps[j].rot(angle);
   }
   
   for (i = 1; i < 4; i++) {
     ShipBitmaps[j + i * k].copy(ShipBitmaps[j + (i - 1) * k]);
     ShipBitmaps[j + i * k].rotc90();
     ShipThrusts[j + i * k].copy(ShipThrusts[j + (i - 1) * k]);
     ShipThrusts[j + i * k].rotc90();
     spinnerBitmaps[j + i * k].copy(spinnerBitmaps[j + (i - 1) * k]);
     spinnerBitmaps[j + i * k].rotc90();
   }
  }
}






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


// //////////////////////////////////////////////////////////////////////////
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


///////////////////////////////////////////
// Loop through and move every object. 
void AllObjectsTick()
{
  for (int i = 0; i < MAX_OBJECTS; i++) {
    if (ObjectList[i]) {
      ObjectList[i]->tick();
      
      if (!ObjectList[i]->alive()) {
	delete ObjectList[i];
	ObjectList[i] = 0;     
      }
    }
  }
}



///////////////////////////////////////////////////////
// Kill an asteroid - and create new ones if needed.
void KillAsteroid(int number, int killedBy, bool killChildren = false)
{
  int j, ctype;
  Explosion* explode;

  if(ObjectList[number]->alive()) {
    // play a sound. :)
    PlaySound(SND_BOOM_B);

    ObjectList[number]->die();    
    numasts -= 1;
    
    // this asteroid is dead now
    // explode the original asteroid!
    j = GetOpenObject();
    if(j == -1) return;
    explode = new Explosion(ObjectList[number]->GetX(),
			    ObjectList[number]->GetY(),
			    ObjectList[number]->VelX(),
			    ObjectList[number]->VelY());
    ObjectList[j] = explode;

    // Check if we need to do something special when the asteroid dies
    if(ObjectList[number]->type() == ESMAST) {
      unsigned char angle; 

      for(angle = 0; angle < 220; angle += 30) {
	float fsin = FastMath::sin(angle) * 4.0f;
	float fcos = FastMath::cos(angle) * 4.0f;
	int j = GetOpenObject();
	if(j == -1) return;
	ObjectList[j] = new ScreenObject;
	ObjectList[j]->restore();
	ObjectList[j]->SetVel(fsin, fcos);
	ObjectList[j]->SetXY(ObjectList[number]->GetX(), 
			     ObjectList[number]->GetY());
	ObjectList[j]->SetAcc(0.0f, 0.0f);
	ObjectList[j]->SetWrapMoves(false);
	ObjectList[j]->SetMaxSpeed(255.0f);
	ObjectList[j]->settype(BULLET2);
	ObjectList[j]->SetBitmap(&Gbit[BULLET2]);
	ObjectList[j]->setsize(5);
      } 
    }



    // up the points, but only if WE killed the asteroid
    if(killedBy != 0 && ObjectList[killedBy]) {
      if(ObjectList[killedBy]->type() == SHIP_BUL) {
	if(ObjectList[number]->type() == BIGAST) upscore(500);
	if(ObjectList[number]->type() == MEDAST) upscore(250);
	if(ObjectList[number]->type() == SMALLAST) upscore(100);
      }
    }
    
    // figure out the new type of asteroids to create.
    if(ObjectList[number]->type() == BIGAST) ctype = MEDAST;
    else if(ObjectList[number]->type() == MEDAST) ctype = SMALLAST;
    else ctype = 255;
    
    // create the asteroids
    // when creating the asteroid we blast 2 at a time in opposite
    // directions. TODO: Make this a more correct in terms of physics.
    if(ctype != 255 && !killChildren) {
      char rA1, rA2;
      float px, py, vx, vy;
      
      rA1 = rand() % 255; 
      rA2 = int(rA1 + 128) % 255;
      
      px = ObjectList[number]->GetX();
      py = ObjectList[number]->GetY();
      vx = ObjectList[number]->VelX();
      vy = ObjectList[number]->VelY();
         
      CreateAsteroid( px, py, 
		      FastMath::sin(rA1) * 1.0f + vx, 
		      FastMath::cos(rA1) * 1.0f + vy, ctype );     
      CreateAsteroid( px, py,
		      FastMath::sin(rA2) * 1.0f + vx,
		      FastMath::cos(rA2) * 1.0f + vy, ctype );

      do {
	rA2 = rand() % 255;
      } while( abs(rA2 - rA1) < 5 && abs(rA2 - (rA1+124)) < 5);      
      rA1 = int(rA2 + 128) % 255;
      CreateAsteroid( px, py,
		      FastMath::sin(rA1) * 1.0f + vx,
		      FastMath::cos(rA1) * 1.0f + vy, ctype );
      if(ctype == MEDAST && !(rand()%LevelOdds(16,4))) {
	j = GetOpenObject();
	ObjectList[j] = new Spinner;
	ObjectList[j]->SetXY(px, py);
	ObjectList[j]->SetVel(FastMath::sin(rA2)*1.0f + vx, 
			      FastMath::cos(rA2)*1.0f + vy);
      } else {
	CreateAsteroid( px, py, 
			FastMath::sin(rA2) * 1.0f + vx,
			FastMath::cos(rA2) * 1.0f + vy, ctype );
      }
    }   
  }
}





/////////////////////////////////////////////////////////////////////////////
// Create an asteroid - does no checking of variables, just makes it
int CreateAsteroid(float x, float y, float xv, float yv, int type)
{
    ScreenObject *newobject;
    int openObject;

    
    openObject = GetOpenObject();
    if (openObject == -1)
	return -1;
    numasts++;

    newobject = new ScreenObject(type);
    if (!newobject)
	return -1;

    if((type == SMALLAST && !(rand()%LevelOdds(30,3))) && !ClassicMode
       && (Glevel > 3))
      {
	type = ESMAST;
      }

    newobject->SetXY(x, y);
    newobject->SetAcc(0.0f, 0.0f);
    newobject->SetVel(xv, yv);
    newobject->SetBitmap(&Gbit[type]);
    newobject->settype(type);
    newobject->SetWrapMoves(true);
    newobject->restore();

    switch(type) {
    case BIGAST:
      newobject->SetSize(40, 40);
      newobject->setsize(20);
      break;
    case SMALLAST:
    case ESMAST:
      newobject->SetSize(10, 10);
      newobject->setsize(7);
      break;
    case MEDAST:
      newobject->setsize(10);
      newobject->SetSize(20,20);
      break;
    }

    ObjectList[openObject] = newobject;
    return openObject;
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

	obj = CreateAsteroid(x, y, 0, 0, BIGAST);

	if(ObjectList[obj]) {
	  ObjectList[obj]->randomDir(0.40f);
	}

	if(!(rand()%3) && !ClassicMode) {
	  do {
	    x = (float) (rand() % 320);
	    y = (float) (rand() % 200);
	    temp.SetXY(x, y);
	  }
	  while (PlayerShip.GetXYVec().length(temp) < START_DIST);
	  obj = CreateAsteroid(x, y, 0, 0, MEDAST);
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
	  obj = CreateAsteroid(x, y, 0, 0, SMALLAST);
	  if(ObjectList[obj]) {
	    ObjectList[obj]->randomDir(1.40f);
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

  ptype = iobj->effect();
  switch(ptype) {
  case P_WMAX:
    PlayerShip.addMaxPower(20);
    break;
  case P_WENG:
    PlayerShip.addRegPower(2);
    break;
  case P_WTHR:
    canShootThree = 1;
    break;
  case P_SHLD:
    PlayerShip.shieldAdd(50);
    break;
  }
}




// /////////////////////////////////////////////////////////////////////////
// Move all the game objects, check for collisions... etc...
int MoveObjects()
{
  
  int i = 0, crash = 0, touched = 0;
  
  AllObjectsTick();
  
  // Loop through every object.
  for (i = 1; i < MAX_OBJECTS; i++) {
    if (ObjectList[i]) {
      
      // Check for ship collision....
      if (collide(*ObjectList[i], *ObjectList[0])
	  && ObjectList[i]->type() != 254
	  && ObjectList[i]->type() != 255
	  && ObjectList[i]->alive()) {
	crash = 1;
	if(PlayerShip.shielded()) {
	  PlayerShip.SetBounce();
	  crash = 0; touched--;
	} else	if(!PlayerShip.isDeadStick() && !touched) {
	  switch(ObjectList[i]->type()) {
	  case SMALLAST:
	  case MEDAST:
	  case BIGAST:
	  case ESMAST:
	    KillAsteroid(i, 0);
	    break;
	  case SPINNER:
	    HitSpinner(i, 0);
	    HitSpinner(i, 0);
	    HitSpinner(i, 0);
	    HitSpinner(i, 0);
	    HitSpinner(i, 0);
	    break;
	  case BULLET2:
	    ObjectList[i]->die();
	    break;
	  case P_TYPE:
	    ObjectList[i]->die();
	    PowerUpF(i);
	    crash = 0; touched--;
	  } 
	  touched++;
	}
      }
      
      // Check for asteroid collisions... etc...
      for (int j = 1; j < MAX_OBJECTS; j++) {
	if (ObjectList[j]) {
	  
	  // Check every object against every other here 
	  if (i == j)
	    continue;	/* Don't check against self */
	  
	  if (ObjectList[i]->alive() && ObjectList[j]->alive()
	      && (ObjectList[i]->type() == 254 || 
		  ObjectList[i]->type() == BULLET2)
	      && collide(*ObjectList[i], *ObjectList[j])) {
	    switch (ObjectList[j]->type()) {
	    case SMALLAST:
	    case BIGAST:
	    case MEDAST:
	    case ESMAST:
	      KillAsteroid(j, i);
	      ObjectList[i]->die();
	      break;
	    case SPINNER:
	      HitSpinner(j, i);
	      ObjectList[i]->die();
	      break;
	    case ENEMY:
	      if(ObjectList[i]->type() != BULLET2) 
		HitEnemy(j, i);
	    }
	  } else {
	    if(ObjectList[i]->type() == SPINNER || 
	       ObjectList[j]->type() == SPINNER) {
	      BounceObjects(i, j);
	    }    
	  }
	  
	} // end if(ObjectList[i])
      }
      
    }
  }
  
  return crash;
}


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
	KillAsteroid(i, 0);
      } 
    }
  } 
  
  canShootThree = 1;
  deathTimer = 128;
}




//////////////////////////////////////////////////////
// Set up the array of game objects.
void LoadBitmaps()
{
  Gbit[SMALLAST].LoadImage(BINDIR "/graphics/smallast.bmp");
  Gbit[ESMAST].LoadImage(BINDIR "/graphics/esmallast.bmp");
  Gbit[MEDAST].LoadImage(BINDIR "/graphics/medast.bmp");
  Gbit[BIGAST].LoadImage(BINDIR "/graphics/bigast.bmp");
  Gbackdrop.LoadImage(BINDIR "/graphics/back1.bmp", false);
  if(!ClassicMode)
    titleScreen.LoadImage(BINDIR "/graphics/title.bmp", false);
  else
    titleScreen.LoadImage(BINDIR "/graphics/title2.bmp", false);
  Gbit[ENEMY].LoadImage(BINDIR "/graphics/enemy.bmp");
  Gbit[BULLET].LoadImage(BINDIR "/graphics/bullet.bmp");
  Gbit[BULLET2].LoadImage(BINDIR "/graphics/bullet2.bmp");
  Gbit[SPINNER].LoadImage(BINDIR "/graphics/spinner.bmp");
  Gbit[SHIELD].LoadImage(BINDIR "/graphics/shield.bmp");
  Gbit[P_WMAX].LoadImage(BINDIR "/graphics/wMaxPowerUp.bmp");
  Gbit[P_WENG].LoadImage(BINDIR "/graphics/wRechargePowerUp.bmp");
  Gbit[P_WTHR].LoadImage(BINDIR "/graphics/wThreePowerUp.bmp");
  Gbit[P_SHLD].LoadImage(BINDIR "/graphics/wShieldPowerUp.bmp");
  extraLives.LoadImage(BINDIR "/graphics/ship0.bmp");
  extraLives.scalep5();
  extraLives.SetTrans(true);
  morphBitmaps[0].LoadImage(BINDIR "/graphics/medast.bmp");
  morphBitmaps[1].LoadImage(BINDIR "/graphics/morph1.bmp");
  morphBitmaps[2].LoadImage(BINDIR "/graphics/morph2.bmp");
  morphBitmaps[3].LoadImage(BINDIR "/graphics/morph3.bmp");
  morphBitmaps[4].LoadImage(BINDIR "/graphics/morph4.bmp");
  morphBitmaps[5].LoadImage(BINDIR "/graphics/morph5.bmp");
  morphBitmaps[6].LoadImage(BINDIR "/graphics/morph6.bmp");
  morphBitmaps[7].LoadImage(BINDIR "/graphics/morph7.bmp");
  morphBitmaps[8].LoadImage(BINDIR "/graphics/morph8.bmp");
  morphBitmaps[9].LoadImage(BINDIR "/graphics/morph9.bmp"); 
  
  InitShips();
}



//////////////////////////////////////////////////////
// Bounce Objects? Why Yes!
void BounceObjects(int i, int j) 
{
  // TODO: BOUNCE OBJECTS
}

