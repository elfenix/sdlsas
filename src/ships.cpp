// Portions of original code (C)opyright by Brad Pitzel
// New code, by Andrew M.
// 
// Feel free to use/distribute/modify as long as acknowledgement to
// sasteroids author(s) is made.

#include "sasteroids.h"



/////////////////////////////////////////////
// Spinner Class (works a lot like ship class )

void Spinner::SetBitmap()
{
  //    if (morphStage >= 0)
  //	ScreenObject::SetBitmap(&morphBitmaps[morphStage / 3]);
  //    else
  ScreenObject::SetBitmap(&Gbit[BIT_SPINNER]);
}

void Spinner::tick()
{
    rotateLeft();

    if (morphStage >= 0)
	morphStage--;

    // Just for fun... mwahahhahahaha
    
    float ax, ay;

    if(!bounce) {    
      if(!(rand()%20)) {
	ax = PlayerShip.GetX() - GetX();  // Find the distance.  
	ay = PlayerShip.GetY() - GetY();
	
	ax = ax / 200;
	ay = ay / 200;

	if(PlayerShip.isDeadStick() || PlayerShip.shielded())
	  { ax = -ax; ay = -ay; }

	if(ax < 0) ax -= 0.4f;
	if(ax > 0) ax += 0.4f;
	if(ay < 0) ay -= 0.4f;
	if(ay > 0) ay += 0.4f;
    
	if(!(rand()%20)) { ax = 0.8f; }
	if(!(rand()%21)) { ax = -0.8f; }
	if(!(rand()%30)) { ay = 0.8f; }
	if(!(rand()%31)) { ay = -0.8f; }
      
	velocity.SetXY(ax, ay);    
      }
    } else {
      ax = (float(rand()%100)-50.0f)/50.0f;
      ay = (float(rand()%100)-50.0f)/50.0f;
      velocity.SetXY(ax, ay);
    }

    if(bounce) {
      bounce -= 2;
      if(bounce < 0) bounce = 0;
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
    mysprite = &Gbit[0];	// Assign a dumby bitmap.. makes life
    initExplosion();
}


Explosion::Explosion(float gx, float gy, float vx, float vy)
{
    velocity.SetXY(vx, vy);
    position.SetXY((gx), (gy));
    size = 3;
    accelleration.SetXY(0.0f, 0.0f);
    isAlive = true;
    objtype = 255;
    mysize = 10;
    timeLeft = 255;
    mysprite = &Gbit[0];	// Assign a dumby bitmap.
    initExplosion();
}


void Explosion::draw()
{
  int i;
  float x, y, distance;

  for (i = 0; i < numPTS; i++) {
    x = (pts[i].GetX() + position.GetX());
    y = (pts[i].GetY() + position.GetY());

    distance = pts[i].GetX() * pts[i].GetX();
    distance += pts[i].GetY() * pts[i].GetY();
    
    if(distance < 0.1f) distance += 0.1f;
    if(distance > 64.0f) distance = 64.0f;

    distance = distance / 64.0f;
    
    Ui::setpixel(x, y, 255, 255, (char) distance);
    Ui::setpixel(x + 1, y + 1, 255, 255, (char) distance);
    Ui::setpixel(x, y + 1, 255, 255, 0);
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
    pts[i].SetXY(float (rand() % (10)), float (rand() % (10)));
    vel[i].SetXY((10.0f) - float (rand() % (20)),
		 (10.0f) - float (rand() % (20)));
    
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
  mysize = 20;
  SetBitmap(&Gbit[ENEMY]);
  setmass(5.0f/4.0f);
}


void Enemy::tick()
{
  Vector temp;
  int j;

  SetMaxSpeed(2.5f); 
  if(!(rand()%20)) {
    if(vChange) vChange = 0;
    else vChange = 1;
  }

  if(GetY() < 2) vChange = 0;
  if(GetY() > 60) vChange = 1;
  if(!bounce) {
    if(vChange) {
      velocity.SetXY(0.6f, -0.8f);
    } else {
      velocity.SetXY(0.6f, 0.8f);
    }
  } else bounce -= 2;

  ScreenObject::tick(); 


  if(!(rand()%50) && !PlayerShip.isDeadStick()) {
    j = GetOpenObject();
    temp.SetXY( PlayerShip.GetX()-GetX()+PlayerShip.VelX(), 
		PlayerShip.GetY()-GetY()+PlayerShip.VelY());  
    temp /= temp.length();
    temp *= 5.0f;
    
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

  if(Glevel > 5 && !(rand()%7)) {
    ptype = P_SRCG;
  } else {
    ptype = SlotMachine[rand()%MaxSlots];
  }

  SetVel(0.0f, 0.0f);
  SetAcc(0.0f, 0.0f);
  SetXY(float((rand()%Ui::HEIGHT())), 
	float((rand()%Ui::WIDTH())));
  setsize(10);
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
    PlaySound(SND_BOOM_C);
  } else {
    PlaySound(SND_BOOM_A);
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
  PlaySound(SND_BOOM_C);
}

