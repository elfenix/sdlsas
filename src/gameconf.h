// Configuration for SDL Sasteroids...
// I've created this with the hopes of making it a bit easier on the
// portable guys to port this. (Although the code is fairly bloated. :/)

// In general, to disable a feature/option just comment it out.

// Information about the Game

// Set the version string for SDL Sasteroids
#define VERSION "SDL Sasteroids 1.96.2(CVS)"

// Where do we want the game files?
#ifndef BINDIR
#define BINDIR "/usr/games/"
#endif

// Uncomment line bellow if your using an old C++ compiler
// #define _OLD_CPP_HEADERS

// Screen Resolutions
// TODO: Make this configurable in game
#define SCREEN_X	640
#define SCREEN_Y	400

// Double Size 320x200 displays?
#define DOUBLE_SIZE	

// Use backdrops?
#define USE_BACKDROP	

// Use non original enhanced behavoir?
#define NON_ORIGINAL 

// Cheatcodes?
#define CHEAT_CODES	

///////////////////////////////////////////////////////
// Don't Edit bellow this line for standard configuration

#ifdef DOUBLE_SIZE
	#define PLAY_X (SCREEN_X/2)
	#define PLAY_Y	(SCREEN_Y/2)
#else
	#define PLAY_X SCREEN_X
	#define PLAY_Y	SCREEN_Y
#endif
