/*
 * Bullet.hpp
 *
 *  Created on: Feb 25, 2012
 *      Author: andrew
 */

#ifndef BULLET_HPP_
#define BULLET_HPP_

#include "GameEntity.hpp"
#include "vector.h"

class Bullet: public GameEntity
{
public:
    static bool object_is_bullet( const GameEntity& p_obj )
    {
        return dynamic_cast< const Bullet* >( &p_obj ) != 0;
    }

    Bullet( const Vector& p_position, bool p_evil, int p_angle,
            int p_parent_type );
    Bullet( const Vector& p_position, const Vector& p_velocity, bool p_evil,
            int p_parent_type );
    Bullet( GameEntity* p_parent, int p_angle, bool p_evil );
    virtual ~Bullet()
    {
    }

    virtual void notify_destroyed( const GameEntity& p_other )
    {
        if( m_parent )
            m_parent->notify_destroyed( p_other );
    }
    virtual bool check_bounce( const GameEntity& p_other ) const;
    virtual bool check_collide( const GameEntity& p_other ) const;

protected:
    void base_init( const Vector& p_pos, const Vector& p_velocity,
            bool p_evil );

private:
    GameEntity* m_parent;
    int m_parent_type;
};

#endif /* BULLET_HPP_ */
