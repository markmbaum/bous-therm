import numpy as np
import matplotlib.pyplot as plt

visc = lambda T: 2.4e-5*(10.0**(248.0/(T - 140.0)))

T = np.linspace(273, 330)

plt.plot(T, visc(T))
plt.xlabel('T')
plt.ylabel(r'$\mu$')
plt.figure()
plt.plot(T, visc(T)/visc(T[0]))
plt.xlabel('T')
plt.ylabel(r'$\frac{\mu}{\mu(%g)}$' % T[0])
plt.show()
