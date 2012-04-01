// Copyright 1994 Brad Pitzel
//
// Feel free to use/distribute/modify as long as credit/copyrights for myself 
// are included.
//

// File   : SBitmap.h[1.0]
// Changed: Friday 11-OCT-2002

// There are a few simple manipulation functions available too: 
// mainly rotation.
//
// Note the rotation routines aren't that quick. It is best to create all
// the rotated images you'll need before entering the actual game play,
// because calling rotate() during the game will probably slow things
// down too much.
//
// Finally: Don't assume degrees=0..359 for rotation. Look in FastMath.h
// to see how Angle and TRIGSIZE are defined.

#ifndef SCREEN_BITMAP_H
#define SCREEN_BITMAP_H

#include <SDL.h>
#include <GL/gl.h>
#include <string>

class ScreenBitmap
{
private:
	typedef struct {
		GLfloat x1, y1;
		GLfloat x2, y2;
	} TexCoord;

public:
	ScreenBitmap();
	ScreenBitmap( const char *file );
	ScreenBitmap( SDL_Surface* surface );
	ScreenBitmap( const ScreenBitmap& pb );

	~ScreenBitmap();

	void load_image( const char* path );
	void load_surface( SDL_Surface* surface );

	void draw(float x, float y, float sf = 1.0f, float rotate = 1.0f, float layer = 0.0f);
	void draw_alpha(float x, float y, float layer = 0.0f, float alpha = 1.0f);

	inline float width() const
	{
	  return m_width;
	};

	inline float height() const
	{
	  return m_height;
	};

private:
	static GLuint GL_LoadTexture(SDL_Surface *surface, ScreenBitmap::TexCoord& texcoord);

private:
	GLuint		m_texture;
	bool   		m_texture_owned;
	GLfloat 	m_width;
	GLfloat		m_height;
	GLfloat		m_half_width;
	GLfloat		m_half_height;
	TexCoord	m_tex_coord;
};

#endif
