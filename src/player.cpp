#include "sasteroids.h"

/////////////////////////////////////////////////////////////////////////
// Player Class....

Player::Player() 
{
  shieldMax = 200;
  shieldTimeLeft = 0;
  bounce = 0;
  shieldLives = 3;
  lives = 3;
  thrustcnt = 0;
  deadStick = 0;
  wrapMoves = 1;
  wPower = 0;
  objtype = PLAYERSHIP;

  size.SetXY(28,28);
  setsize(16);
  setmass(MassShip);
  SetBitmap();
}

void Player::SetBitmap()
{
  if (thrustcnt)
    ScreenObject::SetBitmap(&Gbit[BIT_PLAYERT]);
  else
    ScreenObject::SetBitmap(&Gbit[BIT_PLAYER]);
}

void Player::draw()
{
  if(!deadStick) {
    ScreenObject::draw();
    if(shielded())
      Gbit[SHIELD].put( int(position.GetX()), int(position.GetY()) );
  }
}


void Player::tick() 
{
    if (thrustcnt)
	thrustcnt--;
    if (!thrustcnt)
	accelleration.SetXY(0, 0);

    if (shieldStatus) {
      shieldTimeLeft--;
      if(shieldTimeLeft<=0) shieldStatus = 0;
    } 
    
    if(canRecharge) {
      if(shieldTimeLeft<shieldMax && rand()%3) shieldTimeLeft++;
    }

    if (wPower < maxPower)
	wPower+=rechargeRate;

    wrapMoves = 1;

    if (bounce > 0) {
	accelleration.SetXY(0,0);
	velocity.SetXY(1.0f - float(rand()%10)/5.0f,
		       1.0f - float(rand()%10)/5.0f);		
	bounce--;
    }
      
    SetBitmap();
    ScreenObject::tick();    
}


void Player::shieldOn()
{
    if (shieldTimeLeft > 0)
      shieldStatus = 1;
}

void Player::shieldOff()
{
  shieldStatus = 0;
}


int Player::shielded()
{
    return shieldStatus;
}

void Player::Hyper()
{
  if(!deadStick)
    SetXY((float) (rand() % Ui::WIDTH()), (float) (rand() % Ui::HEIGHT()));
}


void Player::Reset() 
{ 
  SetBitmap(); bounce = 0;
  objtype = PLAYERSHIP;
  maxPower = 220;
  wPower = maxPower + (2*Glevel);                // As level goes up, 
  rechargeRate = 2 + (int(0.5f*float(Glevel)));  // so does weapon. 
  shieldMax = 280;
  shieldTimeLeft = shieldMax;
  shieldStatus = 0;
  
  canShootThree = 0;
  canShootBack = 0;
  canFireShield = 0;
  canRecharge = 0;
  angle = 0.0f;
  
  restore();
  SetDeadStick(0);

  int width = Gbit[BIT_PLAYER].width();
  int height = Gbit[BIT_PLAYER].height();
  int x = int(ScreenLimits.GetX());
  int y = int(ScreenLimits.GetY());
  x = x / 2; y = y / 2;
  SetXY(x, y);
  SetVel(0.0f, 0.0f);
}



void Player::Brake()
{
  if(!deadStick) 
    SetVel(0.0f, 0.0f );
}
 
void Player::SetDeadStick(int dead)
{
  deadStick = dead;
}


void Player::Thrust(float rRate)
{ 
  if(!deadStick) {
    accelleration.SetXY(-FastMath::sin(getAngle()),FastMath::cos(getAngle())); 
    accelleration = accelleration * rRate;
    thrustcnt = 3;
    SetBitmap();
  }
}

void Player::addMaxPower(int a) 
{ 
  maxPower += a; 
}

void Player::addRegPower(int a) 
{ 
  rechargeRate += a; 
}

void Player::dischargeWeapon()
{
  wPower -= 25;
}

void Player::shieldAdd(int a)
{
  shieldTimeLeft += a;
  if(shieldTimeLeft > shieldMax) shieldTimeLeft = shieldMax;
}


////////////////////////////////////////////////////////////////////////////
// fire a bullet for player ship
void Player::Fire()
{
 int bullet;
  float fcos, fsin;
  float rx, ry;

  if(eeggL == 80 && eeggR == 160 && eeggU == 3 && eeggD == 12 ) {
    CleanUpStuff();
    eeggL = 0; eeggR = 0; eeggU = 0; eeggD = 0; eeggS = 1;
  }

  if (weaponPower() > 10) {
    PlaySound(SND_FIRE);

    rx = GetX();
    ry = GetY();
     
    fsin = -FastMath::cos(getAngle());
    fcos = FastMath::sin(getAngle());

    // Give it a bit of a kick back... ehehehheeheh
    if(!ClassicMode) {
      AddVel(fcos / 10.0f, fsin / 10.0f);
      dischargeWeapon();
    }

    bullet = GetOpenObject();
    ObjectList[bullet] = new ScreenObject;
    ObjectList[bullet]->restore();
    fsin = -FastMath::cos(getAngle());
    fcos = FastMath::sin(getAngle());
    ObjectList[bullet]->SetXY(rx + fcos, ry + fsin);
    ObjectList[bullet]->SetVel(-(VelX() + BUL_SPEED * fcos),
			       -(VelY() + BUL_SPEED * fsin));
    ObjectList[bullet]->SetAcc(0.0f, 0.0f);
    ObjectList[bullet]->SetBitmap(&Gbit[BULLET]);
    ObjectList[bullet]->SetWrapMoves(false);
    ObjectList[bullet]->SetMaxSpeed(255.0f);
    ObjectList[bullet]->settype(SHIP_BUL);     
    ObjectList[bullet]->setsize(5);
 
    if(canShootThree && weaponPower() > 20) {
      // Fire 2
      bullet = GetOpenObject();
      ObjectList[bullet] = new ScreenObject;
      ObjectList[bullet]->restore();
      fsin = -FastMath::cos(getAngle()+5);
      fcos = FastMath::sin(getAngle()+5);
      ObjectList[bullet]->SetXY(rx + Size() * fcos,
				ry + Size() * fsin);
      ObjectList[bullet]->SetVel(-(VelX() + BUL_SPEED * fcos),
				 -(VelY() + BUL_SPEED * fsin));
      ObjectList[bullet]->SetAcc(0.0f, 0.0f);
      ObjectList[bullet]->SetBitmap(&Gbit[BULLET]);
      ObjectList[bullet]->SetWrapMoves(false);
      ObjectList[bullet]->SetMaxSpeed(255.0f);
      ObjectList[bullet]->settype(SHIP_BUL);  
      ObjectList[bullet]->setsize(5);
      
      // Fire 3
      bullet = GetOpenObject();
      ObjectList[bullet] = new ScreenObject;
      ObjectList[bullet]->restore();
      fsin = -FastMath::cos(getAngle()-5);
      fcos = FastMath::sin(getAngle()-5);
      ObjectList[bullet]->SetXY(rx + Size() * fcos,
				ry + Size() * fsin);
      ObjectList[bullet]->SetVel(-(VelX() + BUL_SPEED * fcos),
				 -(VelY() + BUL_SPEED * fsin));
      ObjectList[bullet]->SetAcc(0.0f, 0.0f);
      ObjectList[bullet]->SetBitmap(&Gbit[BULLET]);
      ObjectList[bullet]->SetWrapMoves(false);
      ObjectList[bullet]->SetMaxSpeed(255.0f);
      ObjectList[bullet]->settype(SHIP_BUL); 
      ObjectList[bullet]->setsize(5);

      dischargeWeapon(); // Discharg 2x as fast when shooting 3
      AddVel(-fcos / 10.0f, -fsin / 10.0f);
    }

    if(canShootBack && weaponPower() > 20) {
      bullet = GetOpenObject();
      ObjectList[bullet] = new ScreenObject;
      ObjectList[bullet]->restore();
      fsin = -FastMath::cos(getAngle());
      fcos = FastMath::sin(getAngle());
      ObjectList[bullet]->SetXY(rx + Size() * fcos,
				ry + Size() * fsin);
      ObjectList[bullet]->SetVel((VelX() + BUL_SPEED * fcos),
				 (VelY() + BUL_SPEED * fsin));
      ObjectList[bullet]->SetAcc(0.0f, 0.0f);
      ObjectList[bullet]->SetBitmap(&Gbit[BULLET]);
      ObjectList[bullet]->SetWrapMoves(false);
      ObjectList[bullet]->SetMaxSpeed(255.0f);
      ObjectList[bullet]->settype(SHIP_BUL);     
      ObjectList[bullet]->setsize(5);
      AddVel(fcos / 10.0f, fsin / 10.0f);     
      dischargeWeapon();
      dischargeWeapon();
    }
  } else {
    // TODO: Do something interesting.
  }
}
