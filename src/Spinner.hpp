/*
 * Spinner.hpp
 *
 *  Created on: Feb 25, 2012
 *      Author: andrew
 */

#ifndef SPINNER_HPP_
#define SPINNER_HPP_

#include "sasteroids.h"
#include "GameEntity.hpp"

class Spinner: public GameEntity
{
public:
    Spinner();

    virtual bool check_bounce( const GameEntity& p_other ) const;
    virtual bool check_collide( const GameEntity& p_other ) const;
    virtual bool destructive_collision( const GameEntity& p_hitter );

    virtual void SetBitmap();
    virtual void tick();
    virtual void draw();

    inline int morph()
    {
        return morphStage;
    }

private:
    int morphStage, isEvil, hitCount;
};

#endif /* SPINNER_HPP_ */
