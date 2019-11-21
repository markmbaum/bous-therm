#ifndef BOUS_THERM_SETTINGS_H_
#define BOUS_THERM_SETTINGS_H_

//! \file bous_therm_settings.h

#include <iostream>
#include <vector>
#include <string>
#include <string.h>

//!container struct for all the settings variables needed for a BousThermModel run
struct Settings {

    //-------------------------------------
    //model set up and integration settings

    //!number of steps to run
    long unsigned nstep;
    //!duration of solve
    double tend;
    //!time unit to use (number of seconds)
    double tunit;
    //!number of snaps to take
    int nsnap;
    //!maximum length of output vectors (subsampled to accomodate)
    long unsigned nmaxout;

    //-------------------------------------
    //physical parameters

    //!initial depth of water table
    double Hdep0;
    //!artificial recharge keeping h right at the surface
    bool Rmax;
    //porosity (unitless) in the form c0*exp(-depth/c1)
    //!surface porosity
    double poro0;
    //!porosity exponential decay length (m)
    double porogam;
    //permeability (m^2) in the form c0*exp(-depth/c1)
    //!surface permeability
    double perm0;
    //!permeability exponential decay length (m)
    double permgam;
    //!thermal conductivity (W/m*Kelvin) of rock
    double kTr;
    //!geothermal heat flux (W/m^2)
    double fTgeo;
    //!initial surface temperature at lowest topographic point (Kelvin)
    double Ts0;
    //!final surface temperature at lowest topographic point (Kelvin)
    double Tsf;
    //!exponential decay constant for temperature transition from Ts0 to Tsf (s)
    double Tsgam;
    //!lapse rate (K/m)
    double TsLR;

};

//!converts a string into a bool if it's supposed to be "true" or "false"
bool eval_txt_bool (const char *s);

//!abbreviates strcmp
bool cmp (const char *s1, const char *s2);

//!converts a character to an integet
long to_long(const char *val);

//!parses a settings file and returns it in a Settings structure
Settings parse_settings ( std::vector< std::vector< std::string > > sv );

#endif
