// Copyright 1994 Brad Pitzel
//
// Feel free to use/distribute/modify as long as credit/copyrights for myself 
// are included.

// File   : Ui.h[1.0]
// Changed: FRI OCT-10-2002

// a layer on top of the svgalib library.
// Routines to setup the initialize the screen/svgalib/etc, restore back
// to text when all done, throw up a few messages/prompts, get simple
// string input from the user, etc..

#ifndef __Ui__
#define __Ui__

#include "sasteroids.h"

#include <GL/gl.h>
#include <GL/glu.h>

// call before using setpixel or getpixel. =)
void GraphicsStartDraw( SDL_Surface* visual );
void GraphicsStopDraw( SDL_Surface* visual );

// Other integers
extern int wantFullScreen;
class ScreenBitmap;

class UserInterfaceManager
{
public:
    static void init();
    static void restore();
    static void CenterText( const char* msg );
    static void CenterXText( int y, const char* msg );
    static void ShowText( int x, int y, const char* msg );
    static void ShowTextColor( int x, int y, const char* msg, char r, char g,
            char b );
    static SDL_Surface* get_text( const char* msg, char r, char g, char b );
    static void resync( int x, int y );
    static void updateScreen();
    static void predraw();
    static void display_integer( int num, float x, float y );

    //static void message( int y, const char *msg ); // message centered at line y
    //static void message( const char *msg ) { message(0,msg); }

    static void CenterText( const std::string& p_str )
    {
        CenterText( p_str.c_str() );
    }

    static void CenterXText( int y, const std::string& p_str )
    {
        CenterXText( y, p_str.c_str() );
    }

    //static char yesNo( const char *msg );

//  static int fontHeight()
    //{
//      return TTF_FontHeight(myfont);
    //}

    //static int fontWidth() { return 19; } // Fudged, shouldn't be used...

    static int WIDTH()
    {
        if( myscreen )
            return myscreen->w;
        return 0;
    }
    static int HEIGHT()
    {
        if( myscreen )
            return myscreen->h;
        return 0;
    }

    static void clearscreen()
    {
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    }
    ;

    static void FullScreen()
    {
        SDL_WM_ToggleFullScreen( myscreen );
    }
    ;

    friend class ScreenBitmap;
    friend class Bitmap;

    static SDL_Surface* myscreen;
    static ScreenBitmap* numbers;

private:
    static TTF_Font *myfont;
    static void drawCursor( int x, int y );
    static void hideCursor( int x, int y );

};

#endif
