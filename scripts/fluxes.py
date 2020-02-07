from numpy import *

#vertically averaged permeability (m^2)
k = 3e-13
#gravity (m^2/s)
g = 3.7
#water density (kg/m^3)
rho = 1e3
#water viscosity (Pa*s)
mu = 9e-4
#average hydraulic gradient into lake (m/m)
dh = 0.02
#groundwater flow depth (m)
d = 1e4
#lake groundwater flow radius (m)
R = 75e3 #generous
#lake evaporation surface area (m^2)
SA = 3600e6
#evaporation rate from lake (m/yr)
e = 1.5

#calculate groundwater delivery
G = d*(k*g*rho/mu)*dh*2*pi*R
#calculate evaporative removal
E = (e/(3600*24*365.25))*SA

print(E/G)
