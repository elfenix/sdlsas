// Copyright 1994 Brad Pitzel
// Modifications Copyright 2002 Andrew M.
//
// Feel free to use/distribute/modify as long as credit/copyrights for myself 
// are included.
//
// Notes:
// * maxspeed is really the maximum speed SQUARED of the object in the class
// * The setmaxspeed function automatically takes the sqrt or the parameter 

#ifndef __SDLSAS_OBJ__
#define __SDLSAS_OBJ__

#include "sasteroids.h"

#include "screenobject.h"
#include "player.h"

void SetupObjArray();
void FreeObjArray();
int GetOpenObject();
void DrawObjects();
int CreateAsteroid(float x, float y, float xv, float yv, int type);








class Spinner : public ScreenObject
{
 public:  
  Spinner() 
    { 
      isAlive = true; morphStage = 27; 
      mysize = 20; isEvil = !(rand() % 5); 
      hitCount = 3; objtype = SPINNER;
      angle =0.0f;
      SetSize(28,28);
      
    }


  virtual void SetBitmap();
  virtual void tick();
  virtual void draw();
  virtual void hit();

  inline int morph() {
    return morphStage;
  }


 
 private:
  int morphStage, isEvil, hitCount;		 
};




class Explosion : public ScreenObject {
 public:
  Explosion();
  Explosion(float gx, float gy, float vx, float vy);
  void initExplosion();
  
  virtual void draw();
  virtual void tick(); 
   
 private:
  int timeLeft, size;
  static const int numPTS = 15;
  
  Vector pts[numPTS];
  Vector vel[numPTS];
  int status[numPTS];
};




class Enemy : public ScreenObject
{
 public:
  Enemy();
  virtual void tick();

  private:
  int lastFire, cBitmap, vChange;
};


class PowerUp : public ScreenObject
{
 public:
  PowerUp();
  virtual void tick();

  inline int effect()
    {
      return ptype;
    }
 private:
  int ptype, timeLeft;
};


// Simple collision function
// TODO: make more accurate
inline bool collide(ScreenObject* b1, ScreenObject* b2)
{
  float d  = (float)b1->Size() + (float)b2->Size();
  float dx = float(b1->GetX() - (b2->GetX()));
  float dy = float(b1->GetY() - (b2->GetY()));
  
  d *= d;
  dx *= dx;
  dy *= dy;
  
  if ( (dx+dy) < d) {	
    return true;
  }
  
  return false;
}


// Reverse a collision(avoid objects moving inside
// each other(not the best way of doing this, bu
// should work.... This also keeps the bounce
// code from going crazy. 
inline void rcollide(ScreenObject *i, ScreenObject *j)
{
  Vector vecIM, vecJM, fPi(i->GetXYVec()), fPj(j->GetXYVec());
  float desiredDistance = (float)i->Size() + (float)j->Size();
  float dx = float(i->GetX() - j->GetX());
  float dy = float(i->GetY() - j->GetY());
  float d = sqrt(dx*dx + dy*dy);
  float iM, jM, viM, vjM;

  if(isnan(desiredDistance) || isnan(d)) return;
  if(d > desiredDistance) return;

  d = desiredDistance - d;    // Amount we want to move them away
  d -= 0.3f;                  // Want some overlap though.

  vecIM = i->GetVel();
  vecJM = j->GetVel();
  
  viM = vecIM.length(); 
  vjM = vecJM.length();

  iM = (d-viM) / (vjM+viM);   // Calculate distance to move i and
  jM = (d-vjM) / (vjM+viM);   // j.

  if(isnan(iM) || isnan(jM)) return;
  
  vecIM *= -1 * iM;
  vecJM *= -1 * jM;
  fPi += vecIM;
  fPj += vecJM;

  i->SetXY(fPi);
  j->SetXY(fPj);
}


extern ScreenObject* ObjectList[MAX_OBJECTS];
extern Player PlayerShip;
extern int ClassicMode;

#endif
