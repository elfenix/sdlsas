/*
 * Alien.hpp
 *
 *  Created on: Feb 25, 2012
 *      Author: andrew
 */

#ifndef ALIEN_HPP_
#define ALIEN_HPP_

#include "GameEntity.hpp"

class Alien: public GameEntity
{
public:
    Alien();
    virtual void tick();

    virtual bool check_bounce( const GameEntity& p_other ) const;
    virtual bool check_collide( const GameEntity& p_other ) const;
    virtual bool destructive_collision( const GameEntity& p_hitter );

private:
    int lastFire, cBitmap, vChange;
};

#endif /* ALIEN_HPP_ */
