/*
 * ObjectManager.cpp
 *
 *  Created on: Feb 25, 2012
 *      Author: andrew
 */


#include "PlayingField.hpp"
#include "sasteroids.h"


GameEntity *ObjectList[MAX_OBJECTS];
Vector ScreenLimits(PLAY_X, PLAY_Y);

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

	for (i = 1; i < MAX_OBJECTS; i++)
	{
		ObjectList[i] = 0;
	}

	ObjectList[0] = (GameEntity*)&PlayerShip;
}


void PlayingField::free_objects()
{
  int i;

  ObjectList[0] = (GameEntity*)&PlayerShip;
  for (i = 1; i < MAX_OBJECTS; i++) {
	if (ObjectList[i])
		delete ObjectList[i];

	ObjectList[i] = 0;
  }
}


void PlayingField::draw_objects()
{
	int i;
	for (i = 0; i < MAX_OBJECTS; i++)
	{
		if (ObjectList[i] && ObjectList[i]->alive())
		{
			ObjectList[i]->draw();
		}
	}
}


PlayingField::object_id PlayingField::get_open_object()
{
	int i;

	for (i = 1; i < MAX_OBJECTS; i++)
	{
		if (!ObjectList[i])
			return i;
	}

	return -1;
}


PlayingField::object_id PlayingField::register_object( GameEntity* screen_object )
{
	object_id openObject = get_open_object();
	if (openObject == -1)
	{
		delete screen_object;
		return -1;
	}
	ObjectList[openObject] = screen_object;
	return openObject;
}


int PlayingField::move_game_time( int p_time_ms )
{
	int i = 0, crash = 0, touched = 0, j = 0;

	// Loop through every object.
	for (i = 0; i < MAX_OBJECTS; i++)
	{
		if (ObjectList[i] && ObjectList[i]->alive())
		{
			ObjectList[i]->tick();

			for (j = 1; j < MAX_OBJECTS; j++)
			{
				if (!ObjectList[j])
					continue;
				if (!ObjectList[j]->alive())
					continue;
				if( i == j )
					continue;

				if (collide(ObjectList[i], ObjectList[j]))
				{
					ObjectList[i]->collision( *ObjectList[j] );
				}
			}
		}
		else if (ObjectList[i] && i != 0)
		{
			delete ObjectList[i];
			ObjectList[i] = 0;
		}
	}

	return crash;
}
