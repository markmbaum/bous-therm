#ifndef BOUS_THERM_PARAM_H_
#define BOUS_THERM_PARAM_H_

//! \file bous_therm_param.h

#include <cmath>

//------------------
//miscellaneous

//!little ol' pi
#define PI 3.1415926535897932384626433832795
//!number of seconds in an Earth year
#define YEAR_SEC 31557600.0
//!gravity (m/s^2), for Mars it's ~3.72
#define GRAV 3.72
//!freezing point of the groundwater
#define TFREEZE 273

//------------------
//densities

//!water density (kg/m^3)
#define RHO_W 1000.0
//!rock density (kg/m^3)
#define RHO_R 2900.0

//--------------------------------------
//specific heat capacities

//!ice specific heat capacity (J/kg*Kelvin)
#define C_I 2050.0
//!water specific heat capacity (J/kg*Kelvin)
#define C_W 4180.0
//!rock specific heat capacity (J/kg*Kelvin)
#define C_R 840.0

//-----------------------------------
//thermal conductivities

//!the thermal conductivity of water (W/m*Kelvin)
#define KT_W 0.59

//-----------------------------------
//specific latent heat

//!water heat of fusion (J/kg)
#define LF_W 334000.0
//!temperature width for apparent heat capacity during phase change
#define AHCW 2.5

//-----------------------------------
//depth dependent parameters

//!porosity (dimensionless)
double f_poro (double depth, double c0, double c1);

//!permeability (m^2)
double f_perm (double depth, double c0, double c1);

//!thermal conductivity (W/m*K)
double f_ktherm (double poro, double kTr);

//!thermal capacity (J/m^3*K)
double f_captherm (double poro, double temp, double wsat, double isat);

//-----------------------------------
//time dependent parameters

//!surface temperature over time (K)
double f_surf_temp (double t, double h, double Ts0, double Tsf, double Tsgam, double TsLR);

//!flux of water into the left boundary (m^2/s), positive is INTO the cell
double f_fluxH_a (double t);

//!flux of water into the right boundary (m^2/s), positive is INTO the cell
double f_fluxH_b (double t);

//-----------------------------------
//other parameters

//!temperature dependent viscosity (Pa*s)
double f_visc (double temperature);

#endif
