/*
 * PowerUp.cpp
 *
 *  Created on: Feb 25, 2012
 *      Author: andrew
 */

#include "PowerUp.hpp"
#include "sasteroids.h"
#include "Player.hpp"

PowerUp::PowerUp()
{
    int MaxSlots;

    ActionType SlotMachine[] =
                {
                        WEAPON_MAX, WEAPON_MAX, WEAPON_ENERGY,
                        WEAPON_ENERGY,
                        SHIELD_BOOST, // lvl 1-3  (d5)			d5
                        WEAPON_THREE_FIRE, WEAPON_MAX, WEAPON_ENERGY,
                        SHIELD_BOOST, // lvl 3-6  (d5 + 4)		d9
                        WEAPON_THREE_FIRE, WEAPON_THREE_FIRE, // lvl 6-9  (d5 + 4 + 2)	d11
                        SHIELD_RECHARGE // lvl 9+   (d5 + 4 + 2 + 1)d12
            };

    if( Glevel < 3 )
        MaxSlots = 5;
    else if( Glevel < 6 )
        MaxSlots = 9;
    else if( Glevel < 11 )
        MaxSlots = 11;
    else
        MaxSlots = 12;

    m_action = SlotMachine[rand() % MaxSlots];

    SetVel( 0.0f, 0.0f );
    set_acceleration( 0.0f, 0.0f );
    SetXY( float( (rand() % UserInterfaceManager::HEIGHT()) ),
            float( (rand() % UserInterfaceManager::WIDTH()) ) );
    set_size( 10 );
    settype( POWERUP );
    restore();
    extern ScreenBitmap bmp_powerup_wmax;
    extern ScreenBitmap bmp_powerup_weng;
    extern ScreenBitmap bmp_powerup_wthree;
    extern ScreenBitmap bmp_powerup_wshield;
    extern ScreenBitmap bmp_powerup_wshield_charge;
    switch( m_action )
    {
    case WEAPON_ENERGY:
        timeLeft = 275;
        set_bitmap( &bmp_powerup_weng );
        break;

    case SHIELD_BOOST:
        timeLeft = 200;
        set_bitmap( &bmp_powerup_wshield );
        break;

    case WEAPON_THREE_FIRE:
        timeLeft = 180;
        set_bitmap( &bmp_powerup_wthree );
        break;

    case SHIELD_RECHARGE:
        timeLeft = 100;
        set_bitmap( &bmp_powerup_wshield_charge );
        break;

    default:
        case WEAPON_MAX:
        timeLeft = 275;
        set_bitmap( &bmp_powerup_wmax );
        break;
    }

    timeLeft = 275;
}

bool PowerUp::check_bounce( const GameEntity& p_other ) const
        {
    return false;
}

bool PowerUp::check_collide( const GameEntity& p_other ) const
        {
    const Player* player = dynamic_cast< const Player* >( &p_other );
    if( player )
        return true;
    return false;
}

void PowerUp::do_powerup( Player& p_player ) const
        {
    PlaySound( SND_POWERUP );

    switch( m_action )
    {
    case WEAPON_ENERGY:
        p_player.addRegPower( 2 );
        break;

    case SHIELD_BOOST:
        p_player.shieldAdd( 50 );
        break;

    case WEAPON_THREE_FIRE:
        p_player.set_can_shoot_three(true);
        break;

    case SHIELD_RECHARGE:
        p_player.set_can_shield_recharge(true);
        break;

    default:
        case WEAPON_MAX:
        p_player.addMaxPower( 20 );
        break;
    }
}

void PowerUp::tick()
{
    timeLeft--;
    if( timeLeft == 0 )
        die();
    GameEntity::tick();
}
