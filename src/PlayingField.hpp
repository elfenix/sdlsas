/*
 * ObjectManager.h
 *
 *  Created on: Feb 12, 2012
 *      Author: andrew
 */

#ifndef PLAYINGFIELD_HPP_
#define PLAYINGFIELD_HPP_

#include <list>
#include <vector.h>

class GameEntity;


class PlayingField
{
public:
    typedef std::list<GameEntity*>::iterator entity_iterator;
    typedef std::list<GameEntity*>::const_iterator entity_const_iterator;

public:
    PlayingField();
    virtual ~PlayingField();

    void initialize_list();
    void free_objects();
    void draw_objects();

    const Vector& get_field_size() const
    {
        return m_field_size;
    }

    void set_field_size( const Vector& p_field_size )
    {
        m_field_size = p_field_size;
    }

    void register_object( GameEntity* screen_object );
    void move_game_time( int p_time_ms );

    bool check_current_collision( const GameEntity& p_1, const GameEntity& p_2 );

protected:
    entity_iterator begin_nonplayer_entity() {
        entity_iterator it = m_game_entities.begin();
        it++;
        return it;
    }

    entity_iterator end_nonplayer_entity() {
        return m_game_entities.end();
    }

private:
    Vector                  m_field_size;       ///< Field Dimensions
    std::list<GameEntity*>  m_game_entities;    ///< All tracked game elements
};

#include "Alien.hpp"
#include "Asteroid.hpp"
#include "Bullet.hpp"
#include "Explosion.hpp"
#include "Player.hpp"
#include "Spinner.hpp"
#include "PowerUp.hpp"

extern Player PlayerShip;
extern int ClassicMode;

#endif /* OBJECTMANAGER_H_ */
