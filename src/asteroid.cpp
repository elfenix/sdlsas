/*************************************************************/
/* Original Files Copyright 1994, Brad Pitzel                */
/* Modifications Copyright 2003, Andrew McCall               */
/*************************************************************/
/* Feel free to use/distribute/modify as long as credit      */
/* copyrights for myself and contributors are included.      */
/*************************************************************/

#include "sasteroids.h"
#include "asteroid.h"


/********************************************/
/* Utility Functions */
/********************************************/

bool Asteroid::validpoints(int killedBy) {
  if(killedBy != 0 && ObjectList[killedBy]) 
    if(ObjectList[killedBy]->type() == SHIP_BUL) return true;
  return false;
}


void Asteroid::create_explosion() 
{
  ScreenObject* explode;
  int j;
  
  j = GetOpenObject();
  if(j == -1) return;

  explode = new Explosion(GetX(), GetY(), VelX(), VelY());
  ObjectList[j] = explode;
}


int Asteroid::CreateAsteroid(float x, float y, float xv, float yv, int type)
{
    ScreenObject *newobject;
    int openObject;

    openObject = GetOpenObject();
    if (openObject == -1)
      return -1;
      
    switch(type) 
      {
      case BIGAST:
	newobject = new LargeAsteroid(x, y, xv, yv);
	break;
      case SMALLAST:
	newobject = new SmallAsteroid(x, y, xv, yv);
	break;
      case MEDAST:
	newobject = new MediumAsteroid(x, y, xv, yv);
	break;
      }
    
    if(newobject) numasts++;
    else return -1;

    ObjectList[openObject] = newobject;
    return openObject;
}


// Kill an asteroid 
void Asteroid::KillAsteroid(int number, int killedBy, bool killChildren)
{
  Asteroid* ast;
  
  if(ObjectList[number]->alive()) {
    ast = (Asteroid*) ObjectList[number];
    ast->Explode(killedBy);
    
    ObjectList[number]->die();    
    numasts -= 1;
  }
}




/********************************************/
/* Small Asteroid */
/********************************************/

void SmallAsteroid::initialize(float x, float y, float xv, float yv) 
{
  if(!ClassicMode && !(rand()%LevelOdds(50,14))) {
    objtype = ESMAST;
  } else {
    objtype = SMALLAST;
  }
  
  restore();
  SetXY(x, y);
  SetAcc(0.0f, 0.0f);
  SetVel(xv, yv);
  SetBitmap(&Gbit[objtype]);
  SetWrapMoves(true);
  SetSize(20, 20);
  setsize(14);
  setmass(MassSmallAst);
  SetMaxSpeed(SmallAstMax); 
}


SmallAsteroid::SmallAsteroid()
{
  initialize(0, 0, 1.0f, 1.0f);
}


SmallAsteroid::SmallAsteroid(float x, float y, float xv, float yv)
{
  initialize(x, y, xv, yv);
}

void SmallAsteroid::Explode(int killedBy) 
{
  unsigned char angle; 

  create_explosion();
  if(validpoints(killedBy))
    upscore(100);
  
  if(objtype == ESMAST) {
    for(angle = 0; angle < 220; angle += 30) {
      float fsin = FastMath::sin(angle) * 9.0f;
      float fcos = FastMath::cos(angle) * 9.0f;
      int j = GetOpenObject();
      if(j == -1) return;
      ObjectList[j] = new ScreenObject;
      ObjectList[j]->restore();
      ObjectList[j]->SetVel(fsin, fcos);
      ObjectList[j]->SetXY(GetX(), GetY());
      ObjectList[j]->SetAcc(0.0f, 0.0f);
      ObjectList[j]->SetWrapMoves(false);
      ObjectList[j]->SetMaxSpeed(255.0f);
      ObjectList[j]->settype(BULLET2);
      ObjectList[j]->SetBitmap(&Gbit[BULLET2]);
      ObjectList[j]->setsize(5);
    } 
  }
  PlaySound(SND_BOOM_B);
  
}


/********************************************/
/* Medium Asteroid */
/********************************************/

void MediumAsteroid::initialize(float x, float y, float xv, float yv) 
{
  settype(MEDAST);
  restore();
  SetXY(x, y);
  SetAcc(0.0f, 0.0f);
  SetVel(xv, yv);
  SetBitmap(&Gbit[MEDAST]);
  SetWrapMoves(true);
  SetSize(40, 40);
  setsize(20);
  setmass(MassMedAst);
  SetMaxSpeed(MedAstMax); 
}


MediumAsteroid::MediumAsteroid()
{
  initialize(0, 0, 1.0f, 1.0f);
}


MediumAsteroid::MediumAsteroid(float x, float y, float xv, float yv)
{
  initialize(x, y, xv, yv);
}

void MediumAsteroid::Explode(int killedBy) 
{   
  char rA1, rA2;
  float px, py, vx, vy;

  if(validpoints(killedBy)) {
    if(numasts == 1) upscore(500*Glevel);
    else upscore(50);
  }
  create_explosion();
  PlaySound(SND_BOOM_B);

  rA1 = rand() % 255; 
  rA2 = int(rA1 + 128) % 255;
  
  px = GetX(); py = GetY();
  vx = VelX(); vy = VelY();
  
  CreateAsteroid( px, py, 
		  FastMath::sin(rA1) * 2.0f + vx, 
		  FastMath::cos(rA1) * 2.0f + vy, SMALLAST );     
  
  CreateAsteroid( px, py,
		  FastMath::sin(rA2) * 2.0f + vx,
		  FastMath::cos(rA2) * 2.0f + vy, SMALLAST );

  do {
    rA2 = rand() % 255;
  } while( abs(rA2 - rA1) < 10 && abs(rA2 - (rA1+124)) < 10);      
  
  rA1 = int(rA2 + 128) % 255;
  CreateAsteroid( px, py,
		  FastMath::sin(rA1) * 2.0f + vx,
		  FastMath::cos(rA1) * 2.0f + vy, SMALLAST );

  CreateAsteroid( px, py, 
		  FastMath::sin(rA2) * 2.0f + vx,
		  FastMath::cos(rA2) * 2.0f + vy, SMALLAST );
  
}


/********************************************/
/* Large Asteroid */
/********************************************/

void LargeAsteroid::initialize(float x, float y, float xv, float yv) 
{
  settype(BIGAST);
  restore();
  SetXY(x, y);
  SetAcc(0.0f, 0.0f);
  SetVel(xv, yv);
  SetBitmap(&Gbit[BIGAST]);
  SetWrapMoves(true);
  SetSize(80, 80);
  setsize(40);
  setmass(MassBigAst);
  SetMaxSpeed(BigAstMax); 
}


LargeAsteroid::LargeAsteroid()
{
  initialize(0, 0, 1.0f, 1.0f);
}


LargeAsteroid::LargeAsteroid(float x, float y, float xv, float yv)
{
  initialize(x, y, xv, yv);
}

void LargeAsteroid::Explode(int killedby) 
{
  char rA1, rA2;
  float px, py, vx, vy;
  
  
  if(validpoints(killedby)) {
    if(numasts == 1) upscore(500*Glevel);
    else upscore(25);
  }
  create_explosion();
  PlaySound(SND_BOOM_B);
  
  rA1 = rand() % 255; 
  rA2 = int(rA1 + 128) % 255;
  
  px = GetX(); py = GetY();
  vx = VelX(); vy = VelY();
  
  CreateAsteroid( px, py, 
		  FastMath::sin(rA1) * 2.0f + vx, 
		  FastMath::cos(rA1) * 2.0f + vy, MEDAST );     
  
  CreateAsteroid( px, py,
		  FastMath::sin(rA2) * 2.0f + vx,
		  FastMath::cos(rA2) * 2.0f + vy, MEDAST );

  do {
    rA2 = rand() % 255;
  } while( abs(rA2 - rA1) < 10 && abs(rA2 - (rA1+124)) < 10);      
  
  rA1 = int(rA2 + 128) % 255;
  CreateAsteroid( px, py,
		  FastMath::sin(rA1) * 2.0f + vx,
		  FastMath::cos(rA1) * 2.0f + vy, MEDAST );
  
  //  if(!(rand()%LevelOdds(16,7))) { 
    int j = GetOpenObject();
    ObjectList[j] = new Spinner;
    ObjectList[j]->SetXY(px, py);
    ObjectList[j]->SetVel(FastMath::sin(rA2)*2.0f + vx, 
			  FastMath::cos(rA2)*2.0f + vy);
			  /*			  } else { 
    CreateAsteroid( px, py, 
		    FastMath::sin(rA2) * 2.0f + vx,
		    FastMath::cos(rA2) * 2.0f + vy, MEDAST );
		     }    */
}
