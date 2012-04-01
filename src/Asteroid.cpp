/*************************************************************/
/* Original Files Copyright 1994, Brad Pitzel                */
/* Modifications Copyright 2003, Andrew McCall               */
/*************************************************************/
/* Feel free to use/distribute/modify as long as credit      */
/* copyrights for myself and contributors are included.      */
/*************************************************************/

#include "sasteroids.h"
#include "Asteroid.hpp"
#include "PlayingField.hpp"
#include "ScreenBitmap.hpp"
#include <stdexcept>

const float BigAstMax = 4.0f;
const float MedAstMax = 12.0f;
const float SmallAstMax = 20.0f;

const float START_DIST = 70; // Disance asteroids start from ship
const int MAXASTEROIDS = 10; // Max # Asteroids

const float MassBigAst = 8.0f; // Masses for bouncing + collision
const float MassSmallAst = 0.8f;
const float MassMedAst = 4.0f;

////////////////////////////////////////////////////////////////////////////
// create asteroids for beginning of each level/game
void GenerateAsteroids( PlayingField& p_field )
{
    int i, obj;
    float x, y;
    Vector temp;

    for( i = 0; i <= (Glevel / 2 + 1) && i <= MAXASTEROIDS; i++ )
    {
        do
        {
            x = (float) (rand() % 320);
            y = (float) (rand() % 200);
            temp.SetXY( x, y );
        } while( PlayerShip.get_pos().length( temp ) < START_DIST );

        Asteroid::create_asteroid( p_field, x, y, rand() % 3, rand() % 3,
                Asteroid::ASTEROID_BIG );

        if( !(rand() % 3) && !ClassicMode )
        {
            do
            {
                x = (float) (rand() % 640);
                y = (float) (rand() % 400);
                temp.SetXY( x, y );
            } while( PlayerShip.get_pos().length( temp ) < START_DIST );
            Asteroid::create_asteroid( p_field, x, y, rand() % 5, rand() % 5,
                    Asteroid::ASTEROID_MEDIUM );
        }

        if( !(rand() % 6) && !ClassicMode )
        {
            do
            {
                x = (float) (rand() % 320);
                y = (float) (rand() % 200);
                temp.SetXY( x, y );
            } while( PlayerShip.get_pos().length( temp ) < START_DIST );
            Asteroid::create_asteroid( p_field, x, y, rand() % 7, rand() % 7,
                    Asteroid::ASTEROID_SMALL );
        }
    }
}

/********************************************/
/* Asteroid                                 */
/********************************************/

Asteroid::Asteroid()
{
    numasts++;
}

Asteroid::~Asteroid()
{
    numasts--;
}

void Asteroid::create_explosion()
{
    if( get_field() )
    {
        get_field()->register_object(
                new Explosion( GetX(), GetY(), get_velocity().GetX(),
                        get_velocity().GetY() ) );
    }
    PlaySound( SND_BOOM_B );
}

Asteroid* Asteroid::create_asteroid( PlayingField& p_field, float x, float y,
        float xv, float yv, ObjectType type )
{
    Asteroid *new_asteroid = 0;
    PlayingField::object_id id;

    switch( type )
    {
    case ASTEROID_BIG:
        new_asteroid = new LargeAsteroid( x, y, xv, yv );
        break;

    case ASTEROID_SMALL:
        new_asteroid = new SmallAsteroid( x, y, xv, yv );
        break;

    case ASTEROID_MEDIUM:
        new_asteroid = new MediumAsteroid( x, y, xv, yv );
        break;

    default:
        throw std::runtime_error( "Unknown Asteroid Type in CreateAsteroid" );
    }

    id = p_field.register_object( new_asteroid );
    if( id < 0 )
    {
        new_asteroid = 0;
    }

    return new_asteroid;
}

bool Asteroid::destructive_collision( const GameEntity& p_other )
{
    numasts -= 1;
    create_explosion();
    create_children();

    die();
    return true;
}

/********************************************/
/* Small Asteroid */
/********************************************/

void SmallAsteroid::initialize( float x, float y, float xv, float yv )
{
    if( !ClassicMode && !(rand() % LevelOdds( 50, 14 )) )
    {
        m_evil = true;
        set_bitmap( &bmp_asteroid_evil_small );
    }
    else
    {
        m_evil = false;
        objtype = ASTEROID_SMALL;
        set_bitmap( &bmp_asteroid_small );
    }

    numasts++;
    restore();
    SetXY( x, y );
    set_acceleration( 0.0f, 0.0f );
    set_angular_velocity( 2.0 - (double( rand() % 1024 ) * 4.0 / 1024.0) );
    SetVel( xv, yv );
    set_wrap_moves( true );
    SetSize( 20, 20 );
    set_size( 14 );
    set_mass( MassSmallAst );
    SetMaxSpeed( SmallAstMax );
}

SmallAsteroid::SmallAsteroid()
{
    initialize( 0, 0, 1.0f, 1.0f );
}

SmallAsteroid::SmallAsteroid( float x, float y, float xv, float yv )
{
    initialize( x, y, xv, yv );
}

void SmallAsteroid::create_children()
{
    if( m_evil && get_field() )
    {
        for( angle = 0; angle < 220; angle += 30 )
        {
            get_field()->register_object(
                    new Bullet( this->get_pos(), true, angle, type() ) );
        }
    }
}

/********************************************/
/* Medium Asteroid */
/********************************************/

void MediumAsteroid::initialize( float x, float y, float xv, float yv )
{
    numasts++;
    settype( ASTEROID_MEDIUM );
    restore();
    SetXY( x, y );
    set_acceleration( 0.0f, 0.0f );
    set_angular_velocity( 1.5 - (double( rand() % 1024 ) * 3.0 / 1024.0) );
    SetVel( xv, yv );
    set_bitmap( &bmp_asteroid_medium );
    set_wrap_moves( true );
    SetSize( 40, 40 );
    set_size( 20 );
    set_mass( MassMedAst );
    SetMaxSpeed( MedAstMax );
}

MediumAsteroid::MediumAsteroid()
{
    initialize( 0, 0, 1.0f, 1.0f );
}

MediumAsteroid::MediumAsteroid( float x, float y, float xv, float yv )
{
    initialize( x, y, xv, yv );
}

void MediumAsteroid::create_children()
{
    char rA1, rA2;
    float px, py, vx, vy;

    rA1 = rand() % 255;
    rA2 = int( rA1 + 128 ) % 255;

    px = GetX();
    py = GetY();
    vx = get_velocity().GetX();
    vy = get_velocity().GetY();

    if( get_field() )
    {
        PlayingField& field = *get_field();
        create_asteroid( field, px, py, f_math::sin( rA1 ) * 2.0f + vx,
                f_math::cos( rA1 ) * 2.0f + vy, ASTEROID_SMALL );

        create_asteroid( field, px, py, f_math::sin( rA2 ) * 2.0f + vx,
                f_math::cos( rA2 ) * 2.0f + vy, ASTEROID_SMALL );

        do
        {
            rA2 = rand() % 255;
        } while( abs( rA2 - rA1 ) < 10 && abs( rA2 - (rA1 + 124) ) < 10 );

        rA1 = int( rA2 + 128 ) % 255;
        create_asteroid( field, px, py, f_math::sin( rA1 ) * 2.0f + vx,
                f_math::cos( rA1 ) * 2.0f + vy, ASTEROID_SMALL );

        create_asteroid( field, px, py, f_math::sin( rA2 ) * 2.0f + vx,
                f_math::cos( rA2 ) * 2.0f + vy, ASTEROID_SMALL );
    }
}

/********************************************/
/* Large Asteroid */
/********************************************/

void LargeAsteroid::initialize( float x, float y, float xv, float yv )
{
    numasts++;
    settype( ASTEROID_BIG );
    restore();
    SetXY( x, y );
    set_acceleration( 0.0f, 0.0f );
    set_angular_velocity( 1.0 - (double( rand() % 1024 ) * 2.0 / 1024.0) );
    SetVel( xv, yv );
    set_bitmap( &bmp_asteroid_large );
    set_wrap_moves( true );
    SetSize( 80, 80 );
    set_size( 40 );
    set_mass( MassBigAst );
    SetMaxSpeed( BigAstMax );
}

LargeAsteroid::LargeAsteroid()
{
    initialize( 0, 0, 1.0f, 1.0f );
}

LargeAsteroid::LargeAsteroid( float x, float y, float xv, float yv )
{
    initialize( x, y, xv, yv );
}

void LargeAsteroid::create_children()
{
    char rA1, rA2;
    float px, py, vx, vy;

    rA1 = rand() % 255;
    rA2 = int( rA1 + 128 ) % 255;

    px = GetX();
    py = GetY();
    vx = get_velocity().GetX();
    vy = get_velocity().GetY();

    if( get_field() )
    {
        PlayingField& field = *get_field();

        create_asteroid( field, px, py, f_math::sin( rA1 ) * 2.0f + vx,
                f_math::cos( rA1 ) * 2.0f + vy, ASTEROID_MEDIUM );

        create_asteroid( field, px, py, f_math::sin( rA2 ) * 2.0f + vx,
                f_math::cos( rA2 ) * 2.0f + vy, ASTEROID_MEDIUM );

        do
        {
            rA2 = rand() % 255;
        } while( abs( rA2 - rA1 ) < 10 && abs( rA2 - (rA1 + 124) ) < 10 );

        rA1 = int( rA2 + 128 ) % 255;
        create_asteroid( field, px, py, f_math::sin( rA1 ) * 2.0f + vx,
                f_math::cos( rA1 ) * 2.0f + vy, ASTEROID_MEDIUM );

        if( /* !(rand() % LevelOdds(16, 7)) */true )
        {
            GameEntity* spinner_ptr = new Spinner;
            spinner_ptr->SetXY( px, py );
            spinner_ptr->SetVel( f_math::sin( rA2 ) * 2.0f + vx,
                    f_math::cos( rA2 ) * 2.0f + vy );
            field.register_object( spinner_ptr );
        }
        else
        {
            create_asteroid( field, px, py, f_math::sin( rA2 ) * 2.0f + vx,
                    f_math::cos( rA2 ) * 2.0f + vy, ASTEROID_MEDIUM );
        }
    }
}
