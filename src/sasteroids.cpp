// Some code derived from xasteroids Copyright Phil Goetz.
// See the file README.xast, which is the README file from the xasteroids
// distribution.
// 
// All other code Copyright 1994 Brad Pitzel pitzel@cs.sfu.ca
// Feel free to use/distribute/modify as long as acknowledgement to
// sasteroids author(s) is made.
// 
// 
// SDL Sasteroids Code by Andrew M. (aka Merlin262)

#include "sasteroids.h"


/////////////////////////////////////////////////////////////////////////////
// Function Declarations:
void displayScreen(char *screen);
void upscore(int upby);


/////////////////////////////////////////////////////////////////////////////
// Global variables: 

// System Setup
SBitmap Gbit[NUM_BITMAPS];                // graphics. =)
SBitmap Backdrops[NUM_BACKS];

SBitmap titleScreen;
SBitmap extraLives;                       // 1/2 scale ship for extra men.

#ifdef HAVE_SOUND
Mix_Chunk *soundSamples[NUM_SOUNDS];               // Sound!
#endif


int use_joystick, num_joysticks;

#ifdef HAVE_JOYSTICK
SDL_Joystick *js;
void SDL_JoystickUpdate();
#endif


int G_use_backdrop = 0;

// Global Game Play Variables.
Player PlayerShip;	                  // Info about player's ship.
int score, Glevel, numasts, oldscore;     // scoring and level info
int ClassicMode = 0;                      // classic mode?
int BackdropOn = 1, wantFullScreen = 0;   // is the backdrop on?

// Power Ups, etc...
int deathTimer;                           // Mwahahahahahahh

// These do something.... :D
int eeggU = 0, eeggD = 0, eeggL = 0, eeggR = 0;
int eeggS = 1;

// High Score List
char HiScoreStrings[10][10];
int HiScoreNumbers[10];

// Store the current volume!
int MainVolume = 90;
int DispVolume = 0;


///////////////////////////////////////////////////////////
// Initialize HiScores list - TODO: Load Hi Score List
void InitializeHiScores()
{
  int i;
  for(i = 0; i < 10; i++) {
    strcpy(HiScoreStrings[i], "");
    HiScoreNumbers[i] = 0;
  }
}


//////////////////////////////////////////////////////////
// Save Hi Score list - TODO: Save Hi Score List
void SaveHiScores()
{
  // TODO: Save hiscore list.
}


//////////////////////////////////////////////////////////
// level odds - return an integer based on a level
int LevelOdds(int lvlMax, int maxChance, int lvlStep)
{
  int lvlDifference;
  lvlDifference = lvlMax - Glevel;
  lvlDifference = (lvlDifference-1)*lvlStep;
  if(lvlDifference < maxChance) lvlDifference = maxChance;
  if(lvlDifference < 1) lvlDifference = 1;
  return lvlDifference;
}


/////////////////////////////////////////////////////////////////////////
// play sound function (just to help with the #ifdef's not being everywhere
//
void PlaySound(int soundNumber)
{
#ifdef HAVE_SOUND
  if(soundSamples[soundNumber])
    Mix_PlayChannel(-1, soundSamples[soundNumber], 0);
#endif
}


//////////////////////////////////////////////////////////////
// upscore - from xasteroids
void upscore(int up)
{
    score = score + up;

    if (oldscore + 60000 < score) {
	PlayerShip.addship();	
	oldscore = score;
	PlaySound(SND_POWERUP);
    }
}




/////////////////////////////////////////////////////////////////////////////
// sound display - displays the current mixer level
int sDisplayTimer = 0;


void sounddisplay() 
{
  int x, x1 = 24;
  int y = 36;

  if(sDisplayTimer > 0) sDisplayTimer--;
  if(sDisplayTimer < 0) sDisplayTimer = 0;
  if(!sDisplayTimer) return;

  for( x = 0; x < MainVolume; x++ ) {
    Ui::setpixel(x1+x, y, 255-x, (55)+x, 0);
    Ui::setpixel(x1+x, y+1, 255-x, 55+x, 0);
    Ui::setpixel(x1+x, y+2, 255-x, 55+x, 0);
    Ui::setpixel(x1+x, y+3, 255-x, 55+x, 0);
    Ui::setpixel(x1+x, y+4, 255-x, 55+x, 0);
  } 

}









////////////////////////////////////////////////////////////////////////////
// botline - modified from xasteroids
void botline()
{
    int y, x, x1;

    IntegerDisplay::display_integer(score, 10.0f, float(Ui::HEIGHT()) - 2.0f);
    IntegerDisplay::display_integer(Glevel, Ui::WIDTH()- 40.0f, 20.0f);

    y = Ui::HEIGHT() - 20;
    x1 = Ui::WIDTH() - 205;
    if(!ClassicMode) { 
      // Draw weapon + shield energy meter.
      Gbit[BULLET].put(x1-12, (y));
      for( x = 0; x < (PlayerShip.weaponPercent()*2); x++ ) {
	Ui::setpixel(x1+x, y, 255-x, (55)+x, 0);
	Ui::setpixel(x1+x, y+1, 255-x, 55+x, 0);
	Ui::setpixel(x1+x, y+2, 255-x, 55+x, 0);
	Ui::setpixel(x1+x, y+3, 255-x, 55+x, 0);
	Ui::setpixel(x1+x, y+4, 255-x, 55+x, 0);
      } 
      
      // Draw shield energy meter.
      y += 10;
    }

    Gbit[BULLET].put(x1-12, y);
    for( x = 0; x < (PlayerShip.shieldPercent()*2); x++ ) {
      Ui::setpixel(x1+(x), y, 255-x, (55)+x, 0);
      Ui::setpixel(x1+(x), y+1, 255-x, 55+x, 0);
      Ui::setpixel(x1+(x), y+2, 255-x, 55+x, 0);
      Ui::setpixel(x1+(x), y+3, 255-x, 55+x, 0);
      Ui::setpixel(x1+(x), y+4, 255-x, 55+x, 0);
    } 
    
    y = (Ui::HEIGHT() - 24);
    for( x = 0; x < (PlayerShip.ships()-1); x++) {
      extraLives.put(16+x*20, 15.0f, 0.5f);
    }
}


//////////////////////////////////////////////////////////////////////////
void displayScreen(char *file)
{
  SBitmap backdrop(file);
  Ui::clearscreen();
  backdrop.putA(0, Ui::HEIGHT());
}



// Timer Tick just nicely pushes back a user event onto the stack.
// This might be an issue if we're on slow machines that can't
// respond to this event faster than the timer
// TODO: BugFix this..
static int sastWantTicks = 0;
static int FinishedLastCall = 0;
char globalMessage[256];
int gMsgTimeLeft = 0;

Uint32 TimerTick(Uint32 interval, void* param)
{
  SDL_Event myEvent;
  
  if(sastWantTicks && FinishedLastCall) {
    FinishedLastCall = 0;
    myEvent.type = SDL_USEREVENT;
    SDL_PushEvent(&myEvent);
  } else if(!FinishedLastCall) {
#ifdef PRINT_DIAG
    cerr << "[Diag] Dropped Frame!" << endl;
#endif
  }

  return GAME_CLOCK;
}


void MakeGlobalMessage(char* Tstring)
{
  strcpy(globalMessage, Tstring);
  gMsgTimeLeft = Ui::HEIGHT();
}


/////////////////////////////////////////////////////////////////////////////
// play a game..
void PlayGame()
{
  char pstr[256];
  int pause = 0;
  int dead = 0;
  int finalDead = 0;
  int aegg = 0;
  int chOn;
  int shipInvasion = 0;
  float alpha_anim = -1.0f;
  SDL_Event event;
  SDL_TimerID timer;
  
  Uint8 *keystatebuffer;
  
  char GameOver = 0, cReq, flashShip = 0;
  
  // get starting skill level, <DELETED>
  Glevel = 0;
  
  numasts = 0;
  score = 0;
  oldscore = 0;
  dead = 0;
  
  PlayerShip.setships(3);
  score = 0;
  oldscore = -1;

  PlayerShip.Reset();
  FreeObjArray();
  Ui::clearscreen();

  timer = SDL_AddTimer( GAME_CLOCK, TimerTick, 0 );
  
  if(!timer) {
    cout << "Fatal: Couldn't Create Timer!" << endl;
    exit(-1);
  }
  
  keystatebuffer = SDL_GetKeyState(NULL);
 
  
  pause = 0;
  GameOver = 0;
  cReq = 0;
  sastWantTicks = 1;
  finalDead = 0;
  FinishedLastCall = 1;
  deathTimer = -1;
  gMsgTimeLeft = 0;
  chOn = 1;
  shipInvasion = 0;
  
#ifdef HAVE_SOUND
  Mix_ExpireChannel(0, 0);
#endif  

  while (!GameOver) {
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_VIDEORESIZE:
	ScreenLimits.SetXY((event.resize.w), 
			   (event.resize.h));
	Ui::resync(event.resize.w, event.resize.h);
	break;
	
      case SDL_KEYDOWN:
	if(finalDead && !(finalDead > 2)) {
	  sastWantTicks = 0;
	  GameOver = 1;
	}

	if (event.key.keysym.sym == SDLK_LALT && !pause && !dead) {
	  PlaySound(SND_WARP);
	  PlayerShip.Hyper();
	}
	
	if (event.key.keysym.sym == SDLK_RALT && !pause && !dead) {
	  PlaySound(SND_WARP);
	  PlayerShip.Hyper();
	}

	// I hate cheaters... Do something fun with the old cheat code. :D
	if (event.key.keysym.sym == SDLK_o && 
	    (keystatebuffer[SDLK_LSHIFT] || keystatebuffer[SDLK_RSHIFT])  &&
	    !pause) {
	  deathTimer = 3; 
	  MakeGlobalMessage("CHEATER!!! C H E A T E R!!! CHEATER!!!");
	}

	// Leave this cheat code in, nice for debugging.
	if (event.key.keysym.sym == SDLK_n && !pause) { Glevel++; alpha_anim = 1.0f; }
	
	// I prefer space
	if (event.key.keysym.sym == SDLK_SPACE && !pause)
	  PlayerShip.Fire();

	// but CTRL is the default
	if (event.key.keysym.sym == SDLK_LCTRL && !pause)
	  PlayerShip.Fire();
	
	if (event.key.keysym.sym == SDLK_f)
	  Ui::FullScreen();
	
	if (event.key.keysym.sym == SDLK_y) {
	  if (cReq) {
	    sastWantTicks = 0;
	    pause = 1;
	    GameOver = 1;
	  }
	}
	
	if (event.key.keysym.sym == SDLK_p && !dead) {
	  strcpy(pstr, "Press any key to continue.");
	  if (!pause) {
	    sastWantTicks = 0;
	    pause = 1;
	  } else { 
	    pause = 0;
	    sastWantTicks = 1;
	  }
	} else {
	  pause = 0;
	  if (!GameOver)
	    cReq = 0;
	  sastWantTicks = 1;
	}
	
	if (event.key.keysym.sym == SDLK_q && !dead && !pause) {
	  if (!pause) {
	    sastWantTicks = 0;
	    pause = 2;
	    cReq = 1;
	    strcpy(pstr,
		   "Press any 'Y' to quit, any other key to continue.");
	  }
	}

	if(event.key.keysym.sym == SDLK_s && !finalDead) {
	  if(dead) {
	    PlayerShip.Reset();
	    dead = 0;
	  }
	}

	if(event.key.keysym.sym == SDLK_PLUS || event.key.keysym.sym == SDLK_EQUALS) {
	  if(MainVolume < 128) MainVolume+=8;
	  if(MainVolume > 128) MainVolume=128;
	  DispVolume = 64;
#ifdef HAVE_SOUND
	  Mix_Volume( -1, MainVolume);
	  sDisplayTimer = 35;
#endif
	}

	if(event.key.keysym.sym == SDLK_MINUS) {
	  if(MainVolume > 0) MainVolume-=8;
	  if(MainVolume < 0) MainVolume=0;
	  DispVolume = 64;
#ifdef HAVE_SOUND
	  Mix_Volume( -1, MainVolume );
	  sDisplayTimer = 35;
#endif	 
	}

#ifdef SAS_EXTRAS	
	if(!dead && !pause) {
	  if(event.key.keysym.sym == SDLK_UP) {
	    if(eeggU != 1 || eeggS != 2) {
	      eeggU = 0; eeggD = 0; eeggL = 0; eeggR = 0; eeggS = 1;
	    }
	    eeggU |= eeggS;
	    eeggS = eeggS << 1;
	  }

	  if(event.key.keysym.sym == SDLK_DOWN) {
	    eeggD |= eeggS; eeggS = eeggS << 1;
	  }
	  
	  if(event.key.keysym.sym == SDLK_LEFT) {
	    eeggL |= eeggS; eeggS = eeggS << 1;
	  }

	  if(event.key.keysym.sym == SDLK_RIGHT) {
	    eeggR |= eeggS; eeggS = eeggS << 1;
	  }

	  if(eeggS > 512) {
	    eeggS = 1; eeggU = 0; eeggD = 0; eeggL = 0; eeggR = 0;
	  }
	}
#endif
	

	break;
      case SDL_ACTIVEEVENT:
	;
	break;

      case SDL_QUIT:
	exit(0);
	break;
      

      case SDL_USEREVENT:	      
	if(gMsgTimeLeft > 0) gMsgTimeLeft-=5;
	if(gMsgTimeLeft < 0) gMsgTimeLeft = 0;

	// This only gets called while the GAME CLOCK ITSELF is ticking =)
	deathTimer--;
	if(deathTimer < -128) deathTimer = -1;
	if(finalDead && finalDead > 2) finalDead--;

	if (keystatebuffer[SDLK_RIGHT])
	  PlayerShip.rotateRight();
	
	if (keystatebuffer[SDLK_LEFT])
	  PlayerShip.rotateLeft();
	
	if (keystatebuffer[SDLK_UP] && !dead) {
	  PlayerShip.Thrust(0.15f);
#ifdef HAVE_SOUND
	  if(!chOn) {
	    Mix_PlayChannel(0, soundSamples[SND_ENGINE], -1);
	    chOn = 1;
	  } 
#endif
	} else {
#ifdef HAVE_SOUND
	  Mix_ExpireChannel(0, 10);
	  chOn = 0;
#endif
	}

	if (keystatebuffer[SDLK_DOWN])
	  PlayerShip.shieldOn();
	else
	  PlayerShip.shieldOff();
	
	if (keystatebuffer[SDLK_b])
	  PlayerShip.Brake();

	
	
	if (numasts <= 0) {
	  char tempString[256];
	  Glevel++;
	  sprintf(tempString, "Entering Level %d", Glevel);
	  MakeGlobalMessage(tempString);
	  numasts = 0;
	  GenerateAsteroids();
	  PlayerShip.addMaxPower(2);
	  PlayerShip.addRegPower(1);
	  if(Glevel != 1) alpha_anim = 1.0f;
	}

	
	if(!(rand()%LevelOdds(32, 4050, 1)) || (shipInvasion && !(rand()%50))) {
	
	  if(Glevel > 6 && !(rand()%15) && !shipInvasion) shipInvasion = 10;
	  if(shipInvasion) shipInvasion--;
	  if(shipInvasion < 0) shipInvasion = 0;
	  int j;
	  j = GetOpenObject();
	  ObjectList[j] = new Enemy;
	  PlaySound(SND_ENEMY);
	}
	

	if (!(rand()%250) && !ClassicMode) {
	  int j;
	  j = GetOpenObject();
	  ObjectList[j] = new PowerUp;
	}

	
	dead += MoveObjects();
	if(deathTimer == 0) { dead = 1; deathTimer = -1; aegg = 1; }

	if(dead == 1) {
	  PlaySound(SND_BOOM_C);
	  int j;
	  strcpy(pstr, "Press s to start");
	  PlayerShip.death();
	  PlayerShip.SetDeadStick(1);
	  if(PlayerShip.ships() <= 0) { 
	    finalDead = 20;
	    sastWantTicks = 1;
	  }    
	  
	  j = GetOpenObject();
	  ObjectList[j] = new Explosion( PlayerShip.GetX(),
					 PlayerShip.GetY(),
					 PlayerShip.VelX(),
					 PlayerShip.VelY());
	  dead++;
	  PlayerShip.Reset();
	  shipInvasion = 0;
	  PlayerShip.SetDeadStick(1);
	} else if(dead > 1) { 
	  strcpy(pstr, "Press s to start");
	  dead = 2;
	}
	
	FinishedLastCall = 1;
      }
      
      Ui::predraw();
      if(BackdropOn && Ui::WIDTH() <= 640 &&
	 Ui::HEIGHT() <= 400) {
	Backdrops[Glevel%NUM_BACKS].putA(0, Ui::HEIGHT());
#ifdef WANT_OPENGL
	if(alpha_anim > 0.0f) {
	  Backdrops[(Glevel-1)%NUM_BACKS].putA(0, Ui::HEIGHT(), 0.0f, alpha_anim);
	  alpha_anim -= 0.05f;
	}
#endif
      } else {
	Ui::clearscreen();
      }
      DrawObjects();
      botline();

      if(dead) {
	int x, y, width, height;

	width = Gbit[BIT_PLAYER].width(); 
	height = Gbit[BIT_PLAYER].height();
	x = int(ScreenLimits.GetX()) - width;
	y = int(ScreenLimits.GetY()) - height;
	x = x / 2;
	y = y / 2;
	
	if(flashShip > 24 || flashShip < 0) flashShip = 0;
	flashShip++;
	
	if(flashShip >  12 && !finalDead) {
	  Gbit[BIT_PLAYER].put(x, y);
	}
	
	if(finalDead) {
	  strcpy(pstr, "Game Over - Hit a key to return to the menu.");
	  Ui::CenterXText(180-finalDead*10, pstr);
	} else {
	  Ui::CenterXText(180, pstr);
	}

	if(aegg && finalDead) {
	  strcpy(pstr, "Strange, the only way to win is to not play.");
	  Ui::CenterXText(140, pstr);
	}
      }
      
      if(gMsgTimeLeft > 0) {
	Ui::CenterXText(gMsgTimeLeft, globalMessage);
      }

      if(!sastWantTicks) {
	Ui::CenterText(pstr);
	FinishedLastCall = 1;
      }
      
      sounddisplay();
      Ui::updateScreen();
    }
  }

#ifdef HAVE_SOUND
  Mix_ExpireChannel(0, 0); // Always kill jet sound.
#endif

  SDL_RemoveTimer(timer);
}


////////////////////////////////////////////////////////////////////////////
// return 1=easy, 4=medium, 8=hard
int selectGame()
{
  // Ui::drawToPhysical();
    displayScreen("back.raw");
    // TODO: skill level selection...
    return 1;
}


// //////////////////////////////////////////////////////////////////////////
void ShowInfo()
{

}




/////////////////////////////////////////////////////////////////////////////
void showScoringInfo()
{
    displayScreen(BINDIR "/graphics/back.bmp");	// TODO, rewrite this screen
}


/////////////////////////////////////////////////////////
// Initialize the System 
void InitializeSDL()
{
  // Init SDL Video: 
  {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      cerr << "Couldn't initialize video!" << endl;
      exit(-1);
    }
    atexit (SDL_Quit);
  }

  // Init SDL Audio: 
  { 
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
      cerr << "Couldn't initialize audio!" << endl;
      exit(-1);
    }
    
    atexit (SDL_Quit);
  }
  
  // Init SDL Timer: 
  { 
    if (SDL_Init(SDL_INIT_TIMER) < 0) {
      cerr << "Could'nt initialize timer!" << endl;
      exit(-1);
    }
    
    atexit (SDL_Quit);
  }

  // Init joystick:
  {
    use_joystick = 0;
    num_joysticks = 0;
    
#ifdef HAVE_JOYSTICK
    use_joystick = 1; 
    
    if (SDL_Init(SDL_INIT_JOYSTICK) < 0) {
      cerr << "Couldn't initialize joystick." << endl;
      
      use_joystick = 0;
    }
    else
      {
	/* Look for joysticks: */
	
	num_joysticks = SDL_NumJoysticks();
	
	if (num_joysticks <= 0) {
	  cerr << "No joysticks available." << endl;
	  
	  use_joystick = 0;
	}
	else
	  {
	    /* Open joystick: */
	    
	    js = SDL_JoystickOpen(0);
	    
	    if (js == NULL) {
	      cerr << "Couldn't open joystick 1." << endl;
	      
	      use_joystick = 0;
	    }
	  }
      }
#endif
  }

  atexit(SDL_Quit);
}


/////////////////////////////////////////////////////////
// Handle Command Line Parameters
void HandleCommandLine(int argc, char** argv)
{
  int i;

  for( i = 0; i < argc; i++) {
    if(argv[i]) {
      if(strcmp(argv[i], "-fullscreen")==0) wantFullScreen = 1;
      if(strcmp(argv[i], "-classic")==0) ClassicMode = 1;
    }
  }
}


///////////////////////////////////////////////////////////
// Actions for the main menu

void MainMenuQuit(int* done) 
{
  *done = 1;
}


void MainMenuPlay(int* done) 
{
  PlayGame();
}


void MainMenuInformation(int* done)
{
  char *tStringList[] =
    { "SDL Sasteroids Version " VERSION,
      " ",
      " LEFT ARROW Turn ship Left",
      " RIGHT ARRW Turn ship Right",
      " SPACE Fire",
      " UP ARROW Thrust ",
      " DOWN ARROW Shield",
      " LEFT ALT Hyperspace",
      " F Toggle Full Screen",
      " P Pause",
      " -/+ Volume Down/Up",
      " Q - Quit",
      " ",
      "Return to Game",
      NULL};

  SBitmap mouse(GAMEDIR "/graphics/mouse.png");
  GraphicsMenu MainMenu(&mouse, &titleScreen, tStringList);
  
  /* Run the Main Menu */
  
  MainMenu.setAction(13, MainMenuQuit);
  
  MainMenu.enableSound(SND_FIRE);
  MainMenu.setTopPad(26.0f);
  MainMenu.setMinSelectable(13);
  MainMenu.RunMenu();
}


void MainMenuCredits(int *done)
{
  char *tStringList[] =
    {
      "SDL Sasteroids Version " VERSION,     // 0
      " ",                                   // 1
      " ",                                   // 2
      "(C)opyright 1991 - 2003",             // 3
      "Contributers:",                       // 4
      "Andrew McCall, Brad Pitzel, Digisin", // 5
      " ",                                   // 6
      "Backdrops:",                          // 7
      "From Nasa Image Collection",          // 8
      " ",                                   // 9
      "Return to Game",                      // 10
      NULL
    };

  SBitmap mouse(GAMEDIR "/graphics/mouse.png");
  GraphicsMenu MainMenu(&mouse, &titleScreen, tStringList);
  
  MainMenu.setAction(10, MainMenuQuit);

  MainMenu.enableSound(SND_FIRE);
  MainMenu.setTopPad(26.0f);
  MainMenu.setMinSelectable(10);
  MainMenu.RunMenu();
}


/////////////////////////////////////////////////////////
// Main Program Starts Here 
// Starts all the tasks, etc...
int main(int argc, char *argv[])
{
  char *tStringList[] =
    { "SDL Sasteroids Version " VERSION,
      "INFORMATION",
      "CREDITS",
      "START GAME",
      "QUIT",
      NULL
    };
  
  srand((unsigned int) time(NULL));

  
  InitializeSDL();
  HandleCommandLine(argc, argv);
  InitializeHiScores();


#ifdef HAVE_SOUND
  if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024)==-1) {
    throw "Couldn't initialize sound";
  }

  Mix_AllocateChannels(16);
  Mix_ReserveChannels(1);  // for jet engine!
  Mix_Volume( -1, MainVolume);
#endif

  FastMath::init(1); 
  Ui::init();
  SetupObjArray();

  LoadBitmaps();
  LoadWavs();
  IntegerDisplay::initialize();

  SBitmap mouse(GAMEDIR "/graphics/mouse.png");
  GraphicsMenu MainMenu(&mouse, &titleScreen, tStringList);

  /* Run the Main Menu */

  MainMenu.setAction(1, MainMenuInformation);
  MainMenu.setAction(2, MainMenuCredits);
  MainMenu.setAction(3, MainMenuPlay);
  MainMenu.setAction(4, MainMenuQuit);
  
  MainMenu.enableSound(SND_FIRE);
  MainMenu.setTopPad(26.0f);
  MainMenu.setMinSelectable(1);
  MainMenu.RunMenu();

#ifdef HAVE_SOUND
  Mix_CloseAudio();
#endif 

#ifdef HAVE_JOYSTICK
  SDL_JoystickClose(0);
#endif

  // shutdown & restore text mode
  Ui::restore();
 
  return 0;
}
