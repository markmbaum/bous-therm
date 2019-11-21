from numpy import linspace
import matplotlib.pyplot as plt

#water's latent heat of fusion
L_w = 334e3 #J/kg
#water's density
rho_w = 1e3 #kg/m^3
#water's specific heat capacity
c_w = 4180 #J/kg*Kelvin
#rock's density
rho_r = 2900 #kg/m^3
#rock's specific heat capacity
c_r = 840 #J/kg*Kelvin

#porosity range
n = linspace(0, 0.5)
#total heat capacity
C = n*c_w*rho_w + (1 - n)*c_r*rho_r #J/m^3*Kelvin
#temperature change
delT = n*L_w*rho_w/C

print(50*C.mean(), 0.2*L_w*rho_w)

plt.plot(n, delT)
plt.xlabel('porosity')
plt.ylabel('Temperature Change Equivalent of $L_f$')
plt.show()
