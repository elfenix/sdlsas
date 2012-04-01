/*
 * fastmath.cpp
 * Table lookups of standard mathmatical functions
 *
 * Copyright (C) 2012 Andrew Mulbrook
 * Copyright 1994 Brad Pitzel
 *
 * Feel free to use/distribute/modify as long as credit/copyrights for
 * myself are included.
 */

#include "sasteroids.h"

double f_math::m_cos[TRIGSIZE];
double f_math::m_sin[TRIGSIZE];

// Initialize tables at startup
class _f_math_priv
{
public:
    _f_math_priv()
    {
        f_math::init_tables();
    }
};

static _f_math_priv initialize_tables;

// Routine to build tables
void f_math::init_tables()
{
    double radian;
    int i;

    for( int i = 0; i < TRIGSIZE; i++ )
    {
        radian = (M_PI / 180.0) * (double) i * (360.0 / (double) TRIGSIZE);

        m_cos[i] = std::cos( radian );
        m_sin[i] = std::sin( radian );
    }
}
