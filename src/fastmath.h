// Copyright 1994 Brad Pitzel
//
// Feel free to use/distribute/modify as long as credit/copyrights for myself 
// are included.

// File   : FastMath.h[1.0]
// Name   : gamelib1.0
// Status : proposed
// Changed: Sun Jun 12 22:19:13 1994

#ifndef __FastMath__
#define __FastMath__

#include "sasteroids.h"

typedef float FixedPoint;

// conversion routines
#define INT2FP(x)		float(x)	
#define FP2INT(x)		int(x)

// Size of trig table - should be power of 2
#define TRIGSIZE 256

// change char to match size of trig table (but always use 'unsigned' )
typedef unsigned char Angle;

class FastMath {
	public:
		static double cos(Angle d) { 
			return Vcos[d];
		}

		static double sin(Angle d) {
			return Vsin[d];
		}
	
		static void init(int verbose=0); //verbose=1, send stuff to stdout

    private:
    	static double Vcos[TRIGSIZE], Vsin[TRIGSIZE];
};

#endif
