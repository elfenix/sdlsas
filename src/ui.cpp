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
    fprintf(stderr, "Couldn't initialize TTF library!\n");
    exit(-1);
  }
  atexit(TTF_Quit);
  
  myfont = TTF_OpenFont(BINDIR "/fonts/Wargames.ttf", DMULTCONST(9));
  if (!myfont) {
    fprintf(stderr, "Couldn't grab TTF font!\n");
    exit(-1);
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

    SDL_BlitSurface(surface, NULL, myscreen, &dest);
    SDL_FreeSurface(surface);
}


void Ui::resync(int newX, int newY)
{
  Uint32 flags;

  flags = SDL_HWSURFACE | SDL_ANYFORMAT | SDL_RESIZABLE;
  if(wantFullScreen) flags |= SDL_FULLSCREEN;
  myscreen = SDL_SetVideoMode(newX, newY, 16, flags);

  if(!myscreen) {
    cerr << "Resize Failed, Bailing out." << endl;
    exit(-1);
  }

  switch(myscreen->format->BytesPerPixel) {
  case 1:
    pixelDriver = g_setpixelB1;
    break;
  case 2:
    pixelDriver = g_setpixelB2;
    break;
  case 3:
    pixelDriver = g_setpixelB3;
    break;
  case 4:
      pixelDriver = g_setpixelB4;
      break;
  default:
    cerr << "Unsupported pixel depth!" << endl;
    exit(-1);
  } 
}


// update physical screen from virtualScn
void Ui::updateScreen()
{
    SDL_Flip(myscreen);
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
