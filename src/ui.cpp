// Copyright 1994 Brad Pitzel
// Modifications Copyright 2002 Andrew M.
// 
// Feel free to use/distribute/modify as long as credit/copyrights for
// myself are included.
//
// File : Ui.c[1.0]
// Changed: Friday 11-OCT-2002

#include "sasteroids.h"
using namespace std;

#include <stdexcept>

///////////////////////////////////////////////////////
// Static Class (Global) variables.

SDL_Surface *UserInterfaceManager::myscreen = 0;
TTF_Font *UserInterfaceManager::myfont = 0;
ScreenBitmap *UserInterfaceManager::numbers = 0;

///////////////////////////////////////////////////////
// Initialize SDL Library and Fonts

void UserInterfaceManager::init()
{
  int i;
  char sstring[2] = { 0, 0 };

  resync(SCREEN_X, SCREEN_Y);
  
  SDL_WM_SetCaption("SDL Sasteroids", "SDL Sasteroids");
  SDL_ShowCursor(0);
  
  if( TTF_Init() < 0 )
  {
	  throw std::runtime_error( "Couldn't initialize True type font library" );
  }
  atexit(TTF_Quit);

  myfont = TTF_OpenFont("fonts/nicefont.ttf", 24);
  if( !myfont )
  {
	  throw std::runtime_error( "Couldn't open font file" );
  }

  TTF_SetFontStyle(myfont, TTF_STYLE_NORMAL);


  numbers = new ScreenBitmap[10];

  for(i = 0; i < 10; i++) {
    SDL_Surface* tmp;

    sstring[0] = i + '0';
    tmp = UserInterfaceManager::get_text(sstring, 255, 255, 255);
    numbers[i].load_surface(tmp);
    SDL_FreeSurface(tmp);
  }
}


////////////////////////////////////////////////
// Called when we are done with this library

void UserInterfaceManager::restore()
{
	TTF_CloseFont(myfont);
	delete [] numbers;
}


////////////////////////////////////////////////
// Text Manipulation

void UserInterfaceManager::CenterText( const char* msg )
{
  int y, x, height, width;
  
  TTF_SizeText(myfont, msg, &width, &height);
  
  x = WIDTH() - width;
  y = HEIGHT() - height;
  x /= 2;
  y /= 2;
  
  ShowText(x, y, msg);
}


void UserInterfaceManager::CenterXText(int y, const char* msg)
{
	int x, height, width;

	TTF_SizeText(myfont, msg, &width, &height);

	x = WIDTH() - width;
	x /= 2;

	ShowText(x, y, msg);
}


void UserInterfaceManager::ShowText(int x, int y, const char *msg)
{
	ShowTextColor(x, y, msg, 255, 255, 255);
}


void UserInterfaceManager::ShowTextColor(int x, int y, const char *msg, char r, char g, char b)
{
  /* Note: for compatibility with old code, new code should create bitmaps,
     and cache the results.... */

  SDL_Surface *surface = 0;
  SDL_Rect dest;
  SDL_Color mycolor = {
    r, g, b};

  dest.x = x;
  dest.y = y;

  surface = TTF_RenderText_Blended(myfont, msg, mycolor);
  if(!surface) return;

  ScreenBitmap text(surface);
  text.draw_alpha(float(x), UserInterfaceManager::HEIGHT() - float(y));
}


SDL_Surface* UserInterfaceManager::get_text( const char* msg, char r, char g, char b)
{
  SDL_Surface* surface = 0;
  SDL_Color mycolor = { r, g, b};

  surface = TTF_RenderText_Blended(myfont, msg, mycolor);
  return surface;
}


void UserInterfaceManager::resync(int newX, int newY)
{
  Uint32 flags;

  SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
  SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
  SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
  SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

  flags = SDL_OPENGL | SDL_ANYFORMAT | WANT_FULLSCREEN;
  if(wantFullScreen) flags |= SDL_FULLSCREEN;
  myscreen = SDL_SetVideoMode(newX, newY, 16, flags);

  if(!myscreen) {
    throw "Resize Failed, Bailing out.";
  }

  predraw();
}


// update physical screen from virtualScn
void UserInterfaceManager::updateScreen()
{
  SDL_GL_SwapBuffers();
}


void UserInterfaceManager::predraw()
{
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
  
  glLoadIdentity();
  glViewport(0, 0, 640, 400);
  glOrtho(0, 640, 0, 400, -1, 1);
}


void UserInterfaceManager::display_integer(int num, float x, float y) {
	int tenPower = 1;
	int tnum;
	float cx;

	if (num < 0)
		num = 0;
	while (tenPower <= num)
		tenPower *= 10;

	cx = x;
	do {
		tnum = num % tenPower;

		if (tenPower != 1)
			tenPower = tenPower / 10;
		tnum = tnum / tenPower;
		if (tnum < 0)
			tnum = 0;
		if (tnum > 9)
			tnum = 9;

		numbers[tnum].draw_alpha(cx, y);
		cx += numbers[tnum].width();

	} while (tenPower > 1);
}





