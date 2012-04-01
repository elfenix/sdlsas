/*
 * Bullet.cpp
 *
 *  Created on: Feb 25, 2012
 *      Author: andrew
 */

#include "Bullet.hpp"
#include "fastmath.h"
#include "sasteroids.h"

const float BUL_SPEED = 15.0f;
const float MassBullet = 1.0f / 8.0f;

Bullet::Bullet( const Vector& p_position, bool p_evil, int p_angle,
        int p_parent_type ) :
        m_parent( 0 ),
                m_parent_type( p_parent_type )
{
    Vector unit_position( f_math::sin( p_angle ), -f_math::cos( p_angle ) );
    Vector velocity = (unit_position * BUL_SPEED);
    velocity *= -1.0;

    base_init( p_position, velocity, p_evil );
}

Bullet::Bullet( const Vector& p_position, const Vector& p_velocity, bool p_type,
        int p_parent_type ) :
        m_parent( 0 ),
                m_parent_type( p_parent_type )
{
    base_init( p_position, p_velocity, p_type );
}

Bullet::Bullet( GameEntity* p_parent, int p_angle, bool p_type ) :
        m_parent( p_parent ),
                m_parent_type( -1 )
{
    double fsin = -f_math::cos( p_angle );
    double fcos = f_math::sin( p_angle );
    Vector unit_position( fcos, fsin );

    Vector position = p_parent->get_pos() + unit_position;

    Vector velocity( p_parent->get_velocity() );
    velocity = (unit_position * BUL_SPEED);
    velocity *= -1.0;

    base_init( position, velocity, p_type );
}

void Bullet::base_init( const Vector& p_pos, const Vector& p_velocity,
        bool p_evil )
{
    set_pos( p_pos );
    set_acceleration( 0.0, 0.0 );
    set_angular_velocity( 1.0 );
    restore();
    set_velocity( p_velocity );
    set_acceleration( 0.0f, 0.0f );
    set_wrap_moves( false );
    SetMaxSpeed( 255.0f );
    set_size( 5 );

    if( p_evil )
    {
        settype( EVIL_BULLET );
        set_bitmap( &bmp_evil_bullet );
    }
    else
    {
        settype( BULLET );
        set_bitmap( &bmp_bullet );
    }
}

bool Bullet::check_bounce( const GameEntity& p_other ) const
        {
    return false;
}

bool Bullet::check_collide( const GameEntity& p_other ) const
        {
    bool can_collide = false;
    if( m_parent )
        can_collide = can_collide || p_other.type() != m_parent->type();
    if( m_parent_type != -1 )
        can_collide = can_collide || p_other.type() != m_parent_type;
    if( !m_parent && m_parent_type == -1 )
        can_collide = true;
    return can_collide;
}
