#ifndef __SHIP_H__
#define __SHIP_H__

#include "PlayingField.hpp"
#include "PowerUp.hpp"
#include "GameEntity.hpp"

void CleanUpStuff();


class Player: public GameEntity
{
public:
	Player();

	virtual void draw();
	virtual void tick();
	virtual void SetBitmap();

	void shieldOn();
	void shieldOff();
	void shieldAdd(int a);
	int shielded();

	bool get_disabled() const
	{
		return deadStick;
	}

	virtual void destroy()
	{
		PlaySound(SND_BOOM_C);
		death();
		SetDeadStick( 1 );

		if( get_field() )
		{
			get_field()->register_object( new Explosion( GetX(), GetY(), VelX(), VelY() ) );
		}

		Reset();
		SetDeadStick( 1 );
	}

	virtual bool destructive_collision(const GameEntity& p_other)
	{
		const PowerUp* powerup = 0;
		bool died = false;

		if( (p_other.type() == GameEntity::BULLET || p_other.type() == GameEntity::EVIL_BULLET) &&
			shielded() )
		{
			// do nothing,
		}
		else if( ( powerup = dynamic_cast<const PowerUp*>( &p_other ) ) )
		{
			powerup->do_powerup( *this );
		} else {
			destroy();
			died = true;
		}

		return died;
	}

	virtual bool check_bounce(const GameEntity& p_other) const
	{
		return !isDeadStick() && PlayerShip.shielded();
	}


	virtual bool check_collide(const GameEntity& p_other) const
	{
		return !isDeadStick();
	}

	virtual void notify_bounce(const GameEntity& p_other) const
	{
	}

	virtual void notify_destroyed(const GameEntity& p_other)
	{
		switch( p_other.type() )
		{
		case ASTEROID_BIG:
			score += 500;
			break;

		case ASTEROID_MEDIUM:
			score += 250;
			break;

		case ASTEROID_SMALL:
			score += 125;
			break;

		case SHIP_ALIEN:
			score += 300;
			break;

		case SHIP_SPINNER:
			score += 300;
			break;
		}
	}

	void reset_score()
	{
		score = 0;
	}

	int get_score()
	{
		return score;
	}



	inline void death()
	{
		lives--;
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

	inline int isDeadStick() const
	{
		return deadStick;
	}

	inline int shieldPercent() const
	{
		return (100 * (int) (shieldTimeLeft)) / (int) (shieldMax);
	}

	inline int weaponPercent() const
	{
		return (100 * wPower) / (maxPower);
	}

	inline int weaponPower()
	{
		if (deadStick)
			return 0;

		return wPower;
	}
	void Reset();
	void SetDeadStick(int dead);
	void addMaxPower(int a);
	void addRegPower(int a);
	void Thrust(float rRate);
	void Hyper();
	void dischargeWeapon();
	void Brake();
	void Fire();
	// Game play variables
	int canShootThree;
	int canShootBack;
	int canFireShield;
	int canRecharge;
protected:
	int shieldMax, shieldTimeLeft, shieldLives;
	int lives, thrustcnt, wPower, deadStick;
	int maxPower, rechargeRate, shieldStatus;
	int score;
private:
	void CreateBullet(int discharge_count, int angle, bool apply_thrust,
			double scale_thrust = 0.1);
};

#endif
