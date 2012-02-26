#ifndef __ASTEROID_H__
#define __ASTEROID_H__

#include "sasteroids.h"

class Asteroid : public ScreenObject
{
 public:
  static int CreateAsteroid(float x, float y, float xv, float yv, int type);
  static void KillAsteroid(int number, int killedBy, bool killChildren = false);

  virtual void Explode(int) = 0;
  bool validpoints(int killedBy);
  void create_explosion();
};



class SmallAsteroid : public Asteroid
{
 public:
  void initialize(float x, float y, float xv, float yv);

  SmallAsteroid();
  SmallAsteroid(float x, float y, float xv, float yv);

  virtual void Explode(int);

};


class MediumAsteroid : public Asteroid
{
 public:
  void initialize(float x, float y, float xv, float yv);

  MediumAsteroid();
  MediumAsteroid(float x, float y, float xv, float yv);

  virtual void Explode(int);
};


class LargeAsteroid : public Asteroid
{
 public:
  void initialize(float x, float y, float xv, float yv);

  LargeAsteroid();
  LargeAsteroid(float x, float y, float xv, float yv);

  virtual void Explode(int);
};





#endif
