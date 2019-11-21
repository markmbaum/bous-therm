//! \file bous_therm_numerics.cc

#include "bous_therm_numerics.h"

BousThermNumerics::BousThermNumerics (const std::string &griddir_, long neq_) :
    BousThermGrid (griddir_),  //inherit grid variables
    OdeTrapz (neq_) {} //inherit ode solving method

double BousThermNumerics::f_linfind (double xa, double ya, double xb, double yb, double y) {
    return( (y - ya)*(xa - xb)/(ya - yb) + xa );
}

double BousThermNumerics::f_aquifer_bottom (double *Tin, double Tsurf) {

    //if the surface is frozen, aquifer bottom is at the surface
    if (Tsurf <= TFREEZE)
        return (0.0);

    //first cell is frozen
    if (Tin[Nz-1] <= TFREEZE)
        return ( f_linfind(zc[Nz-1], Tin[Nz-1], 0.0, Tsurf, TFREEZE) );

    //freezing point is inside a deeper cell
    for (long i=Nz-2; i>=0; i--)
        if (Tin[i] <= TFREEZE)
            return ( f_linfind(zc[i], Tin[i], zc[i+1], Tin[i+1], TFREEZE) );

    //no freezing point found, aquifer is totally thawed
    return (zdepth);
}

long BousThermNumerics::point_inside (double *edges, double pt, long nedge) {

    //point below range
    if (pt < edges[0])
        return(0);
    //point in range
    for (long i=1; i<nedge; i++)
        if (pt <= edges[i])
            return(i-1);
    //point above range
    return(nedge-2);
}
