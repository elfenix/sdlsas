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



// ///////////////////////////////////////////////////////////////////////
// Ship Class....
Ship::Ship() : ScreenObject()
{
  shieldMax = 100;
  shieldTimeLeft = 0;
  bounce = 0;
  shieldLives = 3;
  lives = 3;
  thrustcnt = 0;
  pos = 0;
  deadStick = 0;
  wrapMoves = 1;
  wPower = 0;
  angle = 0;

  setsize(8);
  SetBitmap();
}

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
    ScreenObject::draw();
    if(shielded())
      Gbit[SHIELD].put( int(position.GetX()), int(position.GetY()) );
  }
}


void Ship::tick()
{
    wrapMoves = 1;

    if (thrustcnt)
	thrustcnt--;
    if (!thrustcnt)
	accelleration.SetXY(0, 0);
    if (shieldStatus) {
      shieldTimeLeft--;
      if(shieldTimeLeft<=0) shieldStatus = 0;
    } 

    if (bounce > 0) {
	accelleration.SetXY(0,0);
	velocity.SetXY(1.0f - float(rand()%10)/5.0f,
		       1.0f - float(rand()%10)/5.0f);		
	bounce--;
    }
      
    SetBitmap();
    ScreenObject::tick();

    if (wPower < maxPower)
	wPower+=rechargeRate;
}

void Ship::shieldOn()
{
    if (shieldTimeLeft > 0)
      shieldStatus = 1;
}

void Ship::shieldOff()
{
  shieldStatus = 0;
}


int Ship::shielded()
{
    return shieldStatus;
}

void Ship::Hyper()
{
  if(!deadStick)
    SetXY((float) (rand() % Ui::WIDTH()), (float) (rand() % Ui::HEIGHT()));
}


void Ship::Reset() 
{ 
  pos = 0; SetBitmap(); angle = 0; bounce = 0;
  maxPower = 220;
  wPower = maxPower;
  rechargeRate = 2;
  shieldMax = 100;
  shieldTimeLeft = shieldMax;
  shieldStatus = 0;
  
  
  restore();
  SetDeadStick(0);

  int width = ShipBitmaps[0].width();
  int height = ShipBitmaps[0].height();
  int x = int(ScreenLimits.GetX()) - width;
  int y = int(ScreenLimits.GetY()) - height;
  x = x / 2; y = y / 2;
  SetXY(x, y);
  SetVel(0.0f, 0.0f);
}



void Ship::Brake()
{
  if(!deadStick) 
    SetVel(0.0f, 0.0f );
}
 
void Ship::SetDeadStick(int dead)
{
  deadStick = dead;
}


void Ship::Thrust(float rRate)
{ 
  if(!deadStick) {
    accelleration.SetXY(FastMath::sin(angle),-FastMath::cos(angle)); 
    accelleration = accelleration * rRate;
    thrustcnt = 3;
    SetBitmap();
  }
}

void Ship::addMaxPower(int a) 
{ 
  maxPower += a; 
}

void Ship::addRegPower(int a) 
{ 
  rechargeRate += a; 
}

void Ship::dischargeWeapon()
{
  wPower -= 25;
}

void Ship::shieldAdd(int a)
{
  shieldTimeLeft += a;
  if(shieldTimeLeft > shieldMax) shieldTimeLeft = shieldMax;
}

void Ship::SetBounce()
{
  bounce = 15;
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


///////////////////////////////////////////////////////
// Powerup Screen Object
PowerUp::PowerUp()
{
  const int MaxSlots = 7;
  int SlotMachine[MaxSlots] = 
    { 
      P_WMAX, P_WMAX, P_WENG, P_WENG, P_WENG, P_WTHR, P_SHLD 
    };

  ptype = SlotMachine[rand()%MaxSlots];
  
  SetVel(0.0f, 0.0f);
  SetAcc(0.0f, 0.0f);
  SetXY(float(DDIVCONST(rand()%Ui::HEIGHT())), 
	float(DDIVCONST(rand()%Ui::WIDTH())));
  setsize(5);
  settype(P_TYPE);
  SetBitmap(&Gbit[ptype]);
  restore();
  
  timeLeft = 275;
}

void PowerUp::tick()
{
  timeLeft--;
  if(timeLeft == 0) die();
  ScreenObject::tick();
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

    if(type == SMALLAST && !(rand()%LevelOdds(30,3)&&!ClassicMode))
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
