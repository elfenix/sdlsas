/*************************************************************/
/* Original Files Copyright 1994, Brad Pitzel                */
/* Modifications Copyright 2003, Andrew McCall               */
/*************************************************************/
/* Feel free to use/distribute/modify as long as credit      */
/* copyrights for myself and contributors are included.      */
/*************************************************************/

#include "sasteroids.h"


#ifndef WANT_OPENGL
#include <SDL_rotozoom.h>
#endif



/*************************************************************/
/* Convert SDL_Surfaces to GL Textures  **********************/
/*************************************************************/
#ifdef WANT_OPENGL

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


/* Quick utility function for texture creation */
static int power_of_two(int input)
{
	int value = 1;

	while ( value < input ) {
		value <<= 1;
	}
	return value;
}


static GLuint SDL_GL_LoadTexture(SDL_Surface *surface, GLfloat *texcoord)
{
  Uint32 x, y;
  Uint32 w, h;
  Uint32 color;
  GLuint texture;
  SDL_Surface *image;
   
  if(!surface || !surface->format) {
    throw "Attempt to load non existant or invalid surface";
  }

  w = power_of_two(surface->w);
  h = power_of_two(surface->h);

  texcoord[0] = 0.0f;			  // Min X 
  texcoord[1] = 0.0f;			  // Min Y 
  texcoord[2] = (GLfloat)surface->w / w;  // Max X 
  texcoord[3] = (GLfloat)surface->h / h;  // Max Y 
  
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
  if ( image == NULL ) {
    return 0;
  }

  memset(image->pixels, 0, w*h*4);
    
  for(x = 0; x < (Uint32)surface->w; x++) {
    for(y = 0; y < (Uint32)surface->h; y++) {
      switch(surface->format->BytesPerPixel) 
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
	  cerr << "Attempt to load unsupported file format: ";
	  printf("%d\n", surface->format->BytesPerPixel);
	  exit(-1); 
	}
      sb_setpixelB4(image, x, y, color);
    }
  }
  

  // Create an OpenGL texture for the image 
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D,
	       0,
	       GL_RGBA,
	       w, h,
	       0,
	       GL_RGBA,
	       GL_UNSIGNED_BYTE,
	       image->pixels);
  SDL_FreeSurface(image); // No longer needed 
  
  return texture;
}

#endif


/**********************************************************/
/* SBitmap Class ******************************************/
/**********************************************************/

SBitmap::SBitmap()
{
  myTexture = 0;
}


SBitmap::SBitmap(const SBitmap& copy)
{
  int i;

  myTexture = copy.myTexture;
  myWidth = copy.myWidth;
  myHeight = copy.myHeight;
  myHWidth = copy.myHWidth;
  myHHeight = copy.myHHeight;
  for(i = 0; i < 4; i++)
    myTexcoord[i] = copy.myTexcoord[i];
  
  myTextureOwned = 0;
}


SBitmap::SBitmap(SDL_Surface* surface)
{
  myTexture = 0;
  LoadSurface(surface);
}


SBitmap::SBitmap(char* filename) 
{
  myTexture = 0;
  LoadImage(filename);
}


SBitmap::~SBitmap() {
#ifdef WANT_OPENGL
  if(myTextureOwned) {
    glDeleteTextures(1, &myTexture);
  }
#else
  if(myTextureOwned) {
    SDL_FreeSurface(myTexture);
  }
#endif
}


void SBitmap::LoadSurface(SDL_Surface* surface)
{
  if(!surface) { 
    throw "[Fatal] Attempt to load NULL Bitmap";
  }
  
  myWidth = float(surface->w);
  myHeight = float(surface->h);
  myHWidth = myWidth / 2.0f;
  myHHeight = myHeight / 2.0f;

#ifdef WANT_OPENGL
  myTexture = SDL_GL_LoadTexture(surface, myTexcoord);
  if(!myTexture) {
    throw "[Fatal] Could not load texture";
  }

#else
  myTexture = SDL_ConvertSurface(surface, surface->format, SDL_HWSURFACE|SDL_SRCALPHA);
  myTextureOwned = 1;
#endif


}


void SBitmap::LoadImage(char* file) 
{
  SDL_Surface* mySurface;

  mySurface = IMG_Load(file);
  if(!mySurface) {
    throw "[Fatal] Could not load bitmap";
  }

  myWidth = float(mySurface->w);
  myHeight = float(mySurface->h);
  myHWidth = myWidth / 2.0f;
  myHHeight = myHeight / 2.0f;

#ifdef WANT_OPENGL
  myTexture = SDL_GL_LoadTexture(mySurface, myTexcoord);
  if(!myTexture) {
    throw "[Fatal] Could not load texture";
  }
  SDL_FreeSurface(mySurface);
#else
  myTexture = mySurface;
  myTextureOwned = 1;
#endif

}


void SBitmap::putA(float x, float y, float layer) 
{
  if(!myTexture) return;

#ifdef WANT_OPENGL
  glBindTexture(GL_TEXTURE_2D, myTexture);

  glBegin(GL_QUADS);

  glTexCoord2f( myTexcoord[0], myTexcoord[3] );
  glVertex3f(x, y-myHeight, layer);
  
  glTexCoord2f( myTexcoord[2], myTexcoord[3] );
  glVertex3f(x+myWidth, y-myHeight, layer);

  glTexCoord2f( myTexcoord[2], myTexcoord[1] );
  glVertex3f(x+myWidth, y, layer);
  
  glTexCoord2f( myTexcoord[0], myTexcoord[1] );
  glVertex3f(x, y, layer);
  
  glEnd();
#else
  if(!myTexture) return;

  SDL_Rect dstRect;

  dstRect.x = (int) x;
  dstRect.y = Ui::HEIGHT() - (int) y;
  
  SDL_BlitSurface(myTexture, NULL, Ui::myscreen, &dstRect);
#endif
}


void SBitmap::put(float x, float y, float sf, float rotate, float layer) 
{
  if(!myTexture) return;

#ifdef WANT_OPENGL
  glBindTexture(GL_TEXTURE_2D, myTexture);
  glPushMatrix();

  glTranslatef(x, y, layer);
  glRotatef(rotate, 0.0f, 0.0f, 1.0f);

  glBegin(GL_QUADS);
  
  glTexCoord2f( myTexcoord[0], myTexcoord[3] );
  glVertex3f(-myHWidth * sf, -myHHeight *sf, 0.0f);
  
  glTexCoord2f( myTexcoord[2], myTexcoord[3] );
  glVertex3f(myHWidth * sf, -myHHeight * sf, 0.0f);
  
  glTexCoord2f( myTexcoord[2], myTexcoord[1] );
  glVertex3f(myHWidth * sf, myHHeight * sf, 0.0f);
  
  glTexCoord2f( myTexcoord[0], myTexcoord[1] );
  glVertex3f(-myHWidth * sf, myHHeight * sf, 0.0f);

  glEnd();
  glPopMatrix();
#else
  SDL_Surface* tmp;
  SDL_Rect dstRect;
  int cenX, cenY;

  cenX = myTexture->w >> 1;
  cenY = myTexture->h >> 1;

  if(rotate != 0 || sf != 1.0f) {
    
    tmp = rotozoomSurface(myTexture, rotate, sf, 0);
    
    dstRect.x = (myTexture->w - tmp->w) >> 1;
    dstRect.y = (myTexture->h - tmp->h) >> 1;

    dstRect.x -= cenX; dstRect.x += int(x);
    dstRect.y -= cenY; dstRect.y += Ui::HEIGHT() - int(y); // dstRect.y = Ui::HEIGHT() - dstRect.y;

    SDL_BlitSurface(tmp, NULL, Ui::myscreen, &dstRect);
    SDL_FreeSurface(tmp);
  } else {
    dstRect.x = -cenX; dstRect.y = -cenY;
    dstRect.x += int(x); dstRect.y += Ui::HEIGHT() - int(y);
    SDL_BlitSurface(myTexture, NULL, Ui::myscreen, &dstRect);
  }

#endif
}
  
