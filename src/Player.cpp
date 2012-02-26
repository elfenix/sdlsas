#include "sasteroids.h"
#include "PlayingField.hpp"
#include "Player.hpp"

const float MassShip      = 5.0f / 4.0f;

/////////////////////////////////////////////////////////////////////////
// Player Class....

Player::Player()
{
	shieldMax = 200;
	shieldTimeLeft = 0;
	bounce = 0;
	shieldLives = 3;
	lives = 3;
	thrustcnt = 0;
	deadStick = 0;
	wrapMoves = 1;
	wPower = 0;
	objtype = SHIP_PLAYER;

	size.SetXY(28, 28);
	set_size(16);
	//set_mass(MassShip);
	set_mass(MassShip / 8.0);
	SetBitmap();
}

void Player::SetBitmap()
{
	if (thrustcnt)
		GameEntity::set_bitmap(&bmp_player_thrust);
	else
		GameEntity::set_bitmap(&bmp_player);
}

void Player::draw()
{
	if (!deadStick)
	{
		GameEntity::draw();
		if (shielded())
			bmp_shield.draw(int(position.GetX()), int(position.GetY()));
	}
}

void Player::tick()
{
	if (thrustcnt)
		thrustcnt--;
	if (!thrustcnt)
		accelleration.SetXY(0, 0);

	if (shieldStatus)
	{
		shieldTimeLeft--;
		if (shieldTimeLeft <= 0)
			shieldStatus = 0;
	}

	if (canRecharge)
	{
		if (shieldTimeLeft < shieldMax && rand() % 3)
			shieldTimeLeft++;
	}

	if (wPower < maxPower)
		wPower += rechargeRate;

	wrapMoves = 1;

	if (bounce > 0)
	{
		accelleration.SetXY(0, 0);
		velocity.SetXY(1.0f - float(rand() % 10) / 5.0f,
				1.0f - float(rand() % 10) / 5.0f);
		bounce--;
	}

	SetBitmap();
	GameEntity::tick();
}

void Player::shieldOn()
{
	if (shieldTimeLeft > 0)
		shieldStatus = 1;
}

void Player::shieldOff()
{
	shieldStatus = 0;
}

int Player::shielded()
{
	return shieldStatus;
}

void Player::Hyper()
{
	if (!deadStick)
		SetXY((float) (rand() % Ui::WIDTH()), (float) (rand() % Ui::HEIGHT()));
}

void Player::Reset()
{
	SetBitmap();
	bounce = 0;
	objtype = SHIP_PLAYER;
	maxPower = 220;
	wPower = maxPower + (2 * Glevel); // As level goes up,
	rechargeRate = 2 + (int(0.5f * float(Glevel))); // so does weapon.
	shieldMax = 280;
	shieldTimeLeft = shieldMax;
	shieldStatus = 0;

	canShootThree = 0;
	canShootBack = 0;
	canFireShield = 0;
	canRecharge = 0;
	angle = 0.0f;

	restore();
	SetDeadStick(0);

	int x = int(ScreenLimits.GetX());
	int y = int(ScreenLimits.GetY());
	x = x / 2;
	y = y / 2;
	SetXY(x, y);
	SetVel(0.0f, 0.0f);
}

void Player::Brake()
{
	if (!deadStick)
		SetVel(0.0f, 0.0f);
}

void Player::SetDeadStick(int dead)
{
	deadStick = dead;
}

void Player::Thrust(float rRate)
{
	if (!deadStick)
	{
		accelleration.SetXY(-f_math::sin(getAngle()), f_math::cos(getAngle()));
		accelleration = accelleration * rRate;
		accelleration *= 3.80;
		thrustcnt = 3;
		SetBitmap();
	}
}

void Player::addMaxPower(int a)
{
	maxPower += a;
}

void Player::addRegPower(int a)
{
	rechargeRate += a;
}

void Player::dischargeWeapon()
{
	wPower -= 25;
}

void Player::shieldAdd(int a)
{
	shieldTimeLeft += a;
	if (shieldTimeLeft > shieldMax)
		shieldTimeLeft = shieldMax;
}


void Player::CreateBullet( int p_discharge_count, int angle, bool p_apply_thrust, double p_scale_thrust )
{
    double fsin = -f_math::cos( angle );
    double fcos = f_math::sin( angle );

    PlayingField::register_object( new Bullet( this, angle, false ) );

    if(!ClassicMode)
    {
		// Give it a bit of a kick back... ehehehheeheh
    	add_velocity( float( fcos * p_scale_thrust), float( fsin * p_scale_thrust ) );

    	// lower weapon power
    	for( int i = 0; i < p_discharge_count; i++ )
    		dischargeWeapon();
    }
}


////////////////////////////////////////////////////////////////////////////
// fire a bullet for player ship
void Player::Fire()
{
	int bullet;
	float fcos, fsin;
	float rx, ry;

	if (weaponPower() > 10)
	{
		PlaySound(SND_FIRE);

		CreateBullet(1, getAngle(), true);

		if ( canShootThree && weaponPower() > 20 )
		{
			// Fire 2
			CreateBullet(1, getAngle() + 5, true);

			// Fire 3
			CreateBullet(0, getAngle() - 5, true);
		}

		if ( canShootBack && weaponPower() > 20 )
		{
			CreateBullet(2, getAngle() + 128, true, 0.175);
		}
	}
}
