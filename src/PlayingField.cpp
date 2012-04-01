/*
 * ObjectManager.cpp
 *
 *  Created on: Feb 25, 2012
 *      Author: andrew
 */

#include "PlayingField.hpp"
#include "sasteroids.h"

Vector ScreenLimits( PLAY_X, PLAY_Y );

PlayingField::PlayingField()
{
    initialize_list();
}


PlayingField::~PlayingField()
{
}


void PlayingField::initialize_list()
{
    int i;

    m_game_entities.push_back( &PlayerShip );
    PlayerShip.set_field(this);
}


void PlayingField::free_objects()
{
    for( entity_iterator it = begin_nonplayer_entity(); it != end_nonplayer_entity(); it++)
    {
        GameEntity* entity_ptr = *it;

        if( entity_ptr )
            delete entity_ptr;
    }
}


void PlayingField::draw_objects()
{
    for( entity_iterator it = m_game_entities.begin(); it != m_game_entities.end(); it++ )
    {
        GameEntity* entity_ptr = *it;

        if( entity_ptr && entity_ptr->alive() )
            entity_ptr->draw();
    }
}


void PlayingField::register_object( GameEntity* screen_object )
{
    screen_object->set_field(this);
    m_game_entities.push_back(screen_object);
}


void PlayingField::move_game_time( int p_time_ms )
{
    int i = 0, touched = 0, j = 0;

    // Loop through every object.
    for( entity_iterator it = m_game_entities.begin(); it != m_game_entities.end(); it++ )
    {
        GameEntity* game_entity = *it;

        if( game_entity && game_entity->alive() )
        {
            game_entity->tick();

            for( entity_iterator check_it = begin_nonplayer_entity(); check_it != end_nonplayer_entity(); check_it++)
            {
                GameEntity* check_entity = *check_it;

                if( !check_entity )
                    continue;
                if( !check_entity->alive() )
                    continue;
                if( check_it == it )
                    continue;
                if( check_current_collision( *game_entity, *check_entity ) )
                {
                    game_entity->collision( *check_entity );
                }
            }
        }
    }

    // Clear dead and null elements
    for( entity_iterator it = begin_nonplayer_entity(); it != end_nonplayer_entity(); )
    {
        GameEntity* game_entity = *it;

        if( !game_entity )
        {
            it = m_game_entities.erase(it);
        } else if( !game_entity->alive() ) {
            delete game_entity;
            it = m_game_entities.erase(it);
        } else {
            it++;
        }
    }
}


bool PlayingField::check_current_collision( const GameEntity& b1, const GameEntity& b2 )
{
    float collision_distance = (float) b1.Size() + (float) b2.Size();
    float collision_distance_squared = collision_distance * collision_distance;
    float distance_squared = (b1.get_pos() - b2.get_pos()).lengthSqrd();

    return distance_squared <= collision_distance_squared;
}
