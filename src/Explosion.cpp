/*
 * Explosion.cpp
 *
 *  Created on: Feb 25, 2012
 *      Author: andrew
 */

#include "Explosion.hpp"
#include <GL/gl.h>
#include <PlayingField.hpp>
#include "sasteroids.h"

//////////////////////////////////////////////////////////
// Explosion Class - for the nice big booms! =D

Explosion::Explosion()
{
    velocity.SetXY(0, 0);
    position.SetXY(0 * 2, 0 * 2);
    size = 3;
    accelleration.SetXY(0.0f, 0.0f);
    isAlive = true;
    objtype = 255;
    mysize = 10;
    timeLeft = 255;
    initExplosion();
}


Explosion::Explosion(float gx, float gy, float vx, float vy)
{
    velocity.SetXY(vx, vy);
    position.SetXY((gx), (gy));
    size = 3;
    accelleration.SetXY(0.0f, 0.0f);
    isAlive = true;
    objtype = 255;
    mysize = 10;
    timeLeft = 255;
    initExplosion();
}


void Explosion::draw()
{
	int i;

	for (i = 0; i < numPTS; i++)
	{
		unsigned char angle =
			  static_cast<unsigned char>( i + int( double( timeLeft ) / 255.0 ) % 255 );

		Vector draw_pt( position );
		draw_pt += pts[i];

		float scale = std::min( pts[i].lengthSqrd(), 8192.0f );
		scale /= 8192.0f;
		scale = 1 - scale;

		float x = draw_pt.GetX();
		float y = draw_pt.GetY();

		glBindTexture( GL_TEXTURE_2D, 0 );
		glColor3f( 1.0, 1.0, scale );

		scale *= 5.0;
		glBegin( GL_TRIANGLES );
			glColor4f( 1.0, 0.0, 0.0, 1.0 );
			glVertex2f( GLfloat( x ), GLfloat( y ) );

			glColor4f( 1.0, 1.0, 0.0, 1.0 );
			glVertex2f( GLfloat( x + scale * f_math::sin( angle ) ), GLfloat( y + scale * f_math::cos( angle ) ) );

			glColor4f( 1.0, 1.0, 1.0, 0.25 );
			glVertex2f( GLfloat( x + scale * f_math::sin( angle + 27 ) ), GLfloat( y + scale * f_math::cos( angle + 27 ) ) );
		glEnd();
	}
}


void Explosion::tick()
{
  int i;

  for (i = 0; i < numPTS; i++) {
    pts[i] += vel[i];
    if (vel[i].GetX() > 5.0f || vel[i].GetX() < -5.0f)
      status[i] -= 3;
    if (vel[i].GetY() > 5.0f || vel[i].GetY() < -5.0f)
      status[i] -= 3;
    status[i] -= 1;
    timeLeft--;
  }

  if(timeLeft < 0)
    isAlive = false;
}


void Explosion::initExplosion()
{
  int i;

  for (i = 0; i < numPTS; i++) {
    pts[i].SetXY(float (rand() % (10)), float (rand() % (10)));
    vel[i].SetXY((10.0f) - float (rand() % (20)),
		 (10.0f) - float (rand() % (20)));

    vel[i] += velocity;
    status[i] = 255;
  }
}

