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

#define SND_BOOM_A     0
#define SND_BOOM_B     1
#define SND_BOOM_C     2
#define SND_FIRE       3
#define SND_WARP       4

/////////////////////////////////////////////////////////////////////////////
// Global variables: 

// Constants. (tune game here)
const int   GAME_CLOCK    = 27;           // Number of mseconds between ticks.

const float START_DIST    = 70;           // Disance asteroids start from ship
const int   MAXASTEROIDS  = 16;           // Max # Asteroids

const float MassBigAst    = 8.0f;         // Masses for bouncing + collision
const float MassSmallAst  = 4.0f;
const float MassMedAst    = 6.0f;
const float MassShip      = 5.0f / 4.0f;
const float MassEnemy     = 1.0f;
const float MassBullet    = 1.0f / 8.0f;
const float MassSpinner   = 2.0f;

const float BUL_SPEED     = 7.5f;

// System Setup
cfc Gcf;                                  // graphics interface and
SBitmap Gbit[8];                          // graphics. =)
SBitmap Gbackdrop;

#ifdef HAVE_SOUND
Mix_Chunk *soundSamples[8];               // Sound!
#endif

int G_use_backdrop = 0;

// Global Game Play Variables.
Ship PlayerShip;	                  // Info about player's ship.
int score, Glevel, numasts, oldscore;     // scoring and level info
int ClassicMode = 0;                      // classic mode?
int BackdropOn = 1;                       // is the backdrop on?


/////////////////////////////////////////////////////////////////////////
// play sound function (just to help with the #ifdef's not being everywhere
//
inline void PlaySound(int soundNumber)
{
#ifdef HAVE_SOUND
  if(soundSamples[soundNumber])
    Mix_PlayChannel(-1, soundSamples[soundNumber], 0);
#endif
}


/////////////////////////////////////////////////////////////////////////
// set palette by loading one of the game bitmaps.
// This function is a throw back from the vgalib days, but since things
// depend on it so much...
void SetGamePalette()
{
  int i;
  char *pdata;
  RawImageLoader loader(Gcf);
  
  if (loader.load("ship0.raw", LOAD_PALETTE) == EXIT_SUCCESS) {
    for (i = 0; i < loader.numColors(); i++) {
      pdata = (char *) loader.palette();
	    pdata += i * 3;
	    SBitmap::SetPalette(i, pdata[0], pdata[1], pdata[2]);
    }
  } else {
    cout << "Couldn't setup game palette!" << endl;
  }
}


///////////////////////////////////////////////////////
// Load bitmap image from 'file' into 'b'
void LoadBitmap(SBitmap & b, const char *file)
{
    RawImageLoader loader(Gcf);
    if (loader.load(file, LOAD_IMAGE) == EXIT_SUCCESS)
	b.setMap(loader);
    b.setupsurface();
}


//////////////////////////////////////////////////////
// Set up the array of game objects.
void LoadBitmaps()
{
    LoadBitmap(Gbit[SMALLAST], "smallast.raw");
    LoadBitmap(Gbit[MEDAST], "medast.raw");
    LoadBitmap(Gbit[BIGAST], "bigast.raw");
    LoadBitmap(Gbit[ENEMY], "enemy.raw");
    LoadBitmap(Gbit[BULLET], "bullet.raw");
    LoadBitmap(Gbit[BULLET2], "bullet2.raw");
    LoadBitmap(Gbit[SPINNER], "spinner.raw");
    LoadBitmap(Gbit[SHIELD], "shield.raw");
    LoadBitmap(Gbackdrop, "back1.raw");
    LoadBitmap(morphBitmaps[0], "medast.raw");
    LoadBitmap(morphBitmaps[1], "morph1.raw");
    LoadBitmap(morphBitmaps[2], "morph2.raw");
    LoadBitmap(morphBitmaps[3], "morph3.raw");
    LoadBitmap(morphBitmaps[4], "morph4.raw");
    LoadBitmap(morphBitmaps[5], "morph5.raw");
    LoadBitmap(morphBitmaps[6], "morph6.raw");
    LoadBitmap(morphBitmaps[7], "morph7.raw");
    LoadBitmap(morphBitmaps[8], "morph8.raw");
    LoadBitmap(morphBitmaps[9], "morph9.raw");

    InitShips();
}


/////////////////////////////
// Setup Objects for drawing
void SetupObjects()
{
    int i;

    for (i = 0; i < 8; i++) {
	Gbit[i].setupsurface();
    }

    for (i = 0; i < 10; i++) {
	morphBitmaps[i].setupsurface();
    }
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



////////////////////////////////////////////////////////
// Hit a Spinner (called when Spinner is shot)
void HitSpinner(int number, int killedBy)
{
  Spinner* spinner;

  spinner = (Spinner*) ObjectList[number];
  spinner->hit();

  if(!spinner->alive()) {
    Explosion* explode;
    int j;
    explode = new Explosion(spinner->GetX(),
			    spinner->GetY(),
			    spinner->VelX(),
			    spinner->VelY());
    j = GetOpenObject();
    ObjectList[j] = explode;
  }
}

void HitEnemy(int number, int killedBy)
{
  int j;
  Enemy* e;

  e = (Enemy*) ObjectList[number];
  ObjectList[number]->die();
  if(killedBy == 254) upscore(1000);
  
  j = GetOpenObject();
  ObjectList[j] = new Explosion(e->GetX(),
				e->GetY(),
				e->VelX(),
				e->VelY());
}


///////////////////////////////////////////////////////
// Kill an asteroid - and create new ones if needed.
void KillAsteroid(int number, int killedBy)
{
  int j, ctype;
  Explosion* explode;

  if(ObjectList[number]->alive()) {
    // play a sound. :)
    PlaySound(SND_BOOM_B);
    
    numasts -= 1;
    
    // this asteroid is dead now
    ObjectList[number]->die();

    // explode the original asteroid!
    j = GetOpenObject();
    explode = new Explosion(ObjectList[number]->GetX(),
			    ObjectList[number]->GetY(),
			    ObjectList[number]->VelX(),
			    ObjectList[number]->VelY());
    ObjectList[j] = explode;

    // up the points, but only if WE killed the asteroid
    if(ObjectList[killedBy]->type() == SHIP_BUL) {
      if(ObjectList[number]->type() == BIGAST) upscore(500);
      if(ObjectList[number]->type() == MEDAST) upscore(250);
      if(ObjectList[number]->type() == SMALLAST) upscore(100);
    }
    
    // figure out the new type of asteroids to create.
    if(ObjectList[number]->type() == BIGAST) ctype = MEDAST;
    else if(ObjectList[number]->type() == MEDAST) ctype = SMALLAST;
    else ctype = 255;
    
    // create the asteroids
    // when creating the asteroid we blast 2 at a time in opposite
    // directions. TODO: Make this a more correct in terms of physics.
    if(ctype != 255) {
      char rA1, rA2;
      float px, py, vx, vy;
      
      rA1 = rand() % 255; 
      rA2 = int(rA1 + 128) % 255;
      
      px = ObjectList[number]->GetX();
      py = ObjectList[number]->GetY();
      vx = ObjectList[number]->VelX();
      vy = ObjectList[number]->VelY();
         
      CreateAsteroid( px, py, 
		      FastMath::sin(rA1) * 1.0f + vx, 
		      FastMath::cos(rA1) * 1.0f + vy, ctype );     
      CreateAsteroid( px, py,
		      FastMath::sin(rA2) * 1.0f + vx,
		      FastMath::cos(rA2) * 1.0f + vy, ctype );
      numasts+=2;

      do {
	rA2 = rand() % 255;
      } while( abs(rA2 - rA1) < 5 && abs(rA2 - (rA1+124)) < 5);      
      rA1 = int(rA2 + 128) % 255;
      
      CreateAsteroid( px, py,
		      FastMath::sin(rA1) * 1.0f + vx,
		      FastMath::cos(rA1) * 1.0f + vy, ctype );
      numasts++;

      if(ctype == MEDAST && !(rand()%10)) {
	j = GetOpenObject();
	ObjectList[j] = new Spinner;
	ObjectList[j]->SetXY(px, py);
	ObjectList[j]->SetVel(FastMath::sin(rA2)*1.0f + vx, 
			      FastMath::cos(rA2)*1.0f + vy);
      } else {
	CreateAsteroid( px, py, 
			FastMath::sin(rA2) * 1.0f + vx,
			FastMath::cos(rA2) * 1.0f + vy, ctype );
	numasts++;
      }     
    }   
  }
}


//////////////////////////////////////////////////////
// Bounce Objects? Why Yes!
void BounceObjects(int i, int j) 
{
  // TODO: BOUNCE OBJECTS
}


////////////////////////////////////////////////////////////////////////////
// create asteroids for beginning of each level/game
void GenerateAsteroids()
{
    int i, obj;
    float x, y;
    Vector temp;

    //    FreeObjArray();

    for (i = 0; i <= (Glevel / 2 + 1); i++) {
	do {
	    x = (float) (rand() % 320);
	    y = (float) (rand() % 200);
	    temp.SetXY(x, y);
	}
	while (PlayerShip.GetXYVec().length(temp) < START_DIST);

	obj = CreateAsteroid(x, y, 0, 0, BIGAST);

	if(ObjectList[obj]) {
	  ObjectList[obj]->randomDir(0.40f);
	}
	
    }

    numasts = i;
}







// /////////////////////////////////////////////////////////////////////////
// Move all the game objects, check for collisions... etc...
int MoveObjects()
{
  
  int i = 0, crash = 0, touched = 0;
  
  AllObjectsTick();
  
  // Loop through every object.
  for (i = 1; i < MAX_OBJECTS; i++) {
    if (ObjectList[i]) {
      
      // Check for ship collision....
      if (collide(*ObjectList[i], *ObjectList[0])
	  && ObjectList[i]->type() != 254
	  && ObjectList[i]->type() != 255) {
	crash = 1;
	if(!PlayerShip.isDeadStick() && !touched) {
	  switch(ObjectList[i]->type()) {
	  case SMALLAST:
	  case MEDAST:
	  case BIGAST:
	    KillAsteroid(i, 0);
	    break;
	  case SPINNER:
	    HitSpinner(i, 0);
	    HitSpinner(i, 0);
	    HitSpinner(i, 0);
	    HitSpinner(i, 0);
	    HitSpinner(i, 0);
	    break;
	  case BULLET2:
	    ObjectList[i]->die();
	  } 
	  touched = 1;
	}
      }
      
      // Check for asteroid collisions... etc...
      for (int j = 1; j < MAX_OBJECTS; j++) {
	if (ObjectList[j]) {
	  
	  // Check every object against every other here 
	  if (i == j)
	    continue;	/* Don't check against self */
	  
	  if (ObjectList[i]->alive()
	      && (ObjectList[i]->type() == 254 || 
		  ObjectList[i]->type() == BULLET2)
	      && collide(*ObjectList[i], *ObjectList[j])) {
	    switch (ObjectList[j]->type()) {
	    case SMALLAST:
	    case BIGAST:
	    case MEDAST:
	      KillAsteroid(j, i);
	      ObjectList[i]->die();
	      break;
	    case SPINNER:
	      HitSpinner(j, i);
	      ObjectList[i]->die();
	      break;
	    case ENEMY:
	      if(ObjectList[i]->type() != BULLET2) 
		HitEnemy(j, i);
	    }
	  } else {
	    if(ObjectList[i]->type() == SPINNER || 
	       ObjectList[j]->type() == SPINNER) {
	      BounceObjects(i, j);
	    }    
	  }
	  
	} // end if(ObjectList[i])
      }
      
    }
  }
  
  return crash;
}


////////////////////////////////////////////////////////////////////////////
// fire a bullet for player ship
void Fire()
{
  int bullet;
  float fcos, fsin;
  float rx, ry;
  
  
  if (PlayerShip.weaponPower() > 0) {
    PlaySound(SND_FIRE);
    
    bullet = GetOpenObject();
    
    ObjectList[bullet] = new ScreenObject;
    ObjectList[bullet]->restore();
    fsin = -FastMath::cos(PlayerShip.getAngle());
    fcos = FastMath::sin(PlayerShip.getAngle());
    rx = PlayerShip.GetX() + (PlayerShip.GetWidth() / 2);
    ry = PlayerShip.GetY() + (PlayerShip.GetHeight() / 2);
    
    ObjectList[bullet]->SetXY(rx + PlayerShip.Size() * fcos,
				  ry + PlayerShip.Size() * fsin);
    
    ObjectList[bullet]->SetVel(PlayerShip.VelX() + BUL_SPEED * fcos,
			       PlayerShip.VelY() + BUL_SPEED * fsin);
    
    // Give it a bit of a kick back... ehehehheeheh
    if(!ClassicMode)
      PlayerShip.AddVel(-fcos / 10.0f, -fsin / 10.0f);
    
    ObjectList[bullet]->SetAcc(0.0f, 0.0f);
    ObjectList[bullet]->SetBitmap(&Gbit[BULLET]);
    ObjectList[bullet]->SetWrapMoves(false);
    ObjectList[bullet]->SetMaxSpeed(255.0f);
    ObjectList[bullet]->settype(SHIP_BUL);	/* SHIP_BUL(254) is bullet */
    
    PlayerShip.dischargeWeapon();
  } else {
    // TODO: Do something interesting.
  }
}


////////////////////////////////////////////////////////////////////////////
// botline - modified from xasteroids
void botline()
{
    char text[256] = { 0 };
    int y;

    y = Ui::HEIGHT() - Ui::fontHeight();

    sprintf(text, "Score:%7d Ships:%2d  Lvl:%2d  Shield:%2d",
	    score, PlayerShip.ships(), Glevel, PlayerShip.shieldPercent());

    Ui::ShowText(0, y, text);	// This routine is a bit slow.. but
}


//////////////////////////////////////////////////////////////////////////
void displayScreen(char *file)
{
  RawImageLoader loader(Gcf);
  int i;
  
  loader.load(file, LOAD_IMAGE | LOAD_PALETTE);
  
  // assign TITLE.raw to a bitmap
  SBitmap scn;
  
  scn.setMap(loader);
  Ui::clearscreen();
  
  for (i = 0; i < 256; i++) {
    SBitmap::SetPalette(i, 0, 0, 0);
  }
  
  // set palette according to TITLE.raw palette
  for (i = 0; i < loader.numColors(); i++) {
    unsigned char a = 0, b = 0, c = 0;
    
    a = (unsigned char) ((char *) loader.palette())[i * 3 + 0];
    b = (unsigned char) ((char *) loader.palette())[i * 3 + 1];
    c = (unsigned char) ((char *) loader.palette())[i * 3 + 2];
    SBitmap::SetPalette(i, a, b, c);
  }
  
  loader.releasePaletteOwnership();
  scn.setupsurface();
  scn.put(0, 0);
}


void ResetShip() 
{
  int x, y, width, height;
  
  width = ShipBitmaps[0].width(); 
  height = ShipBitmaps[0].height();
  x = int(ScreenLimits.GetX()) - width;
  y = int(ScreenLimits.GetY()) - height;
  x = x / 2;
  y = y / 2;

  PlayerShip.SetDeadStick(0);
  PlayerShip.restore();
  PlayerShip.SetSize(5, 5);
  PlayerShip.SetXY(x, y);
  PlayerShip.SetupDrawing();
  PlayerShip.SetVel(0.0f, 0.0f);
  PlayerShip.resetDirection();
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


  Ui::clearscreen();
  timer = SDL_AddTimer( GAME_CLOCK, TimerTick, 0 );
  
  if(!timer) {
    cout << "Fatal: Couldn't Create Timer!" << endl;
    exit(-1);
  }
  
  SetupObjects();
  keystatebuffer = SDL_GetKeyState(NULL);
  Gbackdrop.SetTrans(0);
  Gbackdrop.setupsurface();
  
  pause = 0;
  GameOver = 0;
  cReq = 0;
  sastWantTicks = 1;
  FinishedLastCall = 1;

  while (!GameOver) {
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_VIDEORESIZE:
	ScreenLimits.SetXY(DDIVCONST(event.resize.w), 
			   DDIVCONST(event.resize.h));
	Ui::resync(event.resize.w, event.resize.h);
	break;
	
      case SDL_KEYDOWN:
	if (event.key.keysym.sym == SDLK_LALT && !pause && !dead) {
	  PlaySound(SND_WARP);
	  PlayerShip.Hyper();
	}
	
	if (event.key.keysym.sym == SDLK_RALT && !pause && !dead) {
	  PlaySound(SND_WARP);
	  PlayerShip.Hyper();
	}
	
	if (event.key.keysym.sym == SDLK_SPACE && !pause)
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

	if(event.key.keysym.sym == SDLK_s) {
	  if(dead) {
	    ResetShip();
	    dead = 0;
	  }
	}

	
	
	break;
      case SDL_ACTIVEEVENT:
	;
	break;

      case SDL_QUIT:
	exit(0);
	break;
	
      case SDL_USEREVENT:	      
	// This only gets called while the GAME CLOCK ITSELF is ticking =)
	
	if (keystatebuffer[SDLK_RIGHT])
	  PlayerShip.rotRight(1);
	
	if (keystatebuffer[SDLK_LEFT])
	  PlayerShip.rotLeft(1);
	
	// Maybe ill just leave these alone... mwahahahahahhahahh
	if (keystatebuffer[SDLK_0]);	// TODO: CHEAT(for authenticity)
	if (keystatebuffer[SDLK_n]);	// TODO: next level
	
	if (keystatebuffer[SDLK_UP])
	  PlayerShip.Thrust(0.1f);
	
	if (keystatebuffer[SDLK_b])
	  PlayerShip.Brake();
	
	if (numasts <= 0) {
	  Glevel++;
	  numasts = 0;
	  GenerateAsteroids();
	}

	if (!(rand()%5000)) {
	  int j;
	  j = GetOpenObject();
	  ObjectList[j] = new Enemy;
	}

	
	dead += MoveObjects();
	if(dead == 1) {
	  PlaySound(SND_BOOM_C);
	  int j;
	  
	  strcpy(pstr, "Press s to start");
	  PlayerShip.death();
	  PlayerShip.SetDeadStick(1);
	  if(PlayerShip.ships() <= 0) { 
	    GameOver = 1;
	    sastWantTicks = 0;
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
	
	if(flashShip >  12) {
	  ShipBitmaps[0].put(x, y);
	}
	
	Ui::CenterXText(180, pstr);
      }

      if(!sastWantTicks) {
	Ui::CenterText(pstr);
	SetGamePalette();
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
    Ui::drawToPhysical();
    displayScreen("back.raw");
    // TODO: skill level selection...
    return 1;
}


// //////////////////////////////////////////////////////////////////////////
void showInfo()
{
    displayScreen("back.raw");
    int x, y;

    x = 0;
    y = 0;

    Ui::ShowText(x, y + 0, "SASTEROIDS");
    Ui::ShowText(x, y + 40, "TO PLAY THE GAME");
    Ui::ShowText(x, y + 60, "KEY");
    Ui::ShowText(x, y + 80, "---------");
    Ui::ShowText(x, y + 100, " LEFT ARROW");
    Ui::ShowText(x, y + 120, " RIGHT ARRW");
    Ui::ShowText(x, y + 140, " SPACE     ");
    Ui::ShowText(x, y + 160, " UP ARROW  ");
    Ui::ShowText(x, y + 180, " DOWN ARROW ");
    Ui::ShowText(x, y + 200, " LEFT ALT   ");
    Ui::ShowText(x, y + 240, " P          ");
    Ui::ShowText(x, y + 280, " Q          ");
    Ui::ShowText(x + 140, y + 60, "ACTION");
    Ui::ShowText(x + 140, y + 80, "------------------------");
    Ui::ShowText(x + 140, y + 100, "Turn ship left");
    Ui::ShowText(x + 140, y + 120, "Turn ship right");
    Ui::ShowText(x + 140, y + 140, "Fire");
    Ui::ShowText(x + 140, y + 160, "Thrust");
    Ui::ShowText(x + 140, y + 180, "Shield, one per ship");
    Ui::ShowText(x + 140, y + 200, "Hyperspece (zap to");
    Ui::ShowText(x + 140, y + 220, "random area on screen)");
    Ui::ShowText(x + 140, y + 240, "Pause ");
    Ui::ShowText(x + 140, y + 280, " Quit game");
    updateScreen();
}


// Show the Game Title..
void ShowTitle(int selected)
{
    displayScreen("title.raw");

    Ui::ShowTextColor(DMULTCONST(40), DMULTCONST(90),
		      "SDL Sasteroids Version " VERSION,
		      255, 255, 255);
    
    Ui::ShowTextColor(DMULTCONST(40), DMULTCONST(100),
		      "(I) INFORMATION",
		      255, 255, 255);
    
    Ui::ShowTextColor(DMULTCONST(40), DMULTCONST(110),
		      "(H) HIGH SCORES",
		      255, 255, 255);
    
    Ui::ShowTextColor(DMULTCONST(40), DMULTCONST(120),
		      "(S) START GAME",
		      255, 255, 255);

    Ui::ShowTextColor(DMULTCONST(40), DMULTCONST(130),
		      "(Q) QUIT",
		      255, 255, 255);


    switch(selected) 
      {
      case 1:
	Ui::ShowTextColor(DMULTCONST(40)+1, DMULTCONST(100)+1,
			  "(I) INFORMATION",
			  255, 255, 0);
	break;
      case 2:
	Ui::ShowTextColor(DMULTCONST(40)+1, DMULTCONST(110)+1,
			  "(H) HIGH SCORES",
			  255, 255, 0);
	break;
      case 3:
	Ui::ShowTextColor(DMULTCONST(40)+1, DMULTCONST(120)+1,
			  "(S) START GAME",
			  255, 255, 0);
	break;
      case 4:   
	Ui::ShowTextColor(DMULTCONST(40)+1, DMULTCONST(130)+1,
			  "(Q) QUIT",
			  255, 255, 0);
	break;
      }
    
    updateScreen();
}



// ///////////////////////////////////////////////////////////////////////////
void showScoringInfo()
{
    displayScreen("back.raw");	// TODO, rewrite this screen....
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
  soundSamples[SND_BOOM_A] = Mix_LoadWAV(BINDIR "boom1.wav");
  soundSamples[SND_BOOM_B] = Mix_LoadWAV(BINDIR "boom2.wav");
  soundSamples[SND_BOOM_C] = Mix_LoadWAV(BINDIR "shipexplode.wav");
  soundSamples[SND_FIRE]   = Mix_LoadWAV(BINDIR "zap.wav");
  soundSamples[SND_WARP]   = Mix_LoadWAV(BINDIR "warp.wav");
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
// Main Program Starts Here 
// Starts all the tasks, etc...
int main(int argc, char *argv[])
{
  char c;
  int done = 0, dirty = 1, mode = 1, menu = 1;
  SDL_Event event;
  
  srand((unsigned int) time(NULL));
  
  InitializeSDL();

  cerr << "Opening: " << BINDIR "sast.cf" << endl;
  if (Gcf.open(BINDIR "sast.cf") != 0) {
    cerr << "Open failed on sast.cf" << endl;
    exit(-1);
  }

#ifdef HAVE_SOUND
  if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024)==-1) {
    cout << "No sound? Ieeyayayyaaaa: " << Mix_GetError() << endl;
    exit(-1);
  }

  Mix_AllocateChannels(16);
#endif
  

  FastMath::init(1);
  SetupObjArray();
  LoadBitmaps();	      
  Ui::init();
  SetGamePalette();	
  ShowTitle(menu);
  LoadWavs();
  
  while (!done) {
    dirty = 0;
    c = 0;
    
    while (SDL_PollEvent(&event)) {
      switch (event.type) {

      case SDL_VIDEORESIZE:
	ScreenLimits.SetXY(DMULTCONST(event.resize.w), 
			   DMULTCONST(event.resize.h));
	Ui::resync(event.resize.w, event.resize.h);
	break;
	
      case SDL_QUIT:
	exit(0);
	break;

      case SDL_KEYDOWN:
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
	  if(menu == 2) c = 'h';
	  if(menu == 3) c = 's';
	  if(menu == 4) c = 'q';
	}
	
	if (event.key.keysym.sym == SDLK_DOWN) 
	  if(menu < 4 && mode == 1) {
	    PlaySound(SND_FIRE);
	    menu++, dirty = 1;
	  }
	
	if (event.key.keysym.sym == SDLK_UP) 
	  if(menu > 1 && mode == 1) {
	    PlaySound(SND_FIRE);
	    menu--, dirty = 1;
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
      SetGamePalette();
      PlayGame();
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
	showInfo();
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
