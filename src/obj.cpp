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

// /////////////////////////////////////////////////////////
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
		tx = 0.1f;
	    if (ty > ScreenLimits.GetY())
		ty = 0.1f;
	    position.SetXY(tx, ty);
	}
    }


    if (position.GetX() < (0.0f) || position.GetY() < (0.0f)) {
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


// ///////////////////////////////////////////////////////////////////////
// Ship Class....
void Ship::SetBitmap()
{
    if (thrustcnt)
	ScreenObject::SetBitmap(&ShipThrusts[pos]);
    else
	ScreenObject::SetBitmap(&ShipBitmaps[pos]);
}

void Ship::draw()
{
  if(!deadStick) {

    Gbit[SHIELD].put( position.GetX(), position.GetY() );

    /* TODO: Implement this as part of the shield.
    for(i = 0; i < 255; i++) {
      Ui::setpixel(
		int(GetCenX()*2.0f+FastMath::sin(i)*25),
		int(GetCenY()*2.0f+FastMath::cos(i)*25),
		255, 255, 0);
		} */

    ScreenObject::draw();	
  }
}

void Ship::tick()
{
    wrapMoves = 1;

    if (thrustcnt)
	thrustcnt--;
    if (!thrustcnt)
	accelleration.SetXY(0, 0);
    SetBitmap();
    ScreenObject::tick();
    if (shieldTimeLeft > 0 && shieldLives)
	shieldTimeLeft--;
    if (shieldTimeLeft <= 0)
	shieldLives = 0;
    if (wPower < 10)
	wPower++;
}

void Ship::shieldOn()
{
    if (shieldTimeLeft > 0)
	shieldLives = 1;
}

int Ship::shielded()
{
    return shieldLives;
}

void Ship::Hyper()
{
  if(!deadStick)
    SetXY((float) (rand() % Ui::WIDTH()), (float) (rand() % Ui::HEIGHT()));
}




/////////////////////////////////////////////
// Spinner Class (works a lot like ship class )

void Spinner::SetBitmap()
{
    if (morphStage >= 0)
	ScreenObject::SetBitmap(&morphBitmaps[morphStage / 3]);
    else
	ScreenObject::SetBitmap(&spinnerBitmaps[pos]);
}

void Spinner::tick()
{
    rotLeft(1);
    if (morphStage >= 0)
	morphStage--;

    // Just for fun... mwahahhahahaha
    if(isEvil && !ClassicMode) {
      float ax, ay;
      
      ax = PlayerShip.GetX() - GetX();  // Find the distance.  
      ay = PlayerShip.GetY() - GetY();
      
      ax = ax / 160;
      ay = ay / 100;

      if(ax < 0) ax -= 0.5f;
      if(ax > 0) ax += 0.5f;
      if(ay < 0) ay -= 0.5f;
      if(ay > 0) ay += 0.5f;
 
      velocity.SetXY(ax, ay);  
    } else {
      if(!(rand()%100)) {
	velocity.SetXY(velocity.GetX(), -velocity.GetY());
      }
    }
     

    wrapMoves = 1;
    SetBitmap();
    ScreenObject::tick();
}

void Spinner::draw()
{
  ScreenObject::draw();
}

void Spinner::hit()
{
  hitCount--;
  if(hitCount <= 0) isAlive = false;
}

//////////////////////////////////////////////////////////
// Explosion Class - for the nice big booms! =D

Explosion::Explosion()
{
    velocity.SetXY(0, 0);
    position.SetXY(0 * 2, 0 * 2);
    size = 3;
    accelleration.SetXY(0.0f, 0.0f);
    isAlive = true;	       
    objtype = 255;
    mysize = 10;
    timeLeft = 255;
    mysprite = &ShipThrusts[0];	// Assign a dumby bitmap.. makes life
    initExplosion();
}


Explosion::Explosion(float gx, float gy, float vx, float vy)
{
    velocity.SetXY(vx, vy);
    position.SetXY(DMULTCONST(gx), DMULTCONST(gy));
    size = 3;
    accelleration.SetXY(0.0f, 0.0f);
    isAlive = true;
    objtype = 255;
    mysize = 10;
    timeLeft = 255;
    mysprite = &ShipThrusts[0];	// Assign a dumby bitmap.
    initExplosion();
}


void Explosion::draw()
{
  int i, x, y;
  
  for (i = 0; i < numPTS; i++) {
    x = int (pts[i].GetX() + position.GetX());
    y = int (pts[i].GetY() + position.GetY());
       
    Ui::setpixel(x, y, 255, 255, status[i]);
#ifdef DOUBLE_SIZE
    Ui::setpixel(x + 1, y + 1, 255, 255, status[i]);
    Ui::setpixel(x, y + 1, 255, 255, 0);
#endif
  }
}


void Explosion::tick()
{
  int i;
  
  for (i = 0; i < numPTS; i++) {
    pts[i] += vel[i];
    if (vel[i].GetX() > 5.0f || vel[i].GetX() < -5.0f)
      status[i] -= 3;
    if (vel[i].GetY() > 5.0f || vel[i].GetY() < -5.0f)
      status[i] -= 3;
    status[i] -= 1;
    timeLeft--;
  }
  
  if(timeLeft < 0) 
    isAlive = false;
}


void Explosion::initExplosion()
{
  int i;
  
  for (i = 0; i < numPTS; i++) {
    pts[i].SetXY(float (rand() % DMULTCONST(5)), float (rand() % DMULTCONST(5)));
    vel[i].SetXY(DMULTCONST(5.0f) - float (rand() % DMULTCONST(10)),
		 DMULTCONST(5.0f) - float (rand() % DMULTCONST(10)));
    
    vel[i] += velocity;
    status[i] = 255;
  }
}


/////////////////////////////////////////////
// The Enemy Class. =D

Enemy::Enemy() 
{
  velocity.SetXY(1.0f, 0.25f);
  position.SetXY(4, 30);
  accelleration.SetXY(0.0f, 0.0f);
  isAlive = true;	       
  objtype = ENEMY;
  mysize = 10;
  SetBitmap(&Gbit[ENEMY]);
}


void Enemy::tick()
{
  Vector temp;
  int j;
  ScreenObject::tick(); 



  if(!(rand()%50)) {
    j = GetOpenObject();
    temp.SetXY( PlayerShip.GetX()-GetX()+PlayerShip.VelX(), 
		PlayerShip.GetY()-GetY()+PlayerShip.VelY());  
    temp /= temp.length();
    temp *= 2.5f;
    
    ObjectList[j] = new ScreenObject;
    ObjectList[j]->restore();
    ObjectList[j]->SetVel(temp.GetX(), temp.GetY());
    ObjectList[j]->SetXY(GetX(), GetY());
    ObjectList[j]->SetAcc(0.0f, 0.0f);
    ObjectList[j]->SetWrapMoves(false);
    ObjectList[j]->SetMaxSpeed(255.0f);
    ObjectList[j]->settype(BULLET2);
    ObjectList[j]->SetBitmap(&Gbit[BULLET2]);
    ObjectList[j]->setsize(5);
  }
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
   ShipThrusts[j].LoadImage(BINDIR "/graphics/eship1.bmp");
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


/////////////////////////////////////////////////////////////////////////////
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

    newobject->SetXY(x, y);
    newobject->SetAcc(0.0f, 0.0f);
    newobject->SetVel(xv, yv);
    newobject->SetBitmap(&Gbit[type]);
    newobject->settype(type);
    newobject->SetWrapMoves(true);
    newobject->restore();

    switch(type) {
    case BIGAST:
      newobject->SetSize(10, 10);
      newobject->setsize(30);
      break;
    case SMALLAST:
      newobject->SetSize(10, 10);
      newobject->setsize(10);
      break;
    case MEDAST:
      newobject->setsize(20);
      newobject->SetSize(10,10);
      break;
    }

    ObjectList[openObject] = newobject;

    return openObject;
}
