// Vector Class
// (C)opyright Andrew M. 2002 
// This is a simple class, if you want to use it, use it. I don't care.
// (eg: consider THIS CLASS/FILE ONLY in the public domain)

// Vector class.... Does all of the grunt work for vectors.
//
// Vector:
// 	Constructors 
// 		Vector(float, float) - create new vector with x, y values
// 		Vector(float[2])     - same
// 		Vector()	       - vector with x & y set to 0
// 		Vector(Vector)     - copy
// 	Functions
// 		float length()	       - length of vector from origin
// 		float length(Vector)  - length between vectors
// 		float lengthSqrd       - length of the vector squared(faster)
// 		GetX() 		       - get X component
// 		GetY()		       - get Y component
// 		SetXY(float, float)    - set X and Y components	
// 		SetXY(float[])	       - set X and Y components(again)
// 		AddXY(float, float)    - add X and Y values
// 		AddXY(float[])	       - add X and Y values
//  	Operators
//  		= 		       - regular = operator
//  		<, >, <=, >=	       - test length of vector A vs. B
//  		==, !=		       - see if A is equal to B
//  		*		       - dot product(returns float)
//  		%		       - cross product of A and B
//  		+=, -=		       - addition and subtraction
//              *=, /=                 - multiplaction/division of scalars
//  		* float, float *       - scalar multiply
//
//  	Everything is done using inline and refs for speed. This class is
//  	used in some speed critical portions of code. 
//
//
//
// Bugs:
// 	In math, the cross and dot product really should be different 
// 	operators than the standard multiply.  However, since this is a 
// 	computer, there really isn't a "dot" or "cross" operator that works 
// 	well.
//
// 	I used the * as the dot product operator, and % as the cross product. 
// 	(% looks like an "x" right?)
// 	
// 	The scalar multiple is also represented using a *. This creates a 
// 	bad side effect. For example:
//
// 	vector4 = vector1 * vector2 * vector3;
//
// 	This will result in vector 3 being scalar multiplied by the dot 
// 	product of vectors 1 and 2. Probably not what you wanted. 
//
// 	A triple scalar product must be represented using ()'s
// 	
// 	floatA = vector1 * (vector2 % vector3);
//
// 	If your doing triple scalar products in 3 dimensions, the 
// 	TripleScalar function is probably more usefull though. (It's faster 
// 	than performing a normal triple scalar product using operators).
//
// 	This library using floating points, so it's not very accurate. 
// 	(Good for game math though.)


#ifndef __VECTOR_LIB__

// Sense everything is inline, we make sure that different versions
// of this header aren't mixed up in binaries by using a Macro. (I 
// _think_ this will work....)

#define Vector Vector__01

// Vector 2d:
class Vector {
 public:
  inline Vector(float x, float y) 
    { myX = x; myY = y; }
  
  inline Vector(float a[2]) 
    { myX = a[0]; myY = a[1]; }
  
  inline Vector() 
    { myX = 0.0f; myY = 0.0f; }
  
  inline Vector(const Vector& copy)
    { myX = copy.myX; myY = copy.myY; }
  
  inline void SetXY(const float& x, const float& y)
    { myX = x; myY = y; }
  
  inline void SetXY(const float arr[2])
    { myX = arr[0]; myY = arr[1]; }
  
  inline void AddXY(const float& x, const float& y)
    { myX += x; myY += y; }
  
  inline void AddXY(const float arr[2])
    { myX += arr[0]; myY += arr[1]; }
				
  inline const float& GetX() const { return myX; };
  inline const float& GetY() const { return myY; };
  
  inline float lengthSqrd() const
    {
      return myX*myX+myY*myY;
    }

  inline float length() const 
    { 
      return sqrt(myX*myX+myY*myY);
    }
  
  inline float length(const Vector& rh) const
    {	
      float a, b;
      a = myX - rh.myX;
      b = myY - rh.myY;
      return sqrt(a*a+b*b);
    }
  
  inline Vector operator=(const Vector& rh)	// Equal
    {
      myX = rh.myX;
      myY = rh.myY;
      return *this;
    }
  
  inline Vector operator%(const Vector& rh) 
    {
      Vector temp;
      temp.myX = myX*rh.myX - myY*rh.myX;
      temp.myY = myY*rh.myX - myX*rh.myY;
      return temp;
    }
  
  inline Vector operator+(const Vector& rh)
    { 
      Vector temp; 
      temp.myX=myX+rh.myX; temp.myY=myY+rh.myY; 
      return temp;
    }
  
  inline Vector operator-(const Vector& rh)
    {
      Vector temp;
      temp.myX=myX-rh.myX; temp.myY=myY-rh.myY;
      return temp;
    }
  
  inline float operator*(const Vector& rh)
    {
      return rh.myX * myX + rh.myY * myY;
    }
  
  inline Vector operator*(const float& rh)  
    {
      Vector a;
      a.myX = myX * rh;
      a.myY = myY * rh;
      return a;
    }

  inline Vector operator/(const float& rh)
    {
      Vector a;
      a.myX = myX / rh;
      a.myY = myY / rh;
      return a;
    }
  
  inline const Vector& operator+=(const Vector& rh)	
    {	
      myX = myX + rh.myX;
      myY = myY + rh.myY;
      return *this;
    }
  
  inline const Vector& operator-=(const Vector& rh) 
    {
      myX = myX - rh.myX;
      myY = myY - rh.myY;
      return *this;
    }
  
  inline const Vector& operator*=(const float& rh)
    {
      myX = myX * rh;
      myY = myY * rh;
      return *this;
    }

  inline const Vector& operator /=(const float& rh)
    {
      myX = myX / rh;
      myY = myY / rh;
      return *this;
    }
      
  inline bool operator==(const Vector& rh) const
    {
      return myX == rh.myX && myY == rh.myY;
    }
  
  inline bool operator!=(const Vector& rh) const
    {
      return !(operator==(rh));
    }		
  
  inline bool operator<(const Vector& rh) const
    {
      return length() < rh.length();
    }
  
  inline bool operator<=(const Vector& rh) const
    {
      return length() <= rh.length();
    }
  
  inline bool operator>(const Vector& rh) const
    {
      return length() > rh.length();
    }
  
  inline bool operator>=(const Vector& rh) const
    {
      return length() >= rh.length();
    }
  
  friend const Vector operator*(float x, const Vector& rh);
  
 private:
  float myX;
  float myY;
};


inline const Vector operator*(float x, const Vector& rh) 
{
  Vector temp;
  temp.myX = rh.myX * x;
  temp.myY = rh.myY * x;
  return temp;
}


#endif
