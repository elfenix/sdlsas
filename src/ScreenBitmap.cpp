/*************************************************************/
/* Original Files Copyright 1994, Brad Pitzel                */
/* Modifications Copyright 2003, Andrew McCall               */
/*************************************************************/
/* Feel free to use/distribute/modify as long as credit      */
/* copyrights for myself and contributors are included.      */
/*************************************************************/

#include <sasteroids.h>
#include <ScreenBitmap.hpp>
#include <fastmath.h>

#include <stdexcept>
#include <cstring>
#include <cassert>


/*************************************************************/
/* Convert SDL_Surfaces to GL Textures  **********************/
/*************************************************************/

static void sb_getpixelB1(SDL_Surface* v, SDL_Surface* d, int x, int y, Uint32* color)
{  
  Uint32 tmp;
  Uint8 *ubuff8;
  Uint8 r, g, b;
  
  ubuff8 = (Uint8 *) v->pixels;
  ubuff8 += (y * v->pitch) + x;
  tmp = (Uint32) (*ubuff8);
  SDL_GetRGB(tmp, v->format, (Uint8*) &r, (Uint8*) &g, (Uint8*) &b);
  *color = SDL_MapRGB(d->format, r, g, b);
}


static void sb_getpixelB2(SDL_Surface* v, SDL_Surface* d, int x, int y, Uint32* color)
{
  Uint32 tmp;
  Uint8 *ubuff8;
  Uint16 *ubuff16;
  Uint8 r, g, b;

  ubuff8 = (Uint8 *) v->pixels;
  ubuff8 += (y * v->pitch) + (x * v->format->BytesPerPixel);
  ubuff16 = (Uint16 *) ubuff8;
  tmp = (Uint32) *ubuff16;
 
  SDL_GetRGB(tmp, v->format, (Uint8*) &r, (Uint8*) &g, (Uint8*) &b);
  *color = SDL_MapRGB(d->format, r, g, b);
}


static void sb_getpixelB3(SDL_Surface* v, SDL_Surface* d, int x, int y, Uint32* color)
{
  Uint8 *ubuff8;
  Uint32 tmp;
  Uint8 r, g, b;
  
  ubuff8 = (Uint8 *) v->pixels;        // This is probably broken.
  ubuff8 += (y * v->pitch) + (x * 3);  // TODO: Fix this.
  
  tmp = ((Uint32)ubuff8[2]) << 16;
  tmp |= ((Uint32)ubuff8[1]) << 8;
  tmp |= ((Uint32)ubuff8[0]);
  
  SDL_GetRGB(tmp, v->format, (Uint8*)&r, (Uint8*)&g, (Uint8*)&b);  
  *color = SDL_MapRGB(d->format, r, g, b);
}


static void sb_getpixelB4(SDL_Surface* v, SDL_Surface* d, int x, int y, Uint32* color)
{
  Uint32* ubuff32;
  ubuff32 = (Uint32 *) v->pixels;
  ubuff32 += ((y*v->pitch) >> 2) + x;
  *color = *ubuff32;
}


static void sb_setpixelB4(SDL_Surface* visual, int x, int y, Uint32 color)
{
  Uint32 *ubuff32;

  ubuff32 = (Uint32 *) visual->pixels;
  ubuff32 += ((y * visual->pitch) >> 2) + x;
  *ubuff32 = color;
}


GLuint ScreenBitmap::GL_LoadTexture(SDL_Surface *surface, ScreenBitmap::TexCoord& texcoord)
{
	Uint32 x = 0, y = 0;
	Uint32 w = 0, h = 0;
	Uint32 color = 0;
	GLuint texture = 0;
	SDL_Surface *image = 0;

	if( !surface || !surface->format )
	{
		throw std::runtime_error( "Attempt to load non-existent of invalid surface" );
	}

	w = f_math::next_pow2( surface->w );
	h = f_math::next_pow2( surface->h );

	texcoord.x1 = 0.0f;			  // Min X
	texcoord.y1 = 0.0f;			  // Min Y
	texcoord.x2 = (GLfloat)surface->w / w;  // Max X
	texcoord.y2 = (GLfloat)surface->h / h;  // Max Y

	image = SDL_CreateRGBSurface(
		   SDL_SWSURFACE,
		   w, h,
		   32,
	#if SDL_BYTEORDER == SDL_LIL_ENDIAN // OpenGL RGBA masks
		   0x000000FF,
		   0x0000FF00,
		   0x00FF0000,
		   0xFF000000
	#else
		   0xFF000000,
		   0x00FF0000,
		   0x0000FF00,
		   0x000000FF
	#endif
		   );
	if( !image )
		throw std::runtime_error( "Unable to create texture RGB surface");

	std::memset( image->pixels, 0, w*h*4 );
	for( x = 0; x < (Uint32) surface->w; x++ )
	{
		for (y = 0; y < (Uint32) surface->h; y++)
		{
			switch (surface->format->BytesPerPixel)
			{
			case 1:
				sb_getpixelB1(surface, image, x, y, &color);
				break;
			case 2:
				sb_getpixelB2(surface, image, x, y, &color);
				break;
			case 3:
				sb_getpixelB3(surface, image, x, y, &color);
				break;
			case 4:
				sb_getpixelB4(surface, image, x, y, &color);
				break;
			default:
				throw std::runtime_error( "Unable to load texture from unsupported format" );
			}

			sb_setpixelB4(image, x, y, color);
		}
	}

	// Create an OpenGL texture for the image
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels );
	SDL_FreeSurface(image); // No longer needed

	return texture;
}

/**********************************************************/
/* SBitmap Class ******************************************/
/**********************************************************/

ScreenBitmap::ScreenBitmap() :
	m_texture( false )
{
}


ScreenBitmap::ScreenBitmap( const ScreenBitmap& p_other ) :
	m_texture( p_other.m_texture ),
	m_width( p_other.m_width ),
	m_height( p_other.m_height ),
	m_half_width( p_other.m_half_width ),
	m_half_height( p_other.m_half_height ),
	m_tex_coord( p_other.m_tex_coord ),
	m_texture_owned( false )
{
}


ScreenBitmap::ScreenBitmap( SDL_Surface* surface ) :
	m_texture( false )
{
	load_surface( surface );
}


ScreenBitmap::ScreenBitmap( const char* filename ) :
	m_texture( false )
{
	load_image( filename );
}


ScreenBitmap::~ScreenBitmap()
{
	if(m_texture_owned)
	{
		glDeleteTextures( 1, &m_texture );
	}
}


void ScreenBitmap::load_surface( SDL_Surface* p_surface )
{
	if( !p_surface )
	{
		throw std::runtime_error( "Attempted to load NULL surface" );
	}

	m_width = float(p_surface->w);
	m_height = float(p_surface->h);
	m_half_width = m_width / 2.0f;
	m_half_height = m_height / 2.0f;

	m_texture = GL_LoadTexture(p_surface, m_tex_coord);
	if(!m_texture)
	{
		throw std::runtime_error( "Loading texture from surface failed" );
	}
}


void ScreenBitmap::load_image( const char* file )
{
  SDL_Surface* surface = IMG_Load(file);
  if(!surface)
  {
	  throw std::runtime_error( std::string( "Couldn't load bitmap from file: ") + file );
  }

  m_width = GLfloat(surface->w);
  m_height = GLfloat(surface->h);
  m_half_width = m_width / 2.0f;
  m_half_height = m_height / 2.0f;

  m_texture = GL_LoadTexture(surface, m_tex_coord);
  if(!m_texture)
  {
	  throw std::runtime_error( "Could not load texture" );
  }

  SDL_FreeSurface(surface);
}


void ScreenBitmap::draw_alpha(float x, float y, float layer, float alpha) 
{
	assert( m_texture != 0);

	glBindTexture(GL_TEXTURE_2D, m_texture);
	glColor4f(1.0f, 1.0f, 1.0f, alpha);

	glBegin(GL_QUADS);

	glTexCoord2f( m_tex_coord.x1, m_tex_coord.y2 );
	glVertex3f(x, y-m_height, layer);

	glTexCoord2f( m_tex_coord.x2, m_tex_coord.y2 );
	glVertex3f(x+m_width, y-m_height, layer);

	glTexCoord2f( m_tex_coord.x2, m_tex_coord.y1 );
	glVertex3f(x+m_width, y, layer);

	glTexCoord2f( m_tex_coord.x1, m_tex_coord.y1 );
	glVertex3f(x, y, layer);

	glEnd();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}


void ScreenBitmap::draw(float x, float y, float sf, float rotate, float layer) 
{
	assert( m_texture != 0 );

	glColor3f( 1.0, 1.0, 1.0 );
	glBindTexture( GL_TEXTURE_2D, m_texture );
	glPushMatrix();

	glTranslatef( x, y, layer );
	glRotatef( rotate, 0.0f, 0.0f, 1.0f );

	glBegin( GL_QUADS );
		glTexCoord2f( m_tex_coord.x1, m_tex_coord.y2 );
		glVertex3f(-m_half_width * sf, -m_half_height *sf, 0.0f);

		glTexCoord2f( m_tex_coord.x2, m_tex_coord.y2 );
		glVertex3f(m_half_width * sf, -m_half_height * sf, 0.0f);

		glTexCoord2f( m_tex_coord.x2, m_tex_coord.y1 );
		glVertex3f(m_half_width * sf, m_half_height * sf, 0.0f);

		glTexCoord2f( m_tex_coord.x1, m_tex_coord.y1 );
		glVertex3f(-m_half_width * sf, m_half_height * sf, 0.0f);
	glEnd();

	glPopMatrix();
}

