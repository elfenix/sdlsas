#include "sasteroids.h"

/**********************************************************/
/* Static Functions */
/**********************************************************/


void GameEntity::set_limits(float x, float y) 
{ 
  ScreenLimits.SetXY(x,y); 
}


/**********************************************************/
/* Constructors/Destructors */
/**********************************************************/

void GameEntity::initialize(int type) 
{
  wrapMoves = false;
  isAlive = false;
  position.SetXY(0, 0);
  velocity.SetXY(0, 0);
  accelleration.SetXY(0, 0);
  m_sprite = 0;
  maxspeed = 64.0f;
  objtype = type;
  mysize = 1;  
  angle = 0;
  m_angular_velocity = 0.0;
  m_field = 0;
}


GameEntity::GameEntity()
{
  initialize(0);
}


GameEntity::GameEntity(int type)
{
  initialize(type);
}


GameEntity::~GameEntity()
{
  // not really anything to do ....
}



/**********************************************************/
/* It's a matter of life and death */
/**********************************************************/

bool GameEntity::alive() {
  return isAlive;
}

void GameEntity::die() 
{
  isAlive = false;
}

void GameEntity::restore()
{
  isAlive = true;
}



/**********************************************************/
/* Our more virtual members */
/**********************************************************/

void GameEntity::draw()
{
  if (isAlive)
    if(m_sprite)
      m_sprite->draw(position.GetX(), position.GetY(), 1.0f, angle);
}


void GameEntity::tick()
{
    float tx, ty;
    float vlimit;

    // Good enough for a game..
    position += velocity;
    
    vlimit = velocity.lengthSqrd();

    if (vlimit > maxspeed) {
      velocity /= sqrt(vlimit);
      velocity *= sqrt(maxspeed);
    }


    angle = angle + m_angular_velocity;
    while(angle > 360.0f) angle -= 360.0f;
    while(angle < 0.0f) angle += 360.0f;
    


    velocity += accelleration;
    
    // Look to see if we are visible...
    if (position.GetX() - (GetWidth() / 2.0f)  > ScreenLimits.GetX() ||
	position.GetY() - (GetHeight() / 2.0f) > ScreenLimits.GetY()) {
	if (!wrapMoves)
	    die();		// If we don't wrap, we die...
	else {
	    tx = position.GetX();
	    ty = position.GetY();
	    if (tx > ScreenLimits.GetX())
		tx = 0.1f-(size.GetX()/2.0f);
	    if (ty > ScreenLimits.GetY())
		ty = 0.1f-(size.GetY()/2.0f);
	    position.SetXY(tx, ty);
	}
    }


    if (position.GetX() - (GetWidth()/2.0f) < (0.0f) || 
	position.GetY() - (GetWidth()/2.0f) < (0.0f)) {
	if (!wrapMoves)
	    die();
	else {
	    tx = position.GetX();
	    ty = position.GetY();

	    if (tx < (1.0f - size.GetX()))
		tx = ScreenLimits.GetX() - 0.1f + (GetWidth()/2.0f);

	    if (ty < (1.0f - size.GetY()))
		ty = ScreenLimits.GetY() - 0.1f + (GetHeight()/2.0f);

	    position.SetXY(tx, ty);
	}
    }
}



/**********************************************************/
/* Bounce! */
/**********************************************************/

int GameEntity::bounceStat() const
{
  return bounce;
}

void GameEntity::setBounceStat(int i) {
  bounce = i;
}

void GameEntity::setbounce()
{
  setBounceStat(15);
}



/**********************************************************/
/* Let's Move! */
/**********************************************************/

void GameEntity::randomSpeed(float start, float stop)
{
  int myMod, rRet;
  float ttmp, ttmp2;
  myMod = int((stop - start) * 100.0f);
  if(myMod < 0) myMod = -myMod;
  rRet = rand() % myMod;
  ttmp = float(rRet) / (100.0f);
  ttmp2 = velocity.length();
  ttmp2 = 1 / ttmp2;
  velocity = velocity * ttmp;
  velocity = velocity * ttmp2;
}


void GameEntity::randomDir(float magn)
{
  int anglef = rand() % 255;
  velocity.SetXY(f_math::sin(anglef)*magn, 
		 f_math::cos(anglef)*magn);
}


void GameEntity::trim()
{
  float vlimit = velocity.lengthSqrd();

  if (vlimit > maxspeed) {
    velocity /= sqrt(vlimit);
    velocity *= sqrt(maxspeed);
  }
  
  if(isnan(velocity.length())) {
    velocity.SetXY(0.3f, 0.3f); // just so things don't really feck up.
  }
  
  if(isnan(angle)) angle = 0;
}


/**********************************************************/
/* Rotate dat thang */
/**********************************************************/

void GameEntity::rotateLeft() 
{
  addAngle(10.0f);
}

void GameEntity::rotateRight() 
{
  addAngle(-10.0f);
}

void GameEntity::addAngle(float a) 
{
  angle += a;
  if(angle < 0) angle += 360.0f;
  if(angle > 360.0f) angle -= 360.0f;
}

float GameEntity::getAnglef() const
{
  return angle;
}

int GameEntity::getAngle() const
{
  float z;
  int a;

  z = ((angle) * 256.0f / 356.0f);
  a = int(z) & 0x0ff;
  return a;
}

void GameEntity::set_angular_velocity(float a) 
{
  m_angular_velocity = a;
}


/**********************************************************/
/* Set us up the bomb! */
/**********************************************************/


void GameEntity::set_bitmap(ScreenBitmap* sprite)
{
  m_sprite = sprite;
  myCenXRef = float(sprite->width())/2.0f; 
  myCenYRef = float(sprite->height())/2.0f; 
}


void GameEntity::set_wrap_moves(bool a) 
{ 
  wrapMoves = a; 
}
  
  
void GameEntity::SetSize(int x, int y)
{ 
  size.SetXY((float)x,(float)y); 
}
  
void GameEntity::SetVel(float x, float y) 
{ 
  velocity.SetXY(x,y); 
}

void GameEntity::set_acceleration(float x, float y)
{
  accelleration.SetXY(x,y); 
}
  
void GameEntity::SetXY(float x, float y)
{ 
  position.SetXY(x,y); 
}

void GameEntity::add_velocity(float x, float y)
{ 
  velocity.SetXY(VelX()+x,VelY()+y); 
}


void GameEntity::settype(int a) 
{ 
  objtype = a; 
}
  
void GameEntity::set_size(int i) 
{ 
  mysize = i; 
}

void GameEntity::SetMaxSpeed(float a) 
{ 
  maxspeed = a*a; 
}
  
void GameEntity::set_mass(float a)
{
  mass = a;
}

void GameEntity::set_velocity(const Vector& nVelocity)
{
  velocity = nVelocity;
}

void GameEntity::set_pos(const Vector& nXY)
{
  position = nXY;
}



/**********************************************************/
/* We have signal! */
/**********************************************************/

float GameEntity::GetX() const
{ 
  return position.GetX(); 
}

float GameEntity::GetY() const
{ 
  return position.GetY(); 
}

float GameEntity::GetWidth() const
{ 
  return (float)m_sprite->width(); 
}
  
float GameEntity::GetHeight() const
{ 
  return (float)m_sprite->height(); 
}

const Vector& GameEntity::get_pos() const
{ 
  return position; 
}

const Vector& GameEntity::get_velocity() const
{ 
  return velocity; 
}
  
GameEntity::ObjectType GameEntity::type() const
{
  return ObjectType( objtype );
}
  
  
int GameEntity::Size() const
{ 
  return mysize; 
}
  
float GameEntity::VelX() const
{ 
  return velocity.GetX(); 
}

float GameEntity::VelY() const
{ 
  return velocity.GetY(); 
}
  

float GameEntity::getMass() const
{
  return mass;
}


