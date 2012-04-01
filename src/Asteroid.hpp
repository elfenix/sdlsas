#ifndef ASTEROID_H
#define ASTEROID_H

#include "sasteroids.h"
#include "GameEntity.hpp"

void GenerateAsteroids( PlayingField& p_field );


class Asteroid : public GameEntity
{
public:
	Asteroid();
	virtual ~Asteroid();

	static Asteroid* create_asteroid( PlayingField& p_field, float x, float y, float xv, float yv, ObjectType type);


	virtual bool destructive_collision( const GameEntity& p_hitter );
	virtual void create_children() = 0;
	virtual void create_explosion();

	virtual bool check_bounce( const GameEntity& p_other ) const
	{
	  return !object_is_asteroid( p_other );
	}

	virtual bool check_collide( const GameEntity& p_other ) const
	{
	  return !object_is_asteroid( p_other );
	}
};



class SmallAsteroid : public Asteroid
{
 public:
  void initialize(float x, float y, float xv, float yv);

  SmallAsteroid();
  SmallAsteroid(float x, float y, float xv, float yv);

  virtual void create_children();

 private:
  bool m_evil;
};


class MediumAsteroid : public Asteroid
{
 public:
  void initialize(float x, float y, float xv, float yv);

  MediumAsteroid();
  MediumAsteroid(float x, float y, float xv, float yv);

  virtual void create_children();
};


class LargeAsteroid : public Asteroid
{
 public:
  void initialize(float x, float y, float xv, float yv);

  LargeAsteroid();
  LargeAsteroid(float x, float y, float xv, float yv);

  virtual void create_children();
};





#endif
