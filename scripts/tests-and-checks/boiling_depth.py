from numpy import *
import matplotlib.pyplot as plt

#-------------------------------------------------------------------------------
# INPUTS

#Clausius-Clapeyron T0 (K)
T0 =
#Clausius-Clapeyron P0 (Pa)
P0 = 101325
#ideal gas constant (J/K*mole)
R = 8.314
#heat of vaporization of water (J/mole)
Lv = 40660
#water density (kg/m^3)
rho = 1e3
#gravity (m/s^2)
g = 3.7
#thermal conductivity of the crust (W/m*K)
kT = 3
#geothermal heat flux (W/m^2)
Fgeo = 0.03
#surface temperature (K)
Tsurf = 290
#boiling point of water (K)
Tb = 373

#-------------------------------------------------------------------------------
# FUNCTIONS

#hydrostatic pressure as a function of depth (Pa)
f_P = lambda d: rho*g*d
#temperature as a function of depth (K)
f_T = lambda d: Tsurf + Fgeo*d/kT

def boiling_point_depth(Tsurf):
    d = (Tb - Tsurf)/(Fgeo/kT)
    return(d)

d = linspace(0, 1e4)
print(boiling_point_depth(Tsurf))
plt.plot(d, f_T(d))
plt.show()
