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
// const int   GAME_CLOCK    = 27;           // Number of mseconds between ticks.
// const int GAME_CLOCK = 1; // For Benchmarking...
const int GAME_CLOCK = 50; // For tuning...
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


//////////////////////////////////////////////////////
// Set up the array of game objects.
void LoadBitmaps()
{
  Gbit[SMALLAST].LoadImage(BINDIR "/graphics/smallast.bmp");
  Gbit[ESMAST].LoadImage(BINDIR "/graphics/esmallast.bmp");
  Gbit[MEDAST].LoadImage(BINDIR "/graphics/medast.bmp");
  Gbit[BIGAST].LoadImage(BINDIR "/graphics/bigast.bmp");
  Gbackdrop.LoadImage(BINDIR "/graphics/back1.bmp", false);
  titleScreen.LoadImage(BINDIR "/graphics/title.bmp", false);
  Gbit[ENEMY].LoadImage(BINDIR "/graphics/enemy.bmp");
  Gbit[BULLET].LoadImage(BINDIR "/graphics/bullet.bmp");
  Gbit[BULLET2].LoadImage(BINDIR "/graphics/bullet2.bmp");
  Gbit[SPINNER].LoadImage(BINDIR "/graphics/spinner.bmp");
  Gbit[SHIELD].LoadImage(BINDIR "/graphics/shield.bmp");
  Gbit[P_WMAX].LoadImage(BINDIR "/graphics/wMaxPowerUp.bmp");
  Gbit[P_WENG].LoadImage(BINDIR "/graphics/wRechargePowerUp.bmp");
  Gbit[P_WTHR].LoadImage(BINDIR "/graphics/wThreePowerUp.bmp");
  extraLives.LoadImage(BINDIR "/graphics/ship0.bmp");
  extraLives.scalep5();
  extraLives.SetTrans(true);
  morphBitmaps[0].LoadImage(BINDIR "/graphics/medast.bmp");
  morphBitmaps[1].LoadImage(BINDIR "/graphics/morph1.bmp");
  morphBitmaps[2].LoadImage(BINDIR "/graphics/morph2.bmp");
  morphBitmaps[3].LoadImage(BINDIR "/graphics/morph3.bmp");
  morphBitmaps[4].LoadImage(BINDIR "/graphics/morph4.bmp");
  morphBitmaps[5].LoadImage(BINDIR "/graphics/morph5.bmp");
  morphBitmaps[6].LoadImage(BINDIR "/graphics/morph6.bmp");
  morphBitmaps[7].LoadImage(BINDIR "/graphics/morph7.bmp");
  morphBitmaps[8].LoadImage(BINDIR "/graphics/morph8.bmp");
  morphBitmaps[9].LoadImage(BINDIR "/graphics/morph9.bmp"); 
  
  InitShips();
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


//////////////////////////////////////////////////////
// Hit the stupid enemy person.
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
void KillAsteroid(int number, int killedBy, bool killChildren = false)
{
  int j, ctype;
  Explosion* explode;

  if(ObjectList[number]->alive()) {
    // play a sound. :)
    PlaySound(SND_BOOM_B);
    
    numasts -= 1;
    
    // this asteroid is dead now
    // explode the original asteroid!
    j = GetOpenObject();
    explode = new Explosion(ObjectList[number]->GetX(),
			    ObjectList[number]->GetY(),
			    ObjectList[number]->VelX(),
			    ObjectList[number]->VelY());
    ObjectList[j] = explode;

    // Check if we need to do something special when the asteroid dies
    if(ObjectList[number]->type() == ESMAST) {
      unsigned char angle; 

      for(angle = 0; angle < 220; angle += 30) {
	float fsin = FastMath::sin(angle) * 4.0f;
	float fcos = FastMath::cos(angle) * 4.0f;
	int j = GetOpenObject();
	ObjectList[j] = new ScreenObject;
	ObjectList[j]->restore();
	ObjectList[j]->SetVel(fsin, fcos);
	ObjectList[j]->SetXY(ObjectList[number]->GetX(), 
			     ObjectList[number]->GetY());
	ObjectList[j]->SetAcc(0.0f, 0.0f);
	ObjectList[j]->SetWrapMoves(false);
	ObjectList[j]->SetMaxSpeed(255.0f);
	ObjectList[j]->settype(BULLET2);
	ObjectList[j]->SetBitmap(&Gbit[BULLET2]);
	ObjectList[j]->setsize(5);
      } 
    }

    ObjectList[number]->die();

    // up the points, but only if WE killed the asteroid
    if(killedBy != 0 && ObjectList[killedBy]) {
      if(ObjectList[killedBy]->type() == SHIP_BUL) {
	if(ObjectList[number]->type() == BIGAST) upscore(500);
	if(ObjectList[number]->type() == MEDAST) upscore(250);
	if(ObjectList[number]->type() == SMALLAST) upscore(100);
      }
    }
    
    // figure out the new type of asteroids to create.
    if(ObjectList[number]->type() == BIGAST) ctype = MEDAST;
    else if(ObjectList[number]->type() == MEDAST) ctype = SMALLAST;
    else ctype = 255;
    
    // create the asteroids
    // when creating the asteroid we blast 2 at a time in opposite
    // directions. TODO: Make this a more correct in terms of physics.
    if(ctype != 255 && !killChildren) {
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
      } else if(ctype == SMALLAST && !(rand()%10) && !ClassicMode) {
	CreateAsteroid( px, py, 
			FastMath::sin(rA2) * 1.0f + vx,
			FastMath::cos(rA2) * 1.0f + vy, ESMAST );
	numasts++;
      } else {
	CreateAsteroid( px, py, 
			FastMath::sin(rA2) * 1.0f + vx,
			FastMath::cos(rA2) * 1.0f + vy, ctype );
	numasts++;
      }
    }   
  }
}


///////////////////////////////////////////////////////
// Do the PowerUps
void PowerUpF(int i)
{
  int ptype;
  PowerUp* iobj;

  iobj = (PowerUp*)ObjectList[i];
  if(!iobj) return;

  ptype = iobj->effect();
  switch(ptype) {
  case P_WMAX:
    PlayerShip.addMaxPower(20);
    break;
  case P_WENG:
    PlayerShip.addRegPower(2);
    break;
  case P_WTHR:
    canShootThree = 1;
    break;
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

    for (i = 0; i <= (Glevel / 2 + 1); i++) {
	do {
	    x = (float) (rand() % 320);
	    y = (float) (rand() % 200);
	    temp.SetXY(x, y);
	}
	while (PlayerShip.GetXYVec().length(temp) < START_DIST);

	obj = CreateAsteroid(x, y, 0, 0, BIGAST);
	numasts++;

	if(ObjectList[obj]) {
	  ObjectList[obj]->randomDir(0.40f);
	}

	if(!(rand()%3)) {
	  do {
	    x = (float) (rand() % 320);
	    y = (float) (rand() % 200);
	    temp.SetXY(x, y);
	  }
	  while (PlayerShip.GetXYVec().length(temp) < START_DIST);
	  obj = CreateAsteroid(x, y, 0, 0, MEDAST);
	  if(ObjectList[obj]) {
	    ObjectList[obj]->randomDir(1.00f);
	  }
	  numasts++;
	}
	
	if(!(rand()%6)) {
	  do {
	    x = (float) (rand() % 320);
	    y = (float) (rand() % 200);
	    temp.SetXY(x, y);
	  }
	  while (PlayerShip.GetXYVec().length(temp) < START_DIST);
	  obj = CreateAsteroid(x, y, 0, 0, SMALLAST);
	  if(ObjectList[obj]) {
	    ObjectList[obj]->randomDir(1.40f);
	  }
	  numasts++;
	}	
    }
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
	  && ObjectList[i]->type() != 255
	  && ObjectList[i]->alive()) {
	crash = 1;
	if(!PlayerShip.isDeadStick() && !touched) {
	  switch(ObjectList[i]->type()) {
	  case SMALLAST:
	  case MEDAST:
	  case BIGAST:
	  case ESMAST:
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
	    break;
	  case P_TYPE:
	    ObjectList[i]->die();
	    PowerUpF(i);
	    crash = 0; touched--;
	  } 
	  touched++;
	}
      }
      
      // Check for asteroid collisions... etc...
      for (int j = 1; j < MAX_OBJECTS; j++) {
	if (ObjectList[j]) {
	  
	  // Check every object against every other here 
	  if (i == j)
	    continue;	/* Don't check against self */
	  
	  if (ObjectList[i]->alive() && ObjectList[j]->alive()
	      && (ObjectList[i]->type() == 254 || 
		  ObjectList[i]->type() == BULLET2)
	      && collide(*ObjectList[i], *ObjectList[j])) {
	    switch (ObjectList[j]->type()) {
	    case SMALLAST:
	    case BIGAST:
	    case MEDAST:
	    case ESMAST:
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


// Utility Function.
void CleanUpStuff() 
{
  int i;
  for(i = 1; i < MAX_OBJECTS; i++) {
    if(!ObjectList[i]) continue;
    if(ObjectList[i]->type() == MEDAST ||
       ObjectList[i]->type() == SMALLAST ||
       ObjectList[i]->type() == BIGAST) {
      if(numasts > 1) { 
	KillAsteroid(i, 0);
      } 
    }
  } 
  
  canShootThree = 1;
  deathTimer = 128;
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

	if (!(rand()%250) && !ClassicMode) {
	  int j;
	  j = GetOpenObject();
	  ObjectList[j] = new PowerUp;
	}

	
	dead += MoveObjects();
	if(deathTimer == 0) { dead = 1; deathTimer = -1; }

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
      "(H) HIGH SCORES",
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
      //      SetGamePalette();
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
