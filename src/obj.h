//
// Feel free to use/distribute/modify as long as credit/copyrights for myself 
// are included.
//
// Notes:
// * maxspeed is really the maximum speed SQUARED of the object in the class
// * The setmaxspeed function automatically takes the sqrt or the parameter 

#ifndef __BPObj__
#define __BPObj__

#include <stdlib.h>
#include "sbitmap.h"
#include "fastmath.h"
#include "sasteroids.h"
#include "vector.h"

const int SHIPTURNS = 32;
const int TURNANGLE = 256/SHIPTURNS;

const int EXPLODE_TYPE = 255;

extern SBitmap ShipBitmaps[32];
extern SBitmap ShipThrusts[32];
extern SBitmap morphBitmaps[10];
extern SBitmap spinnerBitmaps[32];

void InitShips();

const int MAX_OBJECTS = 128;


class ScreenObject;
class Ship;

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
  
  void die()  
    { isAlive = false; }
  
  void restore() 
    { isAlive = true; }
  
  void SetBitmap(SBitmap* sprite) 
    { 
      mysprite = sprite;
      myCenXRef = float(sprite->width())/2.0f; 
      myCenYRef = float(sprite->height())/2.0f;
    }	
  
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
  
  int type() 
    { return objtype; }
  
  void settype(int a) 
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
  
  inline void randomDir(float magn)
    {
      int angle = rand() % 255;
      velocity.SetXY(FastMath::sin(angle)*magn, 
		     FastMath::cos(angle)*magn);
    }
  
  inline void randomSpeed(float start, float stop)
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
  static Vector ScreenLimits;
};




class Ship : public ScreenObject 
{
 public:
  
  Ship() : ScreenObject(), shieldMax(30), shieldTimeLeft(0), 
    Vbounce(0), shieldLives(3), lives(3), thrustcnt(0), pos(0) 
    { deadStick = 0; SetBitmap(); wrapMoves = 1; wPower = 0; }
  
  inline void SetDeadStick(int dead) { deadStick = dead; }
  inline int isDeadStick() { return deadStick; }
  
  inline void Brake() { if(!deadStick) SetVel(0.0f, 0.0f ); }
  
  inline int shieldPercent() const 
    { return (100*(int)shieldTimeLeft)/(int)shieldMax; }
  
  inline void bounce(int time) 
    { Vbounce=time; }
  
  virtual void draw();
  virtual void tick();
  virtual void SetBitmap();
  
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
    { return lives; }
  
  inline void setships(int sships)
    { lives = sships; }
  
  inline void addship()	
    { lives++; }
  
  inline void death() 
    { lives--; }
  
  inline void Thrust(float rRate)
    { 
      if(!deadStick) {
	accelleration.SetXY(FastMath::sin(angle),-FastMath::cos(angle)); 
	accelleration = accelleration * rRate;
	thrustcnt = 3;
	SetBitmap();
      }
    }
  
  
  void shieldOn();
  int shielded();
  void hyper();
  
  void SetupDrawing() {
    for(int i = 0; i < SHIPTURNS; i++) {
      ShipBitmaps[i].setupsurface();
      ShipThrusts[i].setupsurface();
      spinnerBitmaps[i].setupsurface(); // Works for the time being.
    }
  }
  
  inline int weaponPower() { if(deadStick) return 0; return wPower; }
  inline void dischargeWeapon() { wPower += 15; }
  
  inline int& getAngle() 
    { return angle; }
  
 protected:
  int angle, shieldMax, shieldTimeLeft, Vbounce, shieldLives;
  int lives, thrustcnt, wPower, pos, deadStick;
  
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
  
  //  bool realAlive() 
  //{ if(timeLeft > 0) return true; else return false; }
  
  
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






// Simple collision function
// TODO: make more accurate
inline bool collide(ScreenObject b1, ScreenObject b2)
{
  float d = (float)b1.Size() + (float)b2.Size();
  float dx= float(fabs(b1.GetCenX() - (b2.GetCenX())));
  float dy= float(fabs(b1.GetCenY() - (b2.GetCenY())));
  
  if ((dx <= d) && (dy <= d)) {
    if ( (dx*dx+dy*dy) < d*d) {	
      return true;
    }
  }
  
  return false;
}


extern Ship PlayerShip;
extern int ClassicMode;

#endif
