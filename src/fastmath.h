// Copyright 1994 Brad Pitzel
// Modification Copyright Andrew M.
//
// Feel free to use/distribute/modify as long as credit/copyrights for myself 
// are included.

// File   : FastMath.h[1.0]
// Changed: FRI OCT-10-2002

#ifndef __FastMath__
#define __FastMath__

#include "sasteroids.h"

// Size of trig table - should be power of 2
const int TRIGSIZE = 256;

// change char to match size of trig table (but always use 'unsigned' )
typedef unsigned char Angle;

class FastMath {
	public:
		inline static double cos(Angle d) { 
			return Vcos[d];
		}

		inline static double sin(Angle d) {
			return Vsin[d];
		}
	
		static void init(int verbose=0);
    private:
    	static double Vcos[TRIGSIZE], Vsin[TRIGSIZE];
};

#endif
