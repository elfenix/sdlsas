// Copyright 1994 Brad Pitzel
// 
// Feel free to use/distribute/modify as long as credit/copyrights for
// myself 
// are included.

// File : Ui.c[1.0]
// Name : gamelib1.0
// Status : proposed
// Changed: Sun Jun 12 22:18:16 1994

// a layer on top of the svgalib library.
// Routines to setup the initialize the screen/svgalib/etc, restore back
// to text when all done, throw up a few messages/prompts, get simple
// string input from the user, etc..



#include "sasteroids.h"

SDL_Surface *Ui::myscreen = 0;
TTF_Font *Ui::myfont = 0;
void (*Ui::pixelDriver)(SDL_Surface* visual, int x, int y, 
			char r, char g, char b);


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

    surface = TTF_RenderText_Blended(myfont, msg, mycolor);
    if (!surface) {
	cerr << "Couldn't render font!" << endl;
	return;
    }

    SDL_BlitSurface(surface, NULL, myscreen, &dest);
    SDL_FreeSurface(surface);
}



// set drawing to virtualScn screen
void Ui::drawToVirtual()
{
    SDL_Flip(myscreen);
}

void Ui::resync(int newX, int newY)
{
  myscreen = SDL_SetVideoMode(newX, newY, 16, 
				SDL_HWSURFACE | SDL_ANYFORMAT | 
				SDL_RESIZABLE);

  if(!myscreen) {
    cerr << "Resize Failed, Bailing out." << endl;
  }
}


// Initialize SDL Library and User Interface 
void Ui::init()
{
    myscreen = SDL_SetVideoMode(SCREEN_X, SCREEN_Y, 16, 
				SDL_HWSURFACE | SDL_ANYFORMAT | 
				SDL_RESIZABLE);
    if (!myscreen) {
	cerr << "Couldn't Grab a Visual: ERROR!" << endl;
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


    SDL_WM_SetCaption("SDL Sasteroids", "SDL Sasteroids");
    SDL_ShowCursor(0);

    if (TTF_Init() < 0) {
	fprintf(stderr, "Coudln't initialize TTF library!\n");
	exit(-1);
    }
    atexit(TTF_Quit);

    myfont = TTF_OpenFont(BINDIR "Wargames.ttf", DMULTCONST(9));
    if (!myfont) {
	fprintf(stderr, "Couldn't grab TTF font!\n");
	exit(-1);
    }
    TTF_SetFontStyle(myfont, TTF_STYLE_NORMAL);
}


// Kill the SDL library
void Ui::die(int x)
{
    cout << "You shouldn't see this!" << endl;
}

void Ui::restore()
{
    TTF_CloseFont(myfont);
}


void Ui::message(int y, char *msg)
{
    ShowText(0, y, msg);
}


// prompt user, return 1=yes, 0=no
char Ui::yesNo(const char *msg)
{
    // No longer used.
    return 0;
}


// get text input from user
// len must be 1 less than msg buffer to allow for 0 at end
int Ui::input(int x, int y, char *msg, int len)
{
    // No longer used.
    return 0;
}


// for 'input', above.  Show a simple non-blinking cursor
inline void Ui::drawCursor(int x, int y)
{
    // No longer used.
}

inline void Ui::hideCursor(int x, int y)
{
    // No longer used.
}


// update physical screen from virtualScn
void Ui::updateScreen()
{
    SDL_Flip(myscreen);
}

// set drawing to physical screen
void Ui::drawToPhysical()
{
    SDL_Flip(myscreen);
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


void g_setpixelB2(SDL_Surface* visual, int x, int y, char r, char g, char b)
{
  Uint32 color;
  Uint8 *ubuff8;
  Uint16 *ubuff16;

  color = SDL_MapRGB(visual->format, r, g, b);
  ubuff8 = (Uint8 *) visual->pixels;
  ubuff8 +=
    (y * visual->pitch) + (x * visual->format->BytesPerPixel);
  ubuff16 = (Uint16 *) ubuff8;
  *ubuff16 = (Uint16) color;
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


void g_setpixelB4(SDL_Surface* visual, int x, int y, char r, char g, char b)
{
  Uint32 color;
  Uint32 *ubuff32;

  color = SDL_MapRGB(visual->format, r, g, b);
  ubuff32 = (Uint32 *) visual->pixels;
  ubuff32 += ((y * visual->pitch) >> 2) + x;
  *ubuff32 = color;
}
