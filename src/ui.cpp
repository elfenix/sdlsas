// Copyright 1994 Brad Pitzel
// Modifications Copyright 2002 Andrew M.
// 
// Feel free to use/distribute/modify as long as credit/copyrights for
// myself are included.
//
// File : Ui.c[1.0]
// Changed: Friday 11-OCT-2002

#include "sasteroids.h"


///////////////////////////////////////////////////////
// Static Class (Global) variables.

SDL_Surface *Ui::myscreen = 0;
TTF_Font *Ui::myfont = 0;

void (*Ui::pixelDriver)(SDL_Surface* visual, int x, int y, 
			char r, char g, char b);


///////////////////////////////////////////////////////
// Initialize SDL Library and Fonts

void Ui::init()
{
  resync(SCREEN_X, SCREEN_Y);
  
  SDL_WM_SetCaption("SDL Sasteroids", "SDL Sasteroids");
  SDL_ShowCursor(0);
  
  if (TTF_Init() < 0) {
    throw "Couldn't initialize True type font library";
  }
  atexit(TTF_Quit);

  myfont = TTF_OpenFont(BINDIR "/fonts/nicefont.ttf", 24);
  if (!myfont) {
    throw "Couldn't open font file";
  }

  TTF_SetFontStyle(myfont, TTF_STYLE_NORMAL);
}

////////////////////////////////////////////////
// Called when we are done with this library

void Ui::restore()
{
  TTF_CloseFont(myfont);
}


////////////////////////////////////////////////
// Text Manipulation

void Ui::CenterText(char* msg)
{
  int y, x, height, width;
  
  TTF_SizeText(myfont, msg, &width, &height);
  
  x = WIDTH() - width;
  y = HEIGHT() - height;
  x /= 2;
  y /= 2;
  
  ShowText(x, y, msg);
}


void Ui::CenterXText(int y, char* msg)
{
  int x, height, width;
  
  TTF_SizeText(myfont, msg, &width, &height);
  
  x = WIDTH() - width;
  x /= 2;
  
  ShowText(x, y, msg); 
}


void Ui::ShowText(int x, int y, char *msg)
{
  ShowTextColor(x, y, msg, 255, 255, 255);
}


void Ui::ShowTextColor(int x, int y, char *msg, char r, char g, char b)
{
  /* Note: for compatibility with old code, new code should create bitmaps,
     and cache the results.... */

  SDL_Surface *surface = 0;
  SDL_Rect dest;
  SDL_Color mycolor = {
    r, g, b};

  dest.x = x;
  dest.y = y;

#ifndef QUICK_FONTS
  surface = TTF_RenderText_Blended(myfont, msg, mycolor);
#else
  surface = TTF_RenderText_Solid(myfont, msg, mycolor);
#endif

  if(!surface) return;

  SBitmap text(surface);
  text.putA(float(x), Ui::HEIGHT() - float(y));
  
}


SDL_Surface* Ui::get_text(char* msg, char r, char g, char b)
{
  SDL_Surface* surface = 0;
  SDL_Color mycolor = { r, g, b};

  surface = TTF_RenderText_Blended(myfont, msg, mycolor);
  return surface;
}


void Ui::resync(int newX, int newY)
{
  Uint32 flags;

#ifdef WANT_OPENGL
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

#else
  
  flags = SDL_HWSURFACE | SDL_ANYFORMAT | WANT_FULLSCREEN;
  if(wantFullScreen) flags |= SDL_FULLSCREEN;
  myscreen = SDL_SetVideoMode(newX, newY, 16, flags);

  if(!myscreen) {
    throw "Resize Failed, Bailing out.";
  }

#endif



}


// update physical screen from virtualScn
void Ui::updateScreen()
{
#ifdef WANT_OPENGL
  SDL_GL_SwapBuffers();
#else
  SDL_Flip(myscreen);
#endif
}


///////////////////////////////////////////////////
// Pixel Functions and drivers.

void g_getpixelB1(SDL_Surface* visual, int x, int y, char *r, char *g, char *b)
{
  Uint32 color;
  Uint8 *ubuff8;
  
  ubuff8 = (Uint8 *) visual->pixels;
  ubuff8 += (y * visual->pitch) + x;
  color = (Uint32) (*ubuff8);
  SDL_GetRGB(color, visual->format, (Uint8*) r, (Uint8*) g, (Uint8*) b);
}


void g_setpixelB1(SDL_Surface* visual, int x, int y, char r, char g, char b)
{
  Uint32 color;
  Uint8 *ubuff8;
  
  color = SDL_MapRGB(visual->format, r, g, b);
  ubuff8 = (Uint8 *) visual->pixels;
  ubuff8 += (y * visual->pitch) + x;
  *ubuff8 = (Uint8) color;
}


void g_getpixelB2(SDL_Surface* visual, int x, int y, char *r, char *g, char *b)
{
  Uint32 color;
  Uint8 *ubuff8;
  Uint16 *ubuff16;

  ubuff8 = (Uint8 *) visual->pixels;
  ubuff8 += (y * visual->pitch) + (x * visual->format->BytesPerPixel);
  ubuff16 = (Uint16 *) ubuff8;
  color = (Uint32) *ubuff16;
  SDL_GetRGB(color, visual->format, (Uint8*) r, (Uint8*) g, (Uint8*) b);
}


void g_setpixelB2(SDL_Surface* visual, int x, int y, char r, char g, char b)
{
  Uint32 color;
  Uint8 *ubuff8;
  Uint16 *ubuff16;

  color = SDL_MapRGB(visual->format, r, g, b);
  ubuff8 = (Uint8 *) visual->pixels;
  ubuff8 += (y * visual->pitch) + (x * visual->format->BytesPerPixel);
  ubuff16 = (Uint16 *) ubuff8;

  *ubuff16 = (Uint16) color;
}


void g_getpixelB3(SDL_Surface* visual, int x, int y, char *r, char *g, char *b)
{
  Uint8 *ubuff8;
  Uint32 color;
  
  ubuff8 = (Uint8 *) visual->pixels;        // This is probably broken.
  ubuff8 += (y * visual->pitch) + (x * 3);  // TODO: Fix this.
  
  color = ((Uint32)ubuff8[2]) << 16;
  color |= ((Uint32)ubuff8[1]) << 8;
  color |= ((Uint32)ubuff8[0]);
  
  SDL_GetRGB(color, visual->format, (Uint8*)r, (Uint8*)g, (Uint8*)b);  
}


void g_setpixelB3(SDL_Surface* visual, int x, int y, char r, char g, char b)
{
  Uint32 color;
  Uint8 *ubuff8;

  color = SDL_MapRGB(visual->format, r, g, b);
  ubuff8 = (Uint8 *) visual->pixels;
  ubuff8 += (y * visual->pitch) + (x * 3);
  r = (color >> visual->format->Rshift) & 0xFF;
  g = (color >> visual->format->Gshift) & 0xFF;
  b = (color >> visual->format->Bshift) & 0xFF;
  ubuff8[0] = r;
  ubuff8[1] = g;
  ubuff8[2] = b;
}


void g_getpixelB4(SDL_Surface* visual, int x, int y, char *r, char *g, char *b)
{
  Uint32 color;
  Uint32* ubuff32;

  ubuff32 = (Uint32 *) visual->pixels;
  ubuff32 += ((y*visual->pitch) >> 2) + x;
  color = *ubuff32;
  
  SDL_GetRGB(color, visual->format, (Uint8*)r, (Uint8*)g, (Uint8*)b);
}


void g_setpixelB4(SDL_Surface* visual, int x, int y, char r, char g, char b)
{
  Uint32 color;
  Uint32 *ubuff32;

  color = SDL_MapRGB(visual->format, r, g, b);
  ubuff32 = (Uint32 *) visual->pixels;
  ubuff32 += ((y * visual->pitch) >> 2) + x;
  *ubuff32 = color;
}


void getpixel(SDL_Surface *visual, int x, int y, char *r, char *g, char *b)
{
  if(!visual) return;
  if(x < 0 || y < 0) return;
  if(x > visual->w || y > visual->h) return; 
   
  switch (visual->format->BytesPerPixel) {
  case 1:
    g_getpixelB1(visual, x, y, r, g, b);
    break;
  case 2:
    g_getpixelB2(visual, x, y, r, g, b);
    break;
  case 3:
    g_getpixelB3(visual, x, y, r, g, b);
    break;
  case 4:
    g_getpixelB4(visual, x, y, r, g, b);
    break;
  default:
    cerr << "[Warning] getpixel called with unknown bitdepth" << endl;
  }
}


void setpixel(SDL_Surface *visual, int x, int y, char r, char g, char b)
{
  if(!visual) return;
  if(x < 0 || y < 0) return;
  if(x > visual->w || y > visual->h) return; 
   
  switch (visual->format->BytesPerPixel) {
  case 1:
    g_setpixelB1(visual, x, y, r, g, b);
    break;
  case 2:
    g_setpixelB2(visual, x, y, r, g, b);
    break;
  case 3:
    g_setpixelB3(visual, x, y, r, g, b);
    break;
  case 4:
    g_setpixelB4(visual, x, y, r, g, b);
    break;
  default:
    cerr << "[Warning] setpixel called with unknown bitdepth" << endl;
  }
}



void Ui::predraw() {
#ifdef WANT_OPENGL
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
  
  glLoadIdentity();
  glViewport(0, 0, 640, 400);
  glOrtho(0, 640, 0, 400, -1, 1);
#endif
}
  


void GraphicsStartDraw(SDL_Surface* visual)
{
  if(SDL_MUSTLOCK(visual))
     SDL_LockSurface(visual);
}

void GraphicsStopDraw(SDL_Surface* visual)
{
  if(SDL_MUSTLOCK(visual))
    SDL_UnlockSurface(visual);
}





//////////////////////////////////////
// Integer Display

SBitmap *IntegerDisplay::numbers;


void IntegerDisplay::initialize() 
{
  int i;
  char sstring[2] = { 0, 0 };
  numbers = new SBitmap[10];

  for(i = 0; i < 10; i++) {
    SDL_Surface* tmp;

    sstring[0] = i + '0';
    tmp = Ui::get_text(sstring, 255, 255, 255);
    numbers[i].LoadSurface(tmp);
    SDL_FreeSurface(tmp);
  }
}


void IntegerDisplay::finish() 
{
  delete [] numbers;
}


void IntegerDisplay::display_integer(int num, float x, float y) 
{
  int tenPower = 1;
  int tnum;
  float cx;


  if(num < 0) num = 0;
  while(tenPower <= num) tenPower *= 10;

  cx = x;
  do {
    tnum = num % tenPower;
    
    if(tenPower != 1) tenPower = tenPower / 10;
    tnum = tnum / tenPower;
    if(tnum < 0) tnum = 0;
    if(tnum > 9) tnum = 9;
    
    numbers[tnum].putA(cx, y);
    cx += numbers[tnum].width();

  } while(tenPower > 1);
}
