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
    tchd = 0;
    position += velocity;
    
    vlimit = velocity.lengthSqrd();

    if (vlimit > maxspeed) {
      velocity /= sqrt(vlimit);
      velocity *= sqrt(maxspeed);
    }

    if(bounce > 0) {
      bounce--;
    } else {
      velocity += accelleration;
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


void ScreenObject::trim()
{
  float vlimit = velocity.lengthSqrd();

  if (vlimit > maxspeed) {
    velocity /= sqrt(vlimit);
    velocity *= sqrt(maxspeed);
  }
  
  if(isnan(velocity.length())) {
    velocity.SetXY(0.3f, 0.3f); // just so things don't really feck up.
  }
}


void ScreenObject::setbounce()
{
  bounce = 15;
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


///////////////////////////////////////////////////////
// Kill an asteroid - and create new ones if needed.
void KillAsteroid(int number, int killedBy, bool killChildren = false)
{
  int j, ctype;
  Explosion* explode;

  if(ObjectList[number]->alive()) {
    // play a sound. :)
    PlaySound(SND_BOOM_B);

    // up the points, but only if WE killed the asteroid
    if(killedBy != 0 && ObjectList[killedBy]) {
      if(ObjectList[killedBy]->type() == SHIP_BUL) {
	if(ObjectList[number]->type() == BIGAST && numasts == 1)
	  upscore(1000*Glevel);
	else
	  upscore(25);

	if(ObjectList[number]->type() == MEDAST && numasts == 1) 
	  upscore(500*Glevel);
	else
	  upscore(50);
	
	if(ObjectList[number]->type() == SMALLAST) 
	  upscore(100);
      }
    }
    

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
      } while( abs(rA2 - rA1) < 10 && abs(rA2 - (rA1+124)) < 10);      
      rA1 = int(rA2 + 128) % 255;
      CreateAsteroid( px, py,
		      FastMath::sin(rA1) * 1.0f + vx,
		      FastMath::cos(rA1) * 1.0f + vy, ctype );
      if(ctype == MEDAST && !(rand()%LevelOdds(16,7))) {
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





//////////////////////////////////////////////////////////
// Create an asteroid - does no checking of variables, just makes it
int CreateAsteroid(float x, float y, float xv, float yv, int type)
{
    ScreenObject *newobject;
    int openObject;

    
    openObject = GetOpenObject();
    if (openObject == -1)
	return -1;

    newobject = new ScreenObject(type);
    if (!newobject)
	return -1;

    numasts++;

    if((type == SMALLAST && !(rand()%LevelOdds(30,7))) && !ClassicMode
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
      newobject->setmass(MassBigAst);
      newobject->SetMaxSpeed(BigAstMax);
      break;
    case SMALLAST:
    case ESMAST:
      newobject->SetSize(10, 10);
      newobject->setsize(7);
      newobject->setmass(MassSmallAst);
      newobject->SetMaxSpeed(SmallAstMax);
      break;
    case MEDAST:
      newobject->setsize(10);
      newobject->SetSize(20,20);
      newobject->setmass(MassMedAst);
      newobject->SetMaxSpeed(MedAstMax);
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
    canShootThree = 1;
    break;
  case P_SHLD:
    PlayerShip.shieldAdd(50);
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
      	KillAsteroid(j, i);
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
      KillAsteroid(j, i);
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
    if(ObjectList[j]->type() == BULLET2) break;
    if(ObjectList[j]->type() == P_TYPE) break;
    BounceObjects(i, j);
    break;
  case SPINNER:
    Spinner* t = (Spinner*)ObjectList[i];
    if(t->morph()!=-1) break;
    if(ObjectList[j]->type() == P_TYPE) break;
    BounceObjects(i, j);
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

  Vector ij(ObjectList[j]->GetCenX()-ObjectList[i]->GetCenX(),
	      ObjectList[j]->GetCenY()-ObjectList[i]->GetCenY());
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

