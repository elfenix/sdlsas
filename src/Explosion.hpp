/*
 * Explosion.hpp
 *
 *  Created on: Feb 25, 2012
 *      Author: andrew
 */

#ifndef EXPLOSION_HPP_
#define EXPLOSION_HPP_

#include "GameEntity.hpp"
#include "vector.h"

class Explosion:
    public GameEntity
{
public:
    Explosion();
    Explosion( float gx, float gy, float vx, float vy );
    void initExplosion();

    virtual float GetWidth() const
    {
        return 0.0;
    }
    virtual float GetHeight() const
    {
        return 0.0;
    }

    virtual void draw();
    virtual void tick();

private:
    int timeLeft, size;
    static const int numPTS = 15;

    Vector pts[numPTS];
    Vector vel[numPTS];
    int status[numPTS];
};

#endif /* EXPLOSION_HPP_ */
