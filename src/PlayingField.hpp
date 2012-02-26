/*
 * ObjectManager.h
 *
 *  Created on: Feb 12, 2012
 *      Author: andrew
 */

#ifndef OBJECTMANAGER_H_
#define OBJECTMANAGER_H_

#include "GameEntity.hpp"

const int MAX_OBJECTS = 256;
extern GameEntity* ObjectList[MAX_OBJECTS];

int CreateAsteroid(float x, float y, float xv, float yv, int type);

// Simple collision function
// TODO: make more accurate
inline bool collide(GameEntity* b1, GameEntity* b2)
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
inline void rcollide(GameEntity *i, GameEntity *j)
{
  Vector vecIM, vecJM, fPi(i->get_pos()), fPj(j->get_pos());
  float desiredDistance = (float)i->Size() + (float)j->Size();
  float dx = float(i->GetX() - j->GetX());
  float dy = float(i->GetY() - j->GetY());
  float d = sqrt(dx*dx + dy*dy);
  float iM, jM, viM, vjM;

  if(isnan(desiredDistance) || isnan(d)) return;
  if(d > desiredDistance) return;

  d = desiredDistance - d;    // Amount we want to move them away
  d -= 0.3f;                  // Want some overlap though.

  vecIM = i->get_velocity();
  vecJM = j->get_velocity();
  
  viM = vecIM.length(); 
  vjM = vecJM.length();

  iM = (d-viM) / (vjM+viM);   // Calculate distance to move i and
  jM = (d-vjM) / (vjM+viM);   // j.

  if(isnan(iM) || isnan(jM)) return;
  
  vecIM *= -1 * iM;
  vecJM *= -1 * jM;
  fPi += vecIM;
  fPj += vecJM;

  i->set_pos(fPi);
  j->set_pos(fPj);
}


class PlayingField
{
public:
	typedef int object_id;

public:
	PlayingField();
	virtual ~PlayingField();

	void initialize_list();
	void free_objects();
	void draw_objects();

	static object_id get_open_object();
	static object_id register_object( GameEntity* screen_object );
	int move_game_time( int p_time_ms );
};

#include "Alien.hpp"
#include "Asteroid.hpp"
#include "Bullet.hpp"
#include "Explosion.hpp"
#include "Player.hpp"
#include "Spinner.hpp"
#include "PowerUp.hpp"


extern Player PlayerShip;
extern int ClassicMode;


#endif /* OBJECTMANAGER_H_ */
