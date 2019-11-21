import numpy as np
import matplotlib.pyplot as plt

#thermal conductivities of rock, water, air
kw = 0.591
kr = 2.5
ka = 0.026

ktherm = lambda n, S: S*(kr*kw)/(n*kr + (1 - n)*kw) + (1 - S)*(kr*ka)/(n*kr + (1 - n)*ka)

n = np.linspace(0, .3, 500)
S = np.linspace(0, 1, 500)
n, S = np.meshgrid(n, S)
k = ktherm(n, S)

fig, ax = plt.subplots(1,1)
r = ax.pcolormesh(n, S, k)
cs = ax.contour(n, S, k, colors='k', linewidths=0.5)
#plt.clabel(cs, inline=1)
cb = plt.colorbar(r, ax=ax)
cb.set_label('Bulk Thermal Conductivity ($W/m \cdot K$)', rotation=270, va='bottom')
ax.set_xlabel('porosity')
ax.set_ylabel('saturation')
ax.grid(False)

plt.show()
