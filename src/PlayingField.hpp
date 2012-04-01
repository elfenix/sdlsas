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
//extern GameEntity* ObjectList[MAX_OBJECTS];

int CreateAsteroid(float x, float y, float xv, float yv, int type);

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

	const Vector& get_field_size() const { return m_field_size; }
	void set_field_size( const Vector& p_field_size ) { m_field_size = p_field_size; }

	static object_id get_open_object();
	object_id register_object( GameEntity* screen_object );
	int move_game_time( int p_time_ms );

	static bool check_current_collision( const GameEntity& p_1, const GameEntity& p_2 );

private:
	Vector m_field_size;
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
