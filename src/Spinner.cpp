/*
 * Spinner.cpp
 *
 *  Created on: Feb 25, 2012
 *      Author: andrew
 */

#include "Spinner.hpp"
#include "Asteroid.hpp"
#include "PlayingField.hpp"

const float MassSpinner   = 2.0f;


Spinner::Spinner()
{
	isAlive = true;
	morphStage = 27;
	mysize = 20;
	isEvil = !(rand() % 5);
	hitCount = 3;
	settype( SHIP_SPINNER );
	angle = 0.0f;
	SetSize(28, 28);
	set_angular_velocity( 5.0 );
}


bool Spinner::check_bounce( const GameEntity& p_other ) const
{
	if( morphStage > 0 )
		return false;
	return true;
}


bool Spinner::check_collide( const GameEntity& p_other ) const
{
	if( Asteroid::object_is_asteroid( p_other ) )
		return false;
	return true;
}


bool Spinner::destructive_collision( const GameEntity& p_hitter )
{
	bool died = false;

	if( p_hitter.type() == BULLET || p_hitter.type() == EVIL_BULLET )
	{
		hitCount--;
	}
	else
	{
		hitCount = -1;
	}

	if( hitCount <= 0 )
	{
		PlayingField::register_object(
				new Explosion(GetX(), GetY(), get_velocity().GetX(),
						get_velocity().GetY()));
		PlaySound(SND_BOOM_B);
		die();
		died = true;
	} else {
		PlaySound( SND_BOOM_A );
	}

	return died;
}

void Spinner::SetBitmap()
{
	GameEntity::set_bitmap(&bmp_spinner);
}


void Spinner::tick()
{
	if (morphStage >= 0)
		morphStage--;

	// Just for fun... mwahahhahahaha

	float ax, ay;

	if (!bounce)
	{
		if (!(rand() % 5))
		{
			ax = PlayerShip.GetX() - GetX(); // Find the distance.
			ay = (PlayerShip.GetY() - GetY());

			ax = ax / 200;
			ay = ay / 200;

			if (PlayerShip.isDeadStick() || PlayerShip.shielded())
			{
				ax = -ax;
				ay = -ay;
			}

			if (ax < 0)
				ax -= 0.4f;
			if (ax > 0)
				ax += 0.4f;
			if (ay < 0)
				ay -= 0.4f;
			if (ay > 0)
				ay += 0.4f;

			if (!(rand() % 20))
			{
				ax = 0.8f;
			}
			if (!(rand() % 21))
			{
				ax = -0.8f;
			}
			if (!(rand() % 30))
			{
				ay = 0.8f;
			}
			if (!(rand() % 31))
			{
				ay = -0.8f;
			}

			velocity.SetXY(ax, ay);
		}
	}
	else
	{
		ax = (float(rand() % 100) - 50.0f) / 50.0f;
		ay = (float(rand() % 100) - 50.0f) / 50.0f;
		velocity.SetXY(ax, ay);

		bounce -= 2;
		if (bounce < 0)
			bounce = 0;
	}

	wrapMoves = 1;
	SetBitmap();
	GameEntity::tick();
}


void Spinner::draw()
{
	GameEntity::draw();
}






