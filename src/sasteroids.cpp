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
SBitmap Gbackdrop;
SBitmap titleScreen;
SBitmap extraLives;                       // 1/2 scale ship for extra men.

#ifdef HAVE_SOUND
Mix_Chunk *soundSamples[8];               // Sound!
#endif

int G_use_backdrop = 0;

// Global Game Play Variables.
Ship PlayerShip;	                  // Info about player's ship.
int score, Glevel, numasts, oldscore;     // scoring and level info
int ClassicMode = 0;                      // classic mode?
int BackdropOn = 1, wantFullScreen = 0;   // is the backdrop on?

// Power Ups, etc...
int canShootThree = 1;                    // Has the three shooter powerup?
int smartZapper;                          // smart teleport
int deathTimer;                           // Mwahahahahahahh

// These do something.... :D
int eeggU = 0, eeggD = 0, eeggL = 0, eeggR = 0;
int eeggS = 1;

// High Score List
char HiScoreStrings[10][10];
int HiScoreNumbers[10];


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

    if (oldscore + 10000 < score) {
	PlayerShip.addship();	// TODO: Play sound FX
	oldscore = score;
    }
}


////////////////////////////////////////////////////////////////////////////
// fire a bullet for player ship
void Fire()
{
 int bullet;
  float fcos, fsin;
  float rx, ry;

  if(eeggL == 80 && eeggR == 160 && eeggU == 3 && eeggD == 12 ) {
    CleanUpStuff();
    eeggL = 0; eeggR = 0; eeggU = 0; eeggD = 0; eeggS = 1;
  }

  
  if (PlayerShip.weaponPower() > 10) {
    PlaySound(SND_FIRE);

    rx = PlayerShip.GetX() + (PlayerShip.GetWidth() / 2);
    ry = PlayerShip.GetY() + (PlayerShip.GetHeight() / 2);
     
    fsin = -FastMath::cos(PlayerShip.getAngle());
    fcos = FastMath::sin(PlayerShip.getAngle());

    // Give it a bit of a kick back... ehehehheeheh
    if(!ClassicMode) {
      PlayerShip.AddVel(-fcos / 10.0f, -fsin / 10.0f);
      PlayerShip.dischargeWeapon();
    }

    bullet = GetOpenObject();
    ObjectList[bullet] = new ScreenObject;
    ObjectList[bullet]->restore();
    fsin = -FastMath::cos(PlayerShip.getAngle());
    fcos = FastMath::sin(PlayerShip.getAngle());
    ObjectList[bullet]->SetXY(rx + PlayerShip.Size() * fcos,
				  ry + PlayerShip.Size() * fsin);
    ObjectList[bullet]->SetVel(PlayerShip.VelX() + BUL_SPEED * fcos,
			       PlayerShip.VelY() + BUL_SPEED * fsin);
    ObjectList[bullet]->SetAcc(0.0f, 0.0f);
    ObjectList[bullet]->SetBitmap(&Gbit[BULLET]);
    ObjectList[bullet]->SetWrapMoves(false);
    ObjectList[bullet]->SetMaxSpeed(255.0f);
    ObjectList[bullet]->settype(SHIP_BUL);     
 
    if(canShootThree && PlayerShip.weaponPower() > 20) {
      // Fire 2
      bullet = GetOpenObject();
      ObjectList[bullet] = new ScreenObject;
      ObjectList[bullet]->restore();
      fsin = -FastMath::cos(PlayerShip.getAngle()+5);
      fcos = FastMath::sin(PlayerShip.getAngle()+5);
      ObjectList[bullet]->SetXY(rx + PlayerShip.Size() * fcos,
				ry + PlayerShip.Size() * fsin);
      ObjectList[bullet]->SetVel(PlayerShip.VelX() + BUL_SPEED * fcos,
				 PlayerShip.VelY() + BUL_SPEED * fsin);
      ObjectList[bullet]->SetAcc(0.0f, 0.0f);
      ObjectList[bullet]->SetBitmap(&Gbit[BULLET]);
      ObjectList[bullet]->SetWrapMoves(false);
      ObjectList[bullet]->SetMaxSpeed(255.0f);
      ObjectList[bullet]->settype(SHIP_BUL);  
      
      // Fire 3
      bullet = GetOpenObject();
      ObjectList[bullet] = new ScreenObject;
      ObjectList[bullet]->restore();
      fsin = -FastMath::cos(PlayerShip.getAngle()-5);
      fcos = FastMath::sin(PlayerShip.getAngle()-5);
      ObjectList[bullet]->SetXY(rx + PlayerShip.Size() * fcos,
				ry + PlayerShip.Size() * fsin);
      ObjectList[bullet]->SetVel(PlayerShip.VelX() + BUL_SPEED * fcos,
				 PlayerShip.VelY() + BUL_SPEED * fsin);
      ObjectList[bullet]->SetAcc(0.0f, 0.0f);
      ObjectList[bullet]->SetBitmap(&Gbit[BULLET]);
      ObjectList[bullet]->SetWrapMoves(false);
      ObjectList[bullet]->SetMaxSpeed(255.0f);
      ObjectList[bullet]->settype(SHIP_BUL); 

      PlayerShip.dischargeWeapon(); // Discharg 2x as fast when shooting 3
    }
  } else {
    // TODO: Do something interesting.
  }
}


////////////////////////////////////////////////////////////////////////////
// botline - modified from xasteroids
void botline()
{
    char text[256] = { 0 };
    int y, x, x1;

    y = 10;
    x1 = Ui::WIDTH() - DDIV2CONST(205);
    sprintf(text, "Score:%7d", score);
    Ui::ShowText(DMULTCONST(5), DMULTCONST(5), text);

    if(!ClassicMode) { 
      // Draw weapon + shield energy meter.
      Gbit[BULLET].put(DDIVCONST(x1)-DMULTCONST(3), DDIVCONST(y));
      for( x = 0; x < (PlayerShip.weaponPercent()*2); x++ ) {
	Ui::setpixel(x1+DDIV2CONST(x), y, 255-x, (55)+x, 0);
	Ui::setpixel(x1+DDIV2CONST(x), y+1, 255-x, 55+x, 0);
	Ui::setpixel(x1+DDIV2CONST(x), y+2, 255-x, 55+x, 0);
	Ui::setpixel(x1+DDIV2CONST(x), y+3, 255-x, 55+x, 0);
	Ui::setpixel(x1+DDIV2CONST(x), y+4, 255-x, 55+x, 0);
      } 
      
      // Draw shield energy meter.
      y += 10;
    }

    Gbit[BULLET].put(DDIVCONST(x1)-DMULTCONST(3), DDIVCONST(y));
    for( x = 0; x < (PlayerShip.shieldPercent()*2); x++ ) {
      Ui::setpixel(x1+DDIV2CONST(x), y, 255-x, (55)+x, 0);
      Ui::setpixel(x1+DDIV2CONST(x), y+1, 255-x, 55+x, 0);
      Ui::setpixel(x1+DDIV2CONST(x), y+2, 255-x, 55+x, 0);
      Ui::setpixel(x1+DDIV2CONST(x), y+3, 255-x, 55+x, 0);
      Ui::setpixel(x1+DDIV2CONST(x), y+4, 255-x, 55+x, 0);
    } 
    
    y = DDIVCONST(Ui::HEIGHT() - DMULTCONST(12));
    for( x = 0; x < (PlayerShip.ships()-1); x++) {
      extraLives.put(2+x*10, y);
    }
}


//////////////////////////////////////////////////////////////////////////
void displayScreen(char *file)
{
  SBitmap backdrop(file);
  Ui::clearscreen();
  backdrop.put(0, 0);
}


void ResetShip() 
{
  canShootThree = 0;
  PlayerShip.Reset();
}




// Timer Tick just nicely pushes back a user event onto the stack.
// This might be an issue if we're on slow machines that can't
// respond to this event faster than the timer
// TODO: BugFix this..
static int sastWantTicks = 0;
static int FinishedLastCall = 0;

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



/////////////////////////////////////////////////////////////////////////////
// play a game..
void PlayGame()
{
  char pstr[256];
  int pause = 0;
  int dead = 0;
  int finalDead = 0;
  int aegg = 0;
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

  ResetShip();
  FreeObjArray();
  Ui::clearscreen();

  timer = SDL_AddTimer( GAME_CLOCK, TimerTick, 0 );
  
  if(!timer) {
    cout << "Fatal: Couldn't Create Timer!" << endl;
    exit(-1);
  }
  
  keystatebuffer = SDL_GetKeyState(NULL);
  Gbackdrop.SetTrans(false);
 
  
  pause = 0;
  GameOver = 0;
  cReq = 0;
  sastWantTicks = 1;
  finalDead = 0;
  FinishedLastCall = 1;
  deathTimer = -1;

  while (!GameOver) {
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_VIDEORESIZE:
	ScreenLimits.SetXY(DDIVCONST(event.resize.w), 
			   DDIVCONST(event.resize.h));
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
	    !pause) deathTimer = 3; 

	// Leave this cheat code in, nice for debugging.
	if (event.key.keysym.sym == SDLK_n && !pause) Glevel++;
	
	// I prefer space
	if (event.key.keysym.sym == SDLK_SPACE && !pause)
	  Fire();

	// but CTRL is the default
	if (event.key.keysym.sym == SDLK_LCTRL && !pause)
	  Fire();
	
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
	    ResetShip();
	    dead = 0;
	  }
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
	// This only gets called while the GAME CLOCK ITSELF is ticking =)
	deathTimer--;
	if(deathTimer < -128) deathTimer = -1;
	if(finalDead && finalDead > 2) finalDead--;

	if (keystatebuffer[SDLK_RIGHT])
	  PlayerShip.rotRight(1);
	
	if (keystatebuffer[SDLK_LEFT])
	  PlayerShip.rotLeft(1);
	
	if (keystatebuffer[SDLK_UP])
	  PlayerShip.Thrust(0.15f);

	if (keystatebuffer[SDLK_DOWN])
	  PlayerShip.shieldOn();
	else
	  PlayerShip.shieldOff();
	
	if (keystatebuffer[SDLK_b])
	  PlayerShip.Brake();

	
	
	if (numasts <= 0) {
	  Glevel++;
	  numasts = 0;
	  GenerateAsteroids();
	  PlayerShip.addMaxPower(2);
	  PlayerShip.addRegPower(1);
	}

	if (!(rand()%5000)) {
	  int j;
	  j = GetOpenObject();
	  ObjectList[j] = new Enemy;
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
	} else if(dead > 1) { 
	  strcpy(pstr, "Press s to start");
	  dead = 2;
	}
	
	FinishedLastCall = 1;
      }
      
      if(BackdropOn && Ui::WIDTH() <= DMULTCONST(320) &&
	 Ui::HEIGHT() <= DMULTCONST(200)) {
	Gbackdrop.put(0, 0);
      } else {
	Ui::clearscreen();
      }
      DrawObjects();
      botline();

      if(dead) {
	int x, y, width, height;

	width = ShipBitmaps[0].width(); 
	height = ShipBitmaps[0].height();
	x = int(ScreenLimits.GetX()) - width;
	y = int(ScreenLimits.GetY()) - height;
	x = x / 2;
	y = y / 2;
	
	if(flashShip > 24 || flashShip < 0) flashShip = 0;
	flashShip++;
	
	if(flashShip >  12 && !finalDead) {
	  ShipBitmaps[0].put(x, y);
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

      if(!sastWantTicks) {
	Ui::CenterText(pstr);
	FinishedLastCall = 1;
      }

      Ui::updateScreen();
    }
  }

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
    displayScreen(BINDIR "graphics/back.bmp");
    int x, y;

    x = 0;
    y = 0;

    Ui::CenterXText( y + 0, "SASTEROIDS");
    Ui::CenterXText( y + 40, "TO PLAY THE GAME");
   
    Ui::CenterXText( y + 80, "-------------------------");
    Ui::CenterXText( y + 100, " LEFT ARROW - Turn ship Left");
    Ui::CenterXText( y + 120, " RIGHT ARRW - Turn ship Right");
    Ui::CenterXText( y + 140, " SPACE - Fire");
    Ui::CenterXText( y + 160, " UP ARROW - Thrust ");
    Ui::CenterXText( y + 180, " DOWN ARROW - Shield");
    Ui::CenterXText( y + 200, " LEFT ALT - Hyperspace");
    Ui::CenterXText( y + 240, " P - Pause");
    Ui::CenterXText( y + 280, " Q - Quit");
    Ui::updateScreen();
}



/////////////////////////////////////////////////
// Show the Game Title... 
void ShowTitle(int selected)
{
  const int xstart = 40, ystart = 90;
  const int yinc = 10;
  int cstring;
  
  char *tStringList[] =
    { "SDL Sasteroids Version " VERSION,
      "(I) INFORMATION",
      //      "(H) HIGH SCORES",
      "(S) START GAME",
      "(Q) QUIT",
      NULL
    };
      
  titleScreen.put(0,0);
 
  for(cstring = 0; tStringList[cstring] != 0; cstring++) {
    Ui::ShowTextColor(DMULTCONST(xstart),
		      DMULTCONST(ystart+(yinc*cstring)),
		      tStringList[cstring],
		      255, 255, 255);
    if(selected == cstring)
      Ui::ShowTextColor(DMULTCONST(xstart)+1,
			DMULTCONST(ystart+(yinc*cstring))+1,
			tStringList[cstring],
			255, 255, 0);
  }
  
  Ui::updateScreen();
}



// ///////////////////////////////////////////////////////////////////////////
void showScoringInfo()
{
    displayScreen(BINDIR "graphics/back.bmp");	// TODO, rewrite this screen
}


// //////////////////////////////////////////////////////////////////////////
// show high scores screen
// void showScores( HiScore &s )
// { 
// TODO: Hi Score code rewrite
// }








//////////////////////////////////////////////////////////
// Load ze Wavs!
void LoadWavs()
{
#ifdef HAVE_SOUND
  soundSamples[SND_BOOM_A] = Mix_LoadWAV(BINDIR "sounds/boom1.wav");
  soundSamples[SND_BOOM_B] = Mix_LoadWAV(BINDIR "sounds/boom2.wav");
  soundSamples[SND_BOOM_C] = Mix_LoadWAV(BINDIR "sounds/shipexplode.wav");
  soundSamples[SND_FIRE]   = Mix_LoadWAV(BINDIR "sounds/zap.wav");
  soundSamples[SND_WARP]   = Mix_LoadWAV(BINDIR "sounds/warp.wav");
#endif
}




/////////////////////////////////////////////////////////
// Initialize the System
// 
void InitializeSDL()
{
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0) {
    cerr << "Couldn't initialize SDL!" << endl;
    exit(-1);
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



/////////////////////////////////////////////////////////
// Main Program Starts Here 
// Starts all the tasks, etc...
int main(int argc, char *argv[])
{
  char c;
  int done = 0, dirty = 1, mode = 1, menu = 1;
  SDL_Event event;
  
  srand((unsigned int) time(NULL));
  
  InitializeSDL();
  HandleCommandLine(argc, argv);
  InitializeHiScores();


#ifdef HAVE_SOUND
  if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024)==-1) {
    cout << "No sound? Ieeyayayyaaaa: " << Mix_GetError() << endl;
    exit(-1);
  }

  Mix_AllocateChannels(16);
#endif

  FastMath::init(1); 
  Ui::init();
  SetupObjArray();
  LoadBitmaps();
  ShowTitle(menu);
  LoadWavs();

  while (!done) {
    dirty = 0;
    c = 0;
    
    while (SDL_PollEvent(&event)) {
      switch (event.type) {

      case SDL_VIDEORESIZE:
	ScreenLimits.SetXY(DDIVCONST(event.resize.w), 
			   DDIVCONST(event.resize.h));
	Ui::resync(event.resize.w, event.resize.h);
	break;
	
      case SDL_QUIT:
	exit(0);
	break;

      case SDL_KEYDOWN:
	if (event.key.keysym.sym == SDLK_f) {
	  Ui::FullScreen(); dirty = 1;
	}

	if (event.key.keysym.sym == SDLK_h) {
	  c = 'h';
	  PlaySound(SND_BOOM_C);
	}

	if (event.key.keysym.sym == SDLK_i) {
	  c = 'i';
	  PlaySound(SND_BOOM_C);
	}

	if (event.key.keysym.sym == SDLK_q) {
	  c = 'q';
	  PlaySound(SND_BOOM_C);
	}

	if (event.key.keysym.sym == SDLK_s) {
	  c = 's';
	  PlaySound(SND_BOOM_C);
	}

	if (event.key.keysym.sym == SDLK_RETURN) {
	  PlaySound(SND_BOOM_C);
	  if(menu == 1) c = 'i';
	  //	  if(menu == 2) c = 'h';
	  if(menu == 2) c = 's';
	  if(menu == 3) c = 'q';
	}
	
	if (event.key.keysym.sym == SDLK_DOWN) 
	  if(menu < 3 && mode == 1) {
	    PlaySound(SND_FIRE);
	    menu++, dirty = 1;
	  } else if(mode == 1) {
	    PlaySound(SND_FIRE);
	    menu = 1, dirty = 1;
	  }
	
	if (event.key.keysym.sym == SDLK_UP) 
	  if(menu > 1 && mode == 1) {
	    PlaySound(SND_FIRE);
	    menu--, dirty = 1;
	  } else if(mode == 1) {
	    PlaySound(SND_FIRE);
	    menu = 3; dirty = 1;
	  }
	
	if (mode != 1)
	  mode = 1, dirty = 1;
	break;
      case SDL_ACTIVEEVENT:
	dirty = 1;	// Redraw the screen.....
      }
    }
    
    
    switch (c) {
    case 'h':
      mode = 3;
      dirty = 1;
      break;
      
    case 'i':
      mode = 2;
      dirty = 1;
      break;
      
    case 's':
      mode = 1;
      dirty = 1;
      PlayGame();
      SaveHiScores();
      break;
      
    case 'q':
    case 'Q':
      done = 1;
      break;
    }

    
    if (dirty) {
      switch (mode) {
      case 1:
	ShowTitle(menu);
	break;
      case 2:
	ShowInfo();
	break;
      case 3:
	showScoringInfo();
	break;
      }
    }
    
  }
  

#ifdef HAVE_SOUND
  Mix_CloseAudio();
#endif 

  // shutdown & restore text mode
  Ui::restore();
  
  return 0;
}
