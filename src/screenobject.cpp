#include "sasteroids.h"

/**********************************************************/
/* Static Functions */
/**********************************************************/


void ScreenObject::SetLimits(float x, float y) 
{ 
  ScreenLimits.SetXY(x,y); 
}


/**********************************************************/
/* Constructors/Destructors */
/**********************************************************/

void ScreenObject::initialize(int type) 
{
  wrapMoves = false;
  isAlive = false;
  position.SetXY(0, 0);
  velocity.SetXY(0, 0);
  accelleration.SetXY(0, 0);
  mysprite = 0;
  maxspeed = 64.0f;
  objtype = type;
  mysize = 1;  
  angle = 0;
  rotVel = 0.0;
}


ScreenObject::ScreenObject()
{
  initialize(0);
}


ScreenObject::ScreenObject(int type)
{
  initialize(type);
}


ScreenObject::~ScreenObject()
{
  // not really anything to do ....
}



/**********************************************************/
/* It's a matter of life and death */
/**********************************************************/

bool ScreenObject::alive() {
  return isAlive;
}

void ScreenObject::die() 
{
  isAlive = false;
}

void ScreenObject::restore()
{
  isAlive = true;
}



/**********************************************************/
/* Our more virtual members */
/**********************************************************/

void ScreenObject::draw()
{
  if (isAlive)
    if(mysprite)
      mysprite->put(position.GetX(), position.GetY(), 1.0f, angle);
}


void ScreenObject::tick()
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


    angle = angle + rotVel;
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
		tx = 0.1f-size.GetX();
	    if (ty > ScreenLimits.GetY())
		ty = 0.1f-size.GetY();
	    position.SetXY(tx, ty);
	}
    }


    if (position.GetX() + (GetWidth()/2.0f) < (0.0f-size.GetX()) || 
	position.GetY() + (GetWidth()/2.0f)< (0.0f-size.GetY())) {
	if (!wrapMoves)
	    die();
	else {
	    tx = position.GetX();
	    ty = position.GetY();

	    if (tx < (1.0f - size.GetX()))
		tx = ScreenLimits.GetX() - 0.1f;

	    if (ty < (1.0f - size.GetY()))
		ty = ScreenLimits.GetY() - 0.1f;

	    position.SetXY(tx, ty);
	}
    }
}



/**********************************************************/
/* Bounce! */
/**********************************************************/

int ScreenObject::bounceStat() const
{
  return bounce;
}

void ScreenObject::setBounceStat(int i) {
  bounce = i;
}

void ScreenObject::setbounce()
{
  setBounceStat(15);
}



/**********************************************************/
/* Let's Move! */
/**********************************************************/

void ScreenObject::randomSpeed(float start, float stop)
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


void ScreenObject::randomDir(float magn)
{
  int anglef = rand() % 255;
  velocity.SetXY(FastMath::sin(anglef)*magn, 
		 FastMath::cos(anglef)*magn);
}


void ScreenObject::trim()
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

void ScreenObject::rotateLeft() 
{
  addAngle(10.0f);
}

void ScreenObject::rotateRight() 
{
  addAngle(-10.0f);
}

void ScreenObject::addAngle(float a) 
{
  angle += a;
  if(angle < 0) angle += 360.0f;
  if(angle > 360.0f) angle -= 360.0f;
}

float ScreenObject::getAnglef() const
{
  return angle;
}

int ScreenObject::getAngle() const
{
  float z;
  int a;

  z = ((angle) * 256.0f / 356.0f);
  a = int(z) & 0x0ff;
  return a;
}

void ScreenObject::setRotVel(float a) 
{
  rotVel = a;
}


/**********************************************************/
/* Set us up the bomb! */
/**********************************************************/


void ScreenObject::SetBitmap(SBitmap* sprite)
{
  mysprite = sprite;
  myCenXRef = float(sprite->width())/2.0f; 
  myCenYRef = float(sprite->height())/2.0f; 
}


void ScreenObject::SetWrapMoves(bool a) 
{ 
  wrapMoves = a; 
}
  
  
void ScreenObject::SetSize(int x, int y)
{ 
  size.SetXY((float)x,(float)y); 
}
  
void ScreenObject::SetVel(float x, float y) 
{ 
  velocity.SetXY(x,y); 
}

void ScreenObject::SetAcc(float x, float y)
{
  accelleration.SetXY(x,y); 
}
  
void ScreenObject::SetXY(float x, float y)
{ 
  position.SetXY(x,y); 
}

void ScreenObject::AddVel(float x, float y)
{ 
  velocity.SetXY(VelX()+x,VelY()+y); 
}


void ScreenObject::settype(int a) 
{ 
  objtype = a; 
}
  
void ScreenObject::setsize(int i) 
{ 
  mysize = i; 
}

void ScreenObject::SetMaxSpeed(float a) 
{ 
  maxspeed = a*a; 
}
  
void ScreenObject::setmass(float a)
{
  mass = a;
}

void ScreenObject::SetVel(const Vector& nVelocity)
{
  velocity = nVelocity;
}

void ScreenObject::SetXY(const Vector& nXY)
{
  position = nXY;
}



/**********************************************************/
/* We have signal! */
/**********************************************************/

float ScreenObject::GetX() const
{ 
  return position.GetX(); 
}

float ScreenObject::GetY() const
{ 
  return position.GetY(); 
}

float ScreenObject::GetWidth() const
{ 
  return (float)mysprite->width(); 
}
  
float ScreenObject::GetHeight() const
{ 
  return (float)mysprite->height(); 
}

const Vector& ScreenObject::GetXYVec() const
{ 
  return position; 
}

const Vector& ScreenObject::GetVel() const
{ 
  return velocity; 
}
  
int ScreenObject::type() const
{ 
  return objtype; 
}
  
  
int ScreenObject::Size() const
{ 
  return mysize; 
}
  
float ScreenObject::VelX() const
{ 
  return velocity.GetX(); 
}

float ScreenObject::VelY() const
{ 
  return velocity.GetY(); 
}
  

float ScreenObject::getMass() const
{
  return mass;
}


