// Copyright 1994 Brad Pitzel
// 
// Feel free to use/distribute/modify as long as credit/copyrights for
// myself 
// are included.

// File : FastMath.c[1.0]
// Name : gamelib1.0
// Status : proposed
// Changed: Sun Jun 12 22:19:12 1994


#include "sasteroids.h"


double FastMath::Vcos[TRIGSIZE];
double FastMath::Vsin[TRIGSIZE];


void FastMath::init(int verbose)
{
  double radian;
  int i;
  
  if (verbose)
    cout << "Building math tables ";
  
  for (i = 0; i < (TRIGSIZE); i++) {
    
    radian = (M_PI / 180.0) * (double) i *(360.0 / (double) TRIGSIZE);
    
    Vcos[i] =::cos(radian);
    Vsin[i] =::sin(radian);
    
    if (verbose)
      if ((i % 40) == 0)
	cout << '.' << flush;
  }
  
  if (verbose)
    cout << endl;
}
