// Configuration for SDL Sasteroids...
// I've created this with the hopes of making it a bit easier on the
// portable guys to port this. (Although the code is fairly bloated. :/)

// In general, to disable a feature/option just comment it out, changing
// the "TRUE" to "FALSE" WILL NOT WORK.


// Screen Resolutions
// TODO: Make this configurable in game
#define SCREEN_X	640
#define SCREEN_Y	400

// Double Size 320x200 displays?
#define DOUBLE_SIZE	TRUE

// Use backdrops?
#define USE_BACKDROP	TRUE

// Use non original enhanced behavoir?
#define NON_ORIGINAL TRUE

// Cheatcodes?
#define CHEAT_CODES	TRUE

///////////////////////////////////////////////////////
// Don't Edit bellow this line for standard configuration

#ifdef DOUBLE_SIZE
	#define PLAY_X (SCREEN_X/2)
	#define PLAY_Y	(SCREEN_Y/2)
#else
	#define PLAY_X SCREEN_X
	#define PLAY_Y	SCREEN_Y
#endif
