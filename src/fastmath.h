/*
 * Copyright (C) 2012 Andrew Mulbrook
 * Copyright 1994 Brad Pitzel
 *
 * Feel free to use/distribute/modify as long as credit/copyrights for myself
 * are included.
 */

#ifndef FASTMATH_H_
#define FASTMATH_H_

class _f_math_priv;

class f_math
{
public:
	typedef unsigned char angle;
	enum { TRIGSIZE = 256 };

public:
	f_math();

	static inline int pow2( int i )
	{
		return 1 << i;
	}

	static inline unsigned int next_pow2( unsigned int v )
	{
		v--;
		v |= v >> 1;
		v |= v >> 2;
		v |= v >> 4;
		v |= v >> 8;
		v |= v >> 16;
		v++;

		return v;
	}

	static inline double cos( angle d )
	{
		return m_cos[d];
	}

	static inline double sin( angle d )
	{
		return m_sin[d];
	}

private:
	static void init_tables();
	
private:
	static double m_cos[ TRIGSIZE ];
	static double m_sin[ TRIGSIZE ];

	friend class _f_math_priv;
};


#endif
