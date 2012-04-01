/*
 * Alien.cpp
 *
 *  Created on: Feb 25, 2012
 *      Author: andrew
 */

#include "Alien.hpp"
#include "Bullet.hpp"
#include "Asteroid.hpp"
#include "sasteroids.h"
#include "PlayingField.hpp"

const float MassEnemy = 1.0f;
Alien::Alien()
{
    velocity.SetXY( 2.0f, -0.25f );
    position.SetXY( 20.0f, 390.0f );
    accelleration.SetXY( 0.0f, 0.0f );
    isAlive = true;
    objtype = SHIP_ALIEN;
    mysize = 20;
    set_bitmap( &bmp_enemy );
    set_mass( 5.0f / 4.0f );
}

bool Alien::check_bounce( const GameEntity& p_other ) const
{
    return true;
}

bool Alien::check_collide( const GameEntity& p_other ) const
{
    if( Asteroid::object_is_asteroid( p_other ) )
        return false;
    return true;
}

bool Alien::destructive_collision( const GameEntity& p_hitter )
{
    if( get_field() )
    {
        get_field()->register_object(
                new Explosion( GetX(), GetY(), get_velocity().GetX(),
                        get_velocity().GetY() ) );
    }
    PlaySound( SND_BOOM_C );
    die();
    return true;
}

void Alien::tick()
{
    Vector temp;
    int j;

    SetMaxSpeed( 2.5f );
    if( !(rand() % 20) )
    {
        if( vChange )
            vChange = 0;
        else
            vChange = 1;
    }

    if( GetY() > 395.0f )
        vChange = 0;
    if( GetY() < 340.0f )
        vChange = 1;
    if( !bounce )
    {
        if( vChange )
        {
            velocity.SetXY( 0.6f, 0.8f );
        }
        else
        {
            velocity.SetXY( 0.6f, -0.8f );
        }
    }
    else
    {
        bounce -= 2;
        if( bounce < 0 )
            bounce = 0;
    }

    GameEntity::tick();

    if( !(rand() % 50) && get_field() )
    {
        temp.SetXY( PlayerShip.GetX() - GetX() + PlayerShip.VelX(),
                PlayerShip.GetY() - GetY() + PlayerShip.VelY() );
        temp /= temp.length();
        temp *= 5.0f;

        GameEntity* bullet = new Bullet( get_pos(), temp, true, type() );
        get_field()->register_object( bullet );
    }
}

