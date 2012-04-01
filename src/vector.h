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

#include <cmath>

#ifndef VECTOR_LIB_
#define VECTOR_LIB_

// Vector 2d:
template< typename Ftype >
class Vector2D
{
public:
    Vector2D( const Ftype& x, const Ftype& y ) :
            m_x( x ), m_y( y )
    {
        m_x = x;
        m_y = y;
    }

    Vector2D( const Ftype a[2] ) :
            m_x( a[0] ), m_y( a[1] )
    {
    }

    Vector2D() :
            m_x( 0.0 ), m_y( 0.0 )
    {
    }

    Vector2D( const Vector2D< Ftype >& copy ) :
            m_x( copy.m_x ), m_y( copy.m_y )
    {
    }

    void SetXY( const Ftype& x, const Ftype& y )
    {
        m_x = x;
        m_y = y;
    }

    void SetXY( const Ftype a[2] )
    {
        m_x = a[0];
        m_y = a[1];
    }

    void AddXY( const Ftype& x, const Ftype& y )
    {
        m_x += x;
        m_y += y;
    }

    void AddXY( const Ftype a[2] )
    {
        m_x += a[0];
        m_y += a[1];
    }

    inline const Ftype& GetX() const
    {
        return m_x;
    }

    inline const Ftype& GetY() const
    {
        return m_y;
    }

    Ftype lengthSqrd() const
    {
        return m_x * m_x + m_y * m_y;
    }

    Ftype length() const
    {
        return sqrt( m_x * m_x + m_y * m_y );
    }

    Ftype length_squared( const Vector2D< Ftype >& p_rh ) const
            {
        double a = m_x - p_rh.GetX();
        double b = m_y - p_rh.GetY();
        return float( a * a + b * b );
    }

    Ftype length( const Vector2D< Ftype >& p_rh ) const
            {
        return std::sqrt( length_squared( p_rh ) );
    }

    Vector2D< Ftype >& operator=( const Vector2D< Ftype >& rh )
    {
        m_x = rh.m_x;
        m_y = rh.m_y;
        return *this;
    }

    Vector2D< Ftype > operator%( const Vector2D< Ftype >& rh ) const
            {
        Vector2D< Ftype > temp;
        temp.m_x = m_x * rh.m_x - m_y * rh.m_x;
        temp.m_y = m_y * rh.m_x - m_x * rh.m_y;
        return temp;
    }

    Vector2D< Ftype > operator+( const Vector2D< Ftype >& rh ) const
            {
        Vector2D< Ftype > temp;
        temp.m_x = m_x + rh.m_x;
        temp.m_y = m_y + rh.m_y;
        return temp;
    }

    Vector2D< Ftype > operator-( const Vector2D< Ftype >& rh ) const
            {
        Vector2D< Ftype > temp;
        temp.m_x = m_x - rh.m_x;
        temp.m_y = m_y - rh.m_y;
        return temp;
    }

    Ftype operator*( const Vector2D< Ftype >& rh ) const
            {
        return rh.m_x * m_x + rh.m_y * m_y;
    }

    Vector2D< Ftype > operator*( const Ftype& rh ) const
            {
        Vector2D< Ftype > a;
        a.m_x = m_x * rh;
        a.m_y = m_y * rh;
        return a;
    }

    Vector2D< Ftype > operator/( const Ftype& rh ) const
            {
        Vector2D< Ftype > a;
        a.m_x = m_x / rh;
        a.m_y = m_y / rh;
        return a;
    }

    const Vector2D< Ftype >& operator+=( const Vector2D< Ftype >& rh )
    {
        m_x = m_x + rh.m_x;
        m_y = m_y + rh.m_y;
        return *this;
    }

    const Vector2D< Ftype >& operator-=( const Vector2D< Ftype >& rh )
    {
        m_x = m_x - rh.m_x;
        m_y = m_y - rh.m_y;
        return *this;
    }

    const Vector2D< Ftype >& operator*=( const float& rh )
    {
        m_x = m_x * rh;
        m_y = m_y * rh;
        return *this;
    }

    const Vector2D< Ftype >& operator /=( const float& rh )
    {
        m_x = m_x / rh;
        m_y = m_y / rh;
        return *this;
    }

    bool operator==( const Vector2D< Ftype >& rh ) const
            {
        return m_x == rh.m_x && m_y == rh.m_y;
    }

    bool operator!=( const Vector2D< Ftype >& rh ) const
            {
        return !(operator==( rh ));
    }

    bool operator<( const Vector2D< Ftype >& rh ) const
            {
        return length() < rh.length();
    }

    bool operator<=( const Vector2D< Ftype >& rh ) const
            {
        return length() <= rh.length();
    }

    bool operator>( const Vector2D< Ftype >& rh ) const
            {
        return length() > rh.length();
    }

    bool operator>=( const Vector2D< Ftype >& rh ) const
            {
        return length() >= rh.length();
    }

    template< typename F >
    friend const Vector2D< F > operator*( const F& x, const Vector2D< F >& rh );

private:
    Ftype m_x;
    Ftype m_y;
};

template< typename Ftype >
const Vector2D< Ftype > operator*( const Ftype& x, const Vector2D< Ftype >& rh )
{
    Vector2D< Ftype > temp;
    temp.m_x = rh.m_x * x;
    temp.m_y = rh.m_y * x;
    return temp;
}

typedef Vector2D< float > Vector;

#endif
