import numpy as np
from os.path import join
import matplotlib.pyplot as plt

#grid directory
griddir = join('..', 'grid')

#-------------------------------------------------------------------------------
#horizontal grid

xc = np.fromfile(join(griddir, 'xc'))
delx = np.fromfile(join(griddir, 'delx'))
ztopc = np.fromfile(join(griddir, 'ztopc'))

fig, (ax1,ax2) = plt.subplots(2,1)

ax1.plot(xc, ztopc, 'o-', markersize=1, linewidth=0.5)
ax1.set_title('grid points along surface')
ax1.set_xlabel('x')
ax1.set_ylabel('z')

ax2.plot(xc, np.log2(delx/delx.max()), 'k', markersize=1)
ax2.set_title('grid spacing')
ax2.set_xlabel('x')
ax2.set_ylabel(r'$\log_2\left(\frac{\Delta x}{\Delta x_{max}}\right)$')

fig.tight_layout()

#-------------------------------------------------------------------------------
#vertical grid

zc = np.fromfile(join(griddir, 'zc'))
delz = np.fromfile(join(griddir, 'delz'))

fig, (ax1,ax2) = plt.subplots(1,2)

ax1.plot([0]*len(zc), -zc, 'o-', markersize=1, linewidth=0.5)
ax1.invert_yaxis()
ax1.set_title('grid point centers')
ax1.set_ylabel('z')
ax1.set_xticks([])

ax2.plot(np.log10(delz), -zc, 'k', markersize=1)
ax2.invert_yaxis()
ax2.set_title('grid spacing')
ax2.set_ylabel('z')
ax2.set_xlabel(r'$\log_{10}\left(\Delta z\right)$')

plt.show()
