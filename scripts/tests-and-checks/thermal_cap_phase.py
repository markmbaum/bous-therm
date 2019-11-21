import numpy as np
import matplotlib.pyplot as plt

#thermal conductivities of rock, water, air
rho_r = 2800
rho_w = 1000
rho_a = 1.225
c_r = 840
c_w = 418
c_a = 1003

captherm = lambda n, S: (1 - n)*rho_r*c_r + S*n*rho_w*c_w + (1 - S)*n*rho_a*c_a

n = np.linspace(0, .3, 500)
S = np.linspace(0, 1, 500)
n, S = np.meshgrid(n, S)
cap = captherm(n, S)

fig, ax = plt.subplots(1,1)
r = ax.pcolormesh(n, S, cap)
cs = ax.contour(n, S, cap, colors='k', linewidths=0.5)
#plt.clabel(cs, inline=1)
cb = plt.colorbar(r, ax=ax)
cb.set_label('Thermal Capacity ($J/m^3 \cdot K$)', rotation=270, va='bottom')
ax.set_xlabel('porosity')
ax.set_ylabel('saturation')
ax.grid(False)

plt.show()
