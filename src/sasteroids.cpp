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
#include <PlayingField.hpp>
using namespace std;

#include <iostream>
#include <sstream>
#include <stdexcept>

/////////////////////////////////////////////////////////////////////////////
// Function Declarations:
void displayScreen( const char *screen );
void upscore( int upby );

/////////////////////////////////////////////////////////////////////////////
// Global variables: 

// System Setup

PlayingField *m_manager_ptr;

int use_joystick, num_joysticks;
SDL_Joystick *js;
void SDL_JoystickUpdate();

int G_use_backdrop = 0;

// Global Game Play Variables.
Player PlayerShip; // Info about player's ship.
int score, Glevel, numasts, oldscore; // scoring and level info
int ClassicMode = 0; // classic mode?
int BackdropOn = 1, wantFullScreen = 0; // is the backdrop on?

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
    for( i = 0; i < 10; i++ )
    {
        strcpy( HiScoreStrings[i], "" );
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
int LevelOdds( int lvlMax, int maxChance, int lvlStep )
{
    int lvlDifference;
    lvlDifference = lvlMax - Glevel;
    lvlDifference = (lvlDifference - 1) * lvlStep;
    if( lvlDifference < maxChance )
        lvlDifference = maxChance;
    if( lvlDifference < 1 )
        lvlDifference = 1;
    return lvlDifference;
}

/////////////////////////////////////////////////////////////////////////
// play sound function (just to help with the #ifdef's not being everywhere
//
void PlaySound( int soundNumber )
{
    if( soundSamples[soundNumber] )
        Mix_PlayChannel( -1, soundSamples[soundNumber], 0 );
}

//////////////////////////////////////////////////////////////
// upscore - from xasteroids
void upscore( int up )
{
    score = score + up;

    if( oldscore + 60000 < score )
    {
        PlayerShip.addship();
        oldscore = score;
        PlaySound( SND_POWERUP );
    }
}

////////////////////////////////////////////////////////////////////////////
// botline - modified from xasteroids
void botline()
{
    int y, x, x1;
    static int animstage = 200;
    static int animdir = -2;

    UserInterfaceManager::display_integer( PlayerShip.get_score(), 10.0f,
            float( UserInterfaceManager::HEIGHT() ) - 2.0f );
    UserInterfaceManager::display_integer( Glevel,
            UserInterfaceManager::WIDTH() - 40.0f, 20.0f );

    animstage += animdir;
    if( animstage < 155 )
        animdir = -animdir;
    if( animstage > 205 )
        animdir = -animdir;
    GLfloat anim_alpha = GLfloat( double( animstage ) / 255.0 );

    y = UserInterfaceManager::HEIGHT() - 20;
    x1 = UserInterfaceManager::WIDTH() - 205;

    // Draw weapon meter
    if( !ClassicMode )
    {
        bmp_bullet.draw( x1 - 12, (y) );

        glBindTexture( GL_TEXTURE_2D, 0 );
        glBegin( GL_QUADS );
        glColor4f( 1.0, 0.25, 0, anim_alpha );
        glVertex2f( GLfloat( x1 ), GLfloat( y ) );

        GLfloat maxX = std::max(
                GLfloat( x1 + PlayerShip.weaponPercent() * 2 ),
                GLfloat( x1 )
                        );

        glColor4f( GLfloat( std::max(
                double( 1.0 - (PlayerShip.weaponPercent() * 0.01) ),
                0.0
                ) ),
                GLfloat( std::min(
                        double( 0.25 + PlayerShip.weaponPercent() * 0.01 ),
                        1.0
                        ) ),
                GLfloat( 0.0 ),
                anim_alpha
                );
        glVertex2f( maxX, GLfloat( y ) );
        glVertex2f( maxX, GLfloat( y + 4 ) );

        glColor4f( 1.0, 0.25, 0, anim_alpha );
        glVertex2f( GLfloat( x1 ), GLfloat( y + 4 ) );
        glEnd();
        y += 10;
    }

    bmp_bullet.draw( x1 - 12, y );
    {
        glBindTexture( GL_TEXTURE_2D, 0 );
        glBegin( GL_QUADS );
        glColor4f( 1.0, 0.25, 0, anim_alpha );
        glVertex2f( GLfloat( x1 ), GLfloat( y ) );

        GLfloat maxX = std::max(
                GLfloat( x1 + PlayerShip.shieldPercent() * 2 ),
                GLfloat( x1 )
                        );

        glColor4f( GLfloat( std::max(
                double( 1.0 - (PlayerShip.shieldPercent() * 0.01) ),
                0.0
                ) ),
                GLfloat( std::min(
                        double( 0.25 + PlayerShip.shieldPercent() * 0.01 ),
                        1.0
                        ) ),
                GLfloat( 0.0 ),
                anim_alpha
                );
        glVertex2f( maxX, GLfloat( y ) );
        glVertex2f( maxX, GLfloat( y + 4 ) );

        glColor4f( 1.0, 0.25, 0, anim_alpha );
        glVertex2f( GLfloat( x1 ), GLfloat( y + 4 ) );
        glEnd();
    }

    y = (UserInterfaceManager::HEIGHT() - 24);
    for( x = 0; x < (PlayerShip.ships() - 1); x++ )
    {
        extraLives.draw( 16 + x * 20, 15.0f, 0.5f );
    }
}

//////////////////////////////////////////////////////////////////////////
void displayScreen( const char *file )
{
    ScreenBitmap backdrop( file );
    UserInterfaceManager::clearscreen();
    backdrop.draw_alpha( 0, UserInterfaceManager::HEIGHT() );
}

// Timer Tick just nicely pushes back a user event onto the stack.
// This might be an issue if we're on slow machines that can't
// respond to this event faster than the timer
// TODO: BugFix this..
static int FinishedLastCall = 0;
char globalMessage[256];
int gMsgTimeLeft = 0;

void MakeGlobalMessage( char* Tstring )
{
    strcpy( globalMessage, Tstring );
    gMsgTimeLeft = UserInterfaceManager::HEIGHT();
}

/////////////////////////////////////////////////////////////////////////////
// play a game..

class Sasteroids
{
public:
    Sasteroids();
    virtual ~Sasteroids();

    void run();

    void set_scrolling_message( const std::string& p_str )
    {
        scrolling_message = p_str;
        scrolling_message_pos = UserInterfaceManager::HEIGHT();
    }

    void reset_center_message()
    {
        if( quit_request )
            center_message = "Press y to quit, any other key to return to game";
        else if( is_game_over() )
            center_message = "Press any key to return to main menu";
        else if( pause_request )
            center_message = "Press any key to return to game";
        else if( PlayerShip.get_disabled() )
            center_message = "Fire to relaunch!";
        else
            center_message = "";
    }

    void redraw_screen();

    static Uint32 timer_tick( Uint32 interval, void* param );

protected:
    bool is_play_paused() const
    {
        return is_game_over() || quit_request || pause_request;
    }

    bool is_game_over() const
    {
        return game_over;
    }

    bool can_control_ship()
    {
        return PlayerShip.get_disabled() || is_play_paused();
    }

    void keystate_actions();

    void play_timer();

private:
    SDL_TimerID timer;
    Uint8 *keystatebuffer;

    std::string scrolling_message;
    int scrolling_message_pos;

    std::string center_message;

    int keylock_timer;

    float backdrop_alpha;

    bool game_over;

    bool quit_request;

    bool pause_request;

    bool death_displayed;

    int shipInvasion;

    int chOn;
};

Sasteroids::Sasteroids()
{
    timer = SDL_AddTimer( GAME_CLOCK, &Sasteroids::timer_tick, 0 );
    if( !timer )
    {
        throw std::runtime_error( "Unable to create SDL timer" );
    }

    keystatebuffer = SDL_GetKeyState( NULL );
    Mix_ExpireChannel( 0, 0 );

    PlayerShip.Reset();
    PlayerShip.setships( 3 );
    PlayerShip.reset_score();

    backdrop_alpha = 0.0;
    death_displayed = false;
    game_over = false;
    shipInvasion = 0;
    chOn = 0;
    keylock_timer = 0;
    pause_request = false;
    quit_request = false;
}

Sasteroids::~Sasteroids()
{
    SDL_RemoveTimer( timer );
    Mix_ExpireChannel( 0, 0 ); // Always kill jet sound.
}

Uint32 Sasteroids::timer_tick( Uint32 interval, void* param )
{
    SDL_Event event;

    if( FinishedLastCall )
    {
        FinishedLastCall = 0;
        event.type = SDL_USEREVENT;
        SDL_PushEvent( &event );
    }
    else if( !FinishedLastCall )
    {
        //#ifdef PRINT_DIAG
        cerr << "[Diag] Dropped Frame!" << endl;
        //#endif
    }

    return GAME_CLOCK;
}

void Sasteroids::redraw_screen()
{
    UserInterfaceManager::predraw();

    // Paint the backdrop
    if( BackdropOn && UserInterfaceManager::WIDTH() <= 640
            && UserInterfaceManager::HEIGHT() <= 400 )
    {
        Backdrops[Glevel % NUM_BACKS].draw_alpha( 0,
                UserInterfaceManager::HEIGHT() );

        if( backdrop_alpha > 0.0f )
        {
            Backdrops[(Glevel - 1) % NUM_BACKS].draw_alpha( 0,
                    UserInterfaceManager::HEIGHT(), 0.0f, backdrop_alpha );
            backdrop_alpha -= 0.05f;
        }
    }
    else
    {
        UserInterfaceManager::clearscreen();
    }

    // Draw all the screen objects
    m_manager_ptr->draw_objects();

    // Draw the bottom line
    botline();

    // Draw the player's ship - and restart message
    if( PlayerShip.get_disabled() && !is_game_over() )
    {
        int x = int( ScreenLimits.GetX() );
        int y = int( ScreenLimits.GetY() );
        x = x / 2;
        y = y / 2;

        if( PlayerShip.ships() >= 0 )
        {
            Uint32 seconds = (SDL_GetTicks() / 1000);
            if( (seconds % 2) > 0 )
            {
                bmp_player.draw( x, y );
            }
        }
    }

    // Show active messages
    if( scrolling_message.size() > 0 && scrolling_message_pos > 0 )
    {
        UserInterfaceManager::CenterXText( scrolling_message_pos,
                scrolling_message );
    }

    reset_center_message();
    if( center_message.size() > 0 )
    {
        UserInterfaceManager::CenterText( center_message );
    }

    UserInterfaceManager::updateScreen();
}

void Sasteroids::run()
{
    char pstr[256];
    int pause = 0;
    int dead = 0;
    int aegg = 0;
    float alpha_anim = -1.0f;
    SDL_Event event;
    bool need_redraw;

    // get starting skill level, <DELETED>
    Glevel = 0;

    numasts = 0;
    score = 0;
    oldscore = 0;
    dead = 0;

    PlayerShip.setships( 3 );
    score = 0;
    oldscore = -1;

    PlayerShip.Reset();
    m_manager_ptr->free_objects();
    UserInterfaceManager::clearscreen();

    bool end_game = false;
    bool key_consumed = false;

    FinishedLastCall = 1;
    gMsgTimeLeft = 0;
    chOn = 1;
    shipInvasion = 0;

    while( !end_game )
    {
        while( SDL_PollEvent( &event ) )
        {
            keystate_actions();
            if( need_redraw )
            {
                redraw_screen();
                need_redraw = false;
            }

            switch( event.type )
            {
            case SDL_VIDEORESIZE:
                ScreenLimits.SetXY( (event.resize.w), (event.resize.h) );
                UserInterfaceManager::resync( event.resize.w, event.resize.h );
                need_redraw = true;
                break;

            case SDL_KEYDOWN:
                need_redraw = true;
                key_consumed = false;

                if( is_game_over() )
                {
                    if( keylock_timer <= 0 )
                    {
                        end_game = true;
                        key_consumed = true;
                    }
                }
                else if( quit_request )
                {
                    // Displaying quit menu
                    if( event.key.keysym.sym == SDLK_y )
                    {
                        end_game = true;
                        key_consumed = true;
                    }
                }
                else if( !pause_request && !PlayerShip.get_disabled() )
                {
                    // We're playing the game
                    if( event.key.keysym.sym == SDLK_LALT )
                    {
                        PlaySound( SND_WARP );
                        PlayerShip.Hyper();
                        key_consumed = true;
                    }
                    else if( event.key.keysym.sym == SDLK_RALT )
                    {
                        PlaySound( SND_WARP );
                        PlayerShip.Hyper();
                        key_consumed = true;
                    }
                    else if( event.key.keysym.sym == SDLK_o
                            && (keystatebuffer[SDLK_LSHIFT]
                                    || keystatebuffer[SDLK_RSHIFT]) )
                    {
                        // Sasteroids had a nice little cheat code,
                        // I'm not a fan of cheat codes in general, so do something fun instead.
                        set_scrolling_message(
                                "CHEATER!!! C H E A T E R!!! CHEATER!!!" );
                        key_consumed = true;
                    }
                    else if( event.key.keysym.sym == SDLK_SPACE )
                    {
                        PlayerShip.Fire();
                        key_consumed = true;
                    }
                    else if( event.key.keysym.sym == SDLK_LCTRL )
                    {
                        PlayerShip.Fire();
                        key_consumed = true;
                    }
                    else if( event.key.keysym.sym == SDLK_p )
                    {
                        pause_request = true;
                        key_consumed = true;
                    }
                }
                else if( !pause_request && PlayerShip.ships() > 0 )
                {
                    // We're dead, but we can go again
                    if( (event.key.keysym.sym == SDLK_SPACE ||
                            event.key.keysym.sym == SDLK_s ||
                            event.key.keysym.sym == SDLK_LCTRL) &&
                            keylock_timer <= 0
                            )
                    {
                        PlayerShip.Reset();
                        key_consumed = true;
                        death_displayed = false;
                    }
                }
                else
                {
                    std::cout << "Mkay?" << PlayerShip.ships() << std::endl;
                }

                // Generic Actions -> Happen in any mode
                // Leave this cheat code in, nice for debugging.
                if( !key_consumed && event.key.keysym.sym == SDLK_n
                        && (keystatebuffer[SDLK_LSHIFT]
                                || keystatebuffer[SDLK_RSHIFT]) )
                {
                    Glevel++;
                    alpha_anim = 1.0f;
                    key_consumed = true;
                }

                if( !key_consumed && event.key.keysym.sym == SDLK_f )
                {
                    UserInterfaceManager::FullScreen();
                    key_consumed = true;
                }

                if( (!key_consumed || pause_request)
                        && event.key.keysym.sym == SDLK_q )
                {
                    quit_request = true;
                    pause_request = false;
                    key_consumed = true;
                }

                if( event.key.keysym.sym == SDLK_PLUS
                        || event.key.keysym.sym == SDLK_EQUALS )
                {
                    if( MainVolume < 128 )
                        MainVolume += 8;
                    if( MainVolume > 128 )
                        MainVolume = 128;
                    DispVolume = 64;
                    Mix_Volume( -1, MainVolume );
                    key_consumed = true;
                }

                if( event.key.keysym.sym == SDLK_MINUS )
                {
                    if( MainVolume > 0 )
                        MainVolume -= 8;
                    if( MainVolume < 0 )
                        MainVolume = 0;
                    DispVolume = 64;
                    Mix_Volume( -1, MainVolume );
                    key_consumed = true;
                }

                if( (pause_request || quit_request) && !key_consumed )
                {
                    pause_request = false;
                    quit_request = false;
                }

                break;

            case SDL_QUIT:
                need_redraw = true;
                exit( 0 );
                break;

            case SDL_USEREVENT:
                need_redraw = true;
                play_timer();
                break;
            }
        }
    }
}

void Sasteroids::play_timer()
{
    scrolling_message_pos -= 5;

    if( keylock_timer > 0 )
    {
        keylock_timer--;
    }

    // Everything past this point is game play
    if( is_play_paused() )
    {
        FinishedLastCall = 1;
        return;
    }

    // This only gets called while the GAME CLOCK ITSELF is ticking =)
    //deathTimer--;
    //				if (deathTimer < -128)
//						deathTimer = -1;
    //if (finalDead && finalDead > 2)
//						finalDead--;

    //-------------------------------------------
    // Move level as applicable
    if( numasts <= 0 )
    {
        char tempString[256];
        Glevel++;

        std::ostringstream ss;
        ss << "Entering Level " << Glevel;
        std::string st = ss.str();
        set_scrolling_message( st );

        numasts = 0;
        GenerateAsteroids( *m_manager_ptr );
        PlayerShip.addMaxPower( 2 );
        PlayerShip.addRegPower( 1 );
        if( Glevel != 1 )
            backdrop_alpha = 1.0f;
    }

    //---------------------------------------------
    // Generate Aliens
    if( !(rand() % LevelOdds( 32, 4050, 1 ))
            || (shipInvasion && !(rand() % 50)) )
    {
        if( Glevel > 6 && !(rand() % 15) && !shipInvasion )
            shipInvasion = 10;
        if( shipInvasion )
            shipInvasion--;
        if( shipInvasion < 0 )
            shipInvasion = 0;
        int j;
        m_manager_ptr->register_object( new Alien );
        PlaySound( SND_ENEMY );
    }

    //-----------------------------------------------
    // Generate powerups

    // Create a power up :)
    if( !(rand() % /* 250 */25) && !ClassicMode )
    {
        m_manager_ptr->register_object( new PowerUp );
    }

    m_manager_ptr->move_game_time( 33 );

    if( PlayerShip.get_disabled() && !death_displayed )
    {
        shipInvasion = 0;
        death_displayed = true;
        keylock_timer = 10;

        if( PlayerShip.ships() <= 0 )
        {
            set_scrolling_message( "Game Over!" );
            game_over = true;
        }

        Mix_ExpireChannel( 0, 10 );
    }

    FinishedLastCall = 1;
}

void Sasteroids::keystate_actions()
{
    if( !PlayerShip.get_disabled() )
    {
        // Rotation
        if( keystatebuffer[SDLK_RIGHT] )
            PlayerShip.set_angular_velocity( -10 );
        else if( keystatebuffer[SDLK_LEFT] )
            PlayerShip.set_angular_velocity( 10 );
        else
            PlayerShip.set_angular_velocity( 0 );

        // Thrust
        if( keystatebuffer[SDLK_UP] )
        {
            PlayerShip.Thrust( 0.15f );
            if( !chOn )
            {
                Mix_PlayChannel( 0, soundSamples[SND_ENGINE], -1 );
                chOn = 1;
            }
        }
        else if( chOn )
        {
            Mix_ExpireChannel( 0, 10 );
            chOn = 0;
        }

        // Shield
        if( keystatebuffer[SDLK_DOWN] )
            PlayerShip.shieldOn();
        else
            PlayerShip.shieldOff();

        // Brake
        if( keystatebuffer[SDLK_b] )
            PlayerShip.Brake();
    }
}

////////////////////////////////////////////////////////////////////////////
// return 1=easy, 4=medium, 8=hard
int selectGame()
{
    // Ui::drawToPhysical();
    displayScreen( "back.raw" );
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
    displayScreen( "graphics/back.bmp" ); // TODO, rewrite this screen
}

/////////////////////////////////////////////////////////
// Initialize the System 
void InitializeSDL()
{
    // Init SDL Video:
    {
        if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
        {
            cerr << "Couldn't initialize video!" << endl;
            exit( -1 );
        }
        atexit( SDL_Quit );
    }

    // Init SDL Audio:
    {
        if( SDL_Init( SDL_INIT_AUDIO ) < 0 )
        {
            cerr << "Couldn't initialize audio!" << endl;
            exit( -1 );
        }

        atexit( SDL_Quit );
    }

    // Init SDL Timer:
    {
        if( SDL_Init( SDL_INIT_TIMER ) < 0 )
        {
            cerr << "Could'nt initialize timer!" << endl;
            exit( -1 );
        }

        atexit( SDL_Quit );
    }

    // Init joystick:
    {
        use_joystick = 0;
        num_joysticks = 0;

#ifdef HAVE_JOYSTICK
        use_joystick = 1;

        if( SDL_Init( SDL_INIT_JOYSTICK ) < 0 )
        {
            cerr << "Couldn't initialize joystick." << endl;

            use_joystick = 0;
        }
        else
        {
            /* Look for joysticks: */

            num_joysticks = SDL_NumJoysticks();

            if( num_joysticks <= 0 )
            {
                cerr << "No joysticks available." << endl;

                use_joystick = 0;
            }
            else
            {
                /* Open joystick: */

                js = SDL_JoystickOpen( 0 );

                if( js == NULL )
                {
                    cerr << "Couldn't open joystick 1." << endl;

                    use_joystick = 0;
                }
            }
        }
#endif
    }

    atexit( SDL_Quit );
}

/////////////////////////////////////////////////////////
// Handle Command Line Parameters
void HandleCommandLine( int argc, char** argv )
{
    int i;

    for( i = 0; i < argc; i++ )
    {
        if( argv[i] )
        {
            if( strcmp( argv[i], "-fullscreen" ) == 0 )
                wantFullScreen = 1;
            if( strcmp( argv[i], "-classic" ) == 0 )
                ClassicMode = 1;
        }
    }
}

///////////////////////////////////////////////////////////
// Actions for the main menu

void main_menu_quit( int* p_done )
{
    *p_done = 1;
}

void main_menu_play( int* p_done )
{
    Sasteroids game_object;
    game_object.run();
}

void main_menu_information( int* done )
{
    const char *information_cstr[14] =
                {
                        "SDL Sasteroids Version " VERSION,
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
                };

    std::vector< std::string > information( &information_cstr[0],
            &information_cstr[14] );
    ScreenBitmap mouse( "graphics/mouse.png" );
    GraphicsMenu information_menu( &mouse, &titleScreen, information );

    /* Run the Main Menu */

    information_menu.setAction( 13, main_menu_quit );
    information_menu.enableSound( SND_FIRE );
    information_menu.setTopPad( 26.0f );
    information_menu.setMinSelectable( 13 );
    information_menu.run_menu();
}

void main_menu_credits( int *done )
{
    const char *credits_cstr[11] =
                {
                        "SDL Sasteroids Version " VERSION, // 0
                        " ", // 1
                        " ", // 2
                        "(C)opyright 1991 - 2003", // 3
                        "Contributers:", // 4
                        "Andrew Mulbrook, Brad Pitzel, Digisin", // 5
                        " ", // 6
                        "Backdrops:", // 7
                        "From Nasa Image Collection", // 8
                        " ", // 9
                        "Return to Game", // 10
            };

    std::vector< std::string > credits( &credits_cstr[0], &credits_cstr[11] );

    ScreenBitmap mouse( "graphics/mouse.png" );
    GraphicsMenu credits_menu( &mouse, &titleScreen, credits );

    credits_menu.setAction( 10, main_menu_quit );

    credits_menu.enableSound( SND_FIRE );
    credits_menu.setTopPad( 26.0f );
    credits_menu.setMinSelectable( 10 );
    credits_menu.run_menu();
}

/////////////////////////////////////////////////////////
// Main Program Starts Here 
// Starts all the tasks, etc...
int main( int argc, char *argv[] )
{
    const char *main_menu_cstr[5] =
                { "SDL Sasteroids Version " VERSION,
                        "INFORMATION",
                        "CREDITS",
                        "START GAME",
                        "QUIT"
                };
    std::vector< std::string > main_menu_strings( &main_menu_cstr[0],
            &main_menu_cstr[5] );

    srand( (unsigned int) time( NULL ) );

    chdir( GAMEDIR );

    InitializeSDL();
    HandleCommandLine( argc, argv );
    InitializeHiScores();

    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 1024 ) == -1 )
    {
        throw "Couldn't initialize sound";
    }

    Mix_AllocateChannels( 16 );
    Mix_ReserveChannels( 1 ); // for jet engine!
    Mix_Volume( -1, MainVolume );

    UserInterfaceManager::init();

    PlayingField m_manager;
    m_manager.set_field_size( Vector( 640.0, 480.0 ) );
    m_manager_ptr = &m_manager;

    LoadBitmaps();
    LoadWavs();

    ScreenBitmap mouse( "graphics/mouse.png" );
    GraphicsMenu main_menu( &mouse, &titleScreen, main_menu_strings );

    /* Run the Main Menu */

    main_menu.setAction( 1, main_menu_information );
    main_menu.setAction( 2, main_menu_credits );
    main_menu.setAction( 3, main_menu_play );
    main_menu.setAction( 4, main_menu_quit );

    main_menu.enableSound( SND_FIRE );
    main_menu.setTopPad( 26.0f );
    main_menu.setMinSelectable( 1 );
    main_menu.run_menu();

    Mix_CloseAudio();
    SDL_JoystickClose( 0 );

    // shutdown & restore text mode
    UserInterfaceManager::restore();

    return 0;
}
