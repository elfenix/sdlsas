// Configuration for SDL Sasteroids...
// I've created this with the hopes of making it a bit easier on the
// portable guys to port this. (Although the code is fairly bloated. :/)

// In general, to disable a feature/option just comment it out.

// Information about the Game

// Set the version string for SDL Sasteroids
#define VERSION "2.00"

// Where do we want the game files?
// Note: This is NOT the place to edit this, use the *MAKEFILE*
#ifndef GAMEDIR
#define BINDIR "/usr/share/sasteroids/"
#else
#define BINDIR GAMEDIR
#endif

// Uncomment line bellow if your using an old C++ compiler
// #define _OLD_CPP_HEADERS

// Use Sound?
#define HAVE_SOUND

// Double Size 320x200 displays?
#define DOUBLE_SIZE	

// Use non original enhanced behavoir?
#define NON_ORIGINAL 

// If the game runs slow this might help speed it up,
// turns off font Anti Aliasing
#define QUICK_FONTS

// Cheatcodes?
#define CHEAT_CODES	

// Print some diagnostics here and there(not recommended)
// #define PRINT_DIAG

// Extras that you might want to turn off on slower systems
#define SAS_EXTRAS

// Run in fullscreen mode by default? (Comment to disable)
// #define WANT_FULLSCREEN SDL_FULLSCREEN

// Resizable by default? (Uncomment/Comment to enable/disable)
// Only set as resizable if we aren't resizable.
// #define WANT_RESIZE SDL_RESIZABLE


#ifndef WANT_FULLSCREEN
#define WANT_FULLSCREEN 0 
#else
#ifdef WANT_RESIZE
#undef WANT_RESIZE
#endif
#endif

#ifndef WANT_RESIZE
#define WANT_RESIZE 0
#endif

///////////////////////////////////////////////////////
// Don't Edit bellow this line for standard configuration

#ifdef DOUBLE_SIZE
#define SCREEN_X 640
#define SCREEN_Y 400
#define PLAY_X (SCREEN_X/2)
#define PLAY_Y (SCREEN_Y/2)
#define DMULTCONST(a) ((a)*2)
#define DDIVCONST(a) ((a)/2)
#define DDIV2CONST(a) ((a))
   
#else

#define SCREEN_X 320
#define SCREEN_Y 200
#define PLAY_X SCREEN_X
#define PLAY_Y	SCREEN_Y
#define DMULTCONST(a) (a)
#define DDIVCONST(a) (a)
#define DDIV2CONST(a) ((a)/2)

#endif

// Where do we want the game files?
// Note: This is NOT the place to edit this, use the *MAKEFILE*
#ifndef BINDIR
#define BINDIR "/usr/share/sasteroids/"
#endif
