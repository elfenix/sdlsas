#ifndef GAME_ENTITY_H
#define GAME_ENTITY_H

#include "vector.h"
#include "ScreenBitmap.hpp"
#include <cassert>

// Anonymous class for circular dependency
class PlayingField;


// Basic ScreenObject class, could probably be used on its own for most things
class GameEntity
{
public:
	typedef enum
	{
		ASTEROID_BIG,
		ASTEROID_MEDIUM,
		ASTEROID_SMALL,

		SHIP_ALIEN,
		SHIP_PLAYER,
		SHIP_SPINNER,

		BULLET,
		EVIL_BULLET,

		POWERUP
	} ObjectType;

	static bool object_is_asteroid( const GameEntity& p_object )
	{
		return p_object.type() == ASTEROID_BIG ||
			   p_object.type() == ASTEROID_MEDIUM ||
			   p_object.type() == ASTEROID_SMALL;
	}


public:
	GameEntity();
	GameEntity(int type);
	virtual ~GameEntity();
	void initialize(int type);

	static void set_limits(float x, float y);

	bool alive();
	void die();
	void restore();

	virtual void draw();	// Draw the object
	virtual void tick();	// Move the object for one time tick

	// Called when this object's bounding box overlaps with another
	virtual void collision( GameEntity& p_other )
	{
		if( alive() && p_other.alive() )
		{
			if( check_bounce( p_other ) && p_other.check_bounce( *this ) )
			{
				// We can bounce! No need to destroy anything
				calculate_bounce( *this, p_other );
			} else if( check_collide( p_other ) && p_other.check_collide( *this ) ) {
				// No bouncing.... bad day for us
				bool destroyed_self = destructive_collision( p_other );
				bool destroyed_other = p_other.destructive_collision( *this );

				if( destroyed_self )
					p_other.notify_destroyed( *this );
				if( destroyed_other )
					notify_destroyed( p_other );
			}
		}
	}










	// Called when both objects agree there is a collision with no bounce.
	virtual bool destructive_collision( const GameEntity& p_hitter )
	{
		die();
		return true;
	}

	virtual bool check_bounce( const GameEntity& p_other ) const { return false; }

	virtual bool check_collide( const GameEntity& p_other ) const { return false; }

	virtual void notify_bounce( const GameEntity& p_other ) const {}

	virtual void notify_destroyed( const GameEntity& p_other ) {}

	void trim();
	void randomDir(float magn);
	void randomSpeed(float start, float stop);

	void setbounce();
	void setBounceStat(int);
	int bounceStat() const;

	void set_bitmap(ScreenBitmap* sprite);
	void set_wrap_moves(bool a);
	void SetSize(int x, int y);
	void SetVel(float x, float y);
	void set_acceleration(float x, float y);
	void add_velocity(float x, float y);
	void SetXY(float x, float y);
	void SetMaxSpeed(float a);
	void settype(int a);
	void set_size(int i);
	void set_mass(float a);
	void set_velocity(const Vector& nVelocity);
	void set_pos(const Vector& nXY);

	void rotateLeft();
	void rotateRight();
	void addAngle(float a);
	void set_angular_velocity(float a);


	float GetX() const;
	float GetY() const;
	virtual float GetWidth() const;
	virtual float GetHeight() const;
	float getAnglef() const;
	int getAngle() const;

	ObjectType type() const;
	int Size() const;
	float VelX() const;
	float VelY() const;
	float getMass() const;

	const Vector& get_pos() const;
	const Vector& get_velocity() const;

	static void calculate_bounce( GameEntity& a, GameEntity& b )
	{
		float ti, tj;
		Vector vi, vj;
		Vector t1, t2;

		Vector ij = b.get_pos() - a.get_pos();
		ij /= ( ij.length() + 0.01 );

		if (isnan(ij.length()))
		{
			return;
		}

		vi = a.get_velocity();
		vj = b.get_velocity();

		ti = vi * ij;
		tj = vj * ij;

		float t3 = (2.0f * (ti - tj)) / (a.getMass() + b.getMass());

		if (t3 < 0)
			t3 = -t3;

		t1 = vi;
		t1 -= (t3 * a.getMass()) * ij;

		t2 = vj;
		t2 += (t3 * b.getMass()) * ij;

		// Make sure we put in only valid numbers.
		if (isnan(t1.length()) || isnan(t2.length()))
			return;

		a.set_velocity(t1);
		a.trim();
		a.setbounce();
		b.set_velocity(t2);
		b.trim();
		b.setbounce();
	}

	PlayingField* get_field() { assert( m_field != 0 ); return m_field; }
	const PlayingField* get_field() const { assert( m_field != 0 ); return m_field; }

private:
	void set_field( PlayingField* p_field ) { m_field = p_field; }

protected:
	bool wrapMoves;
	bool isAlive;
	float maxspeed, mass;
	float myCenXRef, myCenYRef;
	Vector position;
	Vector velocity;
	Vector accelleration;
	Vector size;
	ScreenBitmap* 	m_sprite;
	PlayingField*	m_field;
	float			m_angular_velocity;
	int objtype, mysize, bounce;
	float angle;

private:
	friend class PlayingField;
};


#endif
