// Portions of original code (C)opyright by Brad Pitzel
// New code, by Andrew M.
// 
// Feel free to use/distribute/modify as long as acknowledgement to
// sasteroids author(s) is made.

#include "sasteroids.h"

/////////////////////////////////////////////////////////////////////////
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


// Hit ** Functions

////////////////////////////////////////////////////////
// Hit a Spinner (called when Spinner is shot)
void HitSpinner(int number, int killedBy)
{
  Spinner* spinner;

  spinner = (Spinner*) ObjectList[number];
  spinner->hit();

  if(!spinner->alive()) {
    Explosion* explode;
    int j;
    explode = new Explosion(spinner->GetX(),
			    spinner->GetY(),
			    spinner->VelX(),
			    spinner->VelY());
    j = GetOpenObject();
    ObjectList[j] = explode;
  }
}


//////////////////////////////////////////////////////
// Hit the stupid enemy person.
void HitEnemy(int number, int killedBy)
{
  int j;
  Enemy* e;

  e = (Enemy*) ObjectList[number];
  ObjectList[number]->die();
  if(killedBy == 254) upscore(1000);
  
  j = GetOpenObject();
  ObjectList[j] = new Explosion(e->GetX(),
				e->GetY(),
				e->VelX(),
				e->VelY());
}

