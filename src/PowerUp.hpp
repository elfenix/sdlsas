/*
 * PowerUp.hpp
 *
 *  Created on: Feb 25, 2012
 *      Author: andrew
 */

#ifndef POWERUP_HPP_
#define POWERUP_HPP_

#include "GameEntity.hpp"

// Anonymous declarations for things we'd power up
class Player;

class PowerUp : public GameEntity
{
public:
	typedef enum
	{
		WEAPON_MAX,
		WEAPON_ENERGY,
		WEAPON_THREE_FIRE,
		WEAPON_BACK_FIRE,

		SHIELD_BOOST,
		SHIELD_RECHARGE
	} ActionType;

public:
	PowerUp();

	virtual bool check_bounce( const GameEntity& p_other ) const;
	virtual bool check_collide( const GameEntity& p_other ) const;

	virtual void do_powerup( Player& p_player ) const;

	virtual void tick();

private:
	ActionType m_action;
	int timeLeft;
};


#endif /* POWERUP_HPP_ */
