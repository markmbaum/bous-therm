//! \file bous_therm_param.cc

#include "bous_therm_param.h"

//-----------------------------------
//depth dependent parameters

double f_poro (double depth, double c0, double c1) {
    return( c0*exp(-depth/c1) );
}

double f_perm (double depth, double c0, double c1) {
    return( c0*exp(-depth/c1) );
}

double f_ktherm (double poro, double kTr) {
    //harmonic mean
    return( kTr*KT_W/(poro*kTr + (1.0 - poro)*KT_W) );
}

double f_captherm (double poro, double temp, double wsat, double isat) {

    //capacity of rock, water, and ice
    double cap = (1.0 - poro)*RHO_R*C_R + poro*RHO_W*(isat*C_I + wsat*C_W);
    //apparent capacity for changing phase
    if ( fabs(temp - TFREEZE) <= AHCW/2.0 )
        cap += poro*isat*RHO_W*LF_W/AHCW;

    return(cap);
}

//-----------------------------------
//time dependent parameters

double f_surf_temp (double t, double h, double Ts0, double Tsf, double Tsgam, double TsLR) {
    return( Ts0 + (Tsf - Ts0)*(1.0 - exp(-t/(Tsgam*YEAR_SEC))) - h*TsLR);
}

double f_fluxH_a (double t) {
    (void)t;
    return( 0.0 );
}

double f_fluxH_b (double t) {
    (void)t;
    return( 0.0 );
}

//-----------------------------------
//other parameters

double f_visc (double temperature) {
    return( 2.4e-5*pow(10.0, 248.0/(temperature - 140)) );
}
