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

const int MAX_OBJECTS = 256;
const int SHIPTURNS = 32;
const int TURNANGLE = 256/SHIPTURNS;
const int EXPLODE_TYPE = 255;

class SBitmap;
class ScreenObject;
class Ship;


extern SBitmap ShipBitmaps[32];
extern SBitmap ShipThrusts[32];
extern SBitmap morphBitmaps[10];
extern SBitmap spinnerBitmaps[32];
extern Vector ScreenLimits;

void InitShips();

extern ScreenObject* ObjectList[MAX_OBJECTS];
extern Ship PlayerShip;

void SetupObjArray();
void FreeObjArray();
int GetOpenObject();
void DrawObjects();
void AllObjectsTick();
int CreateAsteroid(float x, float y, float xv, float yv, int type);


// Basic ScreenObject class, could probably be used on its own for most things
class ScreenObject {
 public:
  ScreenObject();
  ScreenObject(int type);
  virtual ~ScreenObject();
  
  inline bool alive()
    { return isAlive; }
  
  void die(); 
  void restore();
  void SetBitmap(SBitmap* sprite);
  void randomDir(float magn);
  void randomSpeed(float start, float stop);  

  virtual void draw();	// Draw the object
  virtual void tick();	// Move the object for one time tick

  inline void SetWrapMoves(bool a) 
    { wrapMoves = a; }
  
  inline static void SetLimits(float x, float y) 
    { ScreenLimits.SetXY(x,y); }
  
  inline void SetSize(int x, int y)
    { size.SetXY((float)x,(float)y); }
  
  inline void SetVel(float x, float y) 
    { velocity.SetXY(x,y); }
  
  inline void SetAcc(float x, float y)
    { accelleration.SetXY(x,y); }
  
  inline void SetXY(float x, float y)
    { position.SetXY(x,y); }
  
  inline float GetX() const
    { return position.GetX(); }
  
  inline float GetY() const
    { return position.GetY(); }
  
  inline float GetCenX() const
    { return myCenXRef + position.GetX(); }
  
  inline float GetCenY() const 
    { return myCenYRef + position.GetY(); }
  
  inline float GetWidth() const
    { return (float)mysprite->width(); }
  
  inline float GetHeight() const
    { return (float)mysprite->height(); }
  
  inline const Vector& GetXYVec() const
    { return position; }

  inline const Vector& GetVel() const
    { return velocity; }
  
  inline void SetMaxSpeed(float a) 
    { maxspeed = a*a; }
  
  inline int type() 
    { return objtype; }
  
  inline void settype(int a) 
    { objtype = a; }
  
  inline void setsize(int i) 
    { mysize = i; }
  
  inline int Size() 
    { return mysize; }
  
  inline float VelX() 
    { return velocity.GetX(); }
  
  inline float VelY() 
    { return velocity.GetY(); }
  
  inline void AddVel(float x, float y)
    { velocity.SetXY(VelX()+x,VelY()+y); }
  
 protected:
  bool wrapMoves;
  bool isAlive;
  float maxspeed;
  float myCenXRef, myCenYRef;
  Vector position;
  Vector velocity;
  Vector accelleration;
  Vector size;
  SBitmap* mysprite;
  int objtype, mysize;
};




class Ship : public ScreenObject 
{
 public:
  
  Ship();

  virtual void draw();
  virtual void tick();
  virtual void SetBitmap();

  void Reset();
  void SetDeadStick(int dead);
  void addMaxPower(int a);
  void addRegPower(int a);

  void Thrust(float rRate);  
  void Hyper();
  void dischargeWeapon();
  void Brake();
  void SetBounce();

  void shieldOn();
  void shieldOff();
  void shieldAdd(int a);
  int shielded();

  inline int weaponPower() 
    { 
      if(deadStick) return 0; return wPower; 
    }

  inline int& getAngle() 
    { 
      return angle; 
    }
  
  inline int isDeadStick() 
    { 
      return deadStick; 
    }
   
  inline int shieldPercent() const 
    {
      return (100*(int)shieldTimeLeft)/(int)shieldMax; 
    }

  inline int weaponPercent() const
    {
      return (100*wPower)/(maxPower);
    }
 
  inline void rotLeft( int t )
    {
      if(!deadStick) {
	angle -= TURNANGLE*t;
	if(angle > 256) angle -= 256;
	if(angle < 0) angle += 256;
	pos -= t;
	if(pos < 0) pos += SHIPTURNS;
	pos %= SHIPTURNS;
	SetBitmap(); 
      }
    }
  
  inline void rotRight( int t)
    { 
      if(!deadStick) {
	angle += TURNANGLE*t;
	if(angle > 256) angle -=256;
	if(angle < 0) angle += 256; 
	pos += t;
	pos %= SHIPTURNS;
	SetBitmap(); 
      }
    }
  
  inline int ships() 
    { 
      return lives; 
    }
  
  inline void setships(int sships)
    { 
      lives = sships; 
    }
  
  inline void addship()	
    { 
      lives++; 
    }
  
  inline void death() 
    { 
      lives--; 
    }
  
 protected:
  int angle, shieldMax, shieldTimeLeft, bounce, shieldLives;
  int lives, thrustcnt, wPower, pos, deadStick;
  int maxPower, rechargeRate, shieldStatus;
};




class Spinner : public Ship 
{
 public:  
  Spinner() 
    { 
      isAlive = true; morphStage = 27; 
      mysize = 10; isEvil = !(rand() % 5); 
      hitCount = 3; objtype = SPINNER;
    }

  virtual void SetBitmap();
  virtual void tick();
  virtual void draw();
  virtual void hit();

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
  int lastFire, cBitmap;
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
inline bool collide(ScreenObject b1, ScreenObject b2)
{
  float d = (float)b1.Size() + (float)b2.Size();
  float dx= float(b1.GetCenX() - (b2.GetCenX()));
  float dy= float(b1.GetCenY() - (b2.GetCenY()));
  
  d *= d;
  dx *= dx;
  dy *= dy;

  if ( (dx+dy) < d) {	
      return true;
    }
  
  return false;
}


extern Ship PlayerShip;
extern int ClassicMode;

#endif
