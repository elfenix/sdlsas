#ifndef __SCREEN_OBJECT_H_
#define __SCREEN_OBJECT_H_

#include "vector.h"

// Basic ScreenObject class, could probably be used on its own for most things
class ScreenObject {
 public:

  ScreenObject();
  ScreenObject(int type);
  virtual ~ScreenObject();
  void initialize(int type);

  static void SetLimits(float x, float y);

  bool alive();
  void die(); 
  void restore();

  virtual void draw();	// Draw the object
  virtual void tick();	// Move the object for one time tick
  void trim();
  void randomDir(float magn);
  void randomSpeed(float start, float stop);  

  void setbounce();
  void setBounceStat(int);
  int bounceStat() const;
  
  void SetBitmap(SBitmap* sprite);
  void SetWrapMoves(bool a); 
  void SetSize(int x, int y);
  void SetVel(float x, float y);
  void SetAcc(float x, float y);
  void AddVel(float x, float y);
  void SetXY(float x, float y);
  void SetMaxSpeed(float a);
  void settype(int a) ;
  void setsize(int i) ;
  void setmass(float a);
  void SetVel(const Vector& nVelocity);
  void SetXY(const Vector& nXY);

  void rotateLeft();
  void rotateRight();
  void addAngle(float a);
  void setRotVel(float a);


  float GetX() const;
  float GetY() const;
  float GetWidth() const;
  float GetHeight() const;
  float getAnglef() const;
  int getAngle() const;

  int type() const;
  int Size() const;
  float VelX() const;
  float VelY() const;
  float getMass() const;

  const Vector& GetXYVec() const;
  const Vector& GetVel() const;


 protected:
  bool wrapMoves;
  bool isAlive;
  float maxspeed, mass;
  float myCenXRef, myCenYRef;
  Vector position;
  Vector velocity;
  Vector accelleration;
  Vector size;
  SBitmap* mysprite;
  float rotVel;
  int objtype, mysize, bounce;
  float angle;
};


#endif
