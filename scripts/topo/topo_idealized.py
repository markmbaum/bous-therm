import numpy as np
from os.path import join
from scipy.optimize import curve_fit, minimize
import matplotlib.pyplot as plt

plt.rc('font', family='calibri')
#plt.rc('font', size=18)
#plt.rc('text', usetex=True)

#-------------------------------------------------------------------------------
#INPUT

#input data files
fn_yin = join('..', '..', 'data', 'gale-dichotomy-topo', 'profile_A_y_float64')
fn_zin = join('..', '..', 'data', 'gale-dichotomy-topo', 'profile_A_z_float64')

#output file names
fn_yout = join('..', '..', 'data', 'gale-dichotomy-topo', 'y_idealized')
fn_zout = join('..', '..', 'data', 'gale-dichotomy-topo', 'z_idealized')
fn_plot = join('..', '..', 'plots', 'smooth_topo')

#-------------------------------------------------------------------------------
#FUNCTIONS

fquad = lambda x, a, b, c: a*x*x + b*x + c

fsigmoid = lambda x, A, W, C, H: (-A*2/(1 + np.exp(-(x - C)*3/W)) + A) + H

fgaussian = lambda x, A, W, C, H: A*np.exp(-(x - C)*(x - C)/W) + H

funk = lambda x, As, Ws, Cs, Hs, Ag, Wg, Cg, Hg: (
        fsigmoid(x, As, Ws, Cs, Hs) + fgaussian(x, Ag, Wg, Cg, Hg))

def y_slices(y):
    y_c = (-352336, -240000)
    slc1 = (y < y_c[0])
    slc2 = ((y >= y_c[0]) & (y <= y_c[1]))
    slc3 = (y > y_c[1])
    slc4 = (y >= (y_c[0]/2 + y_c[1]/2))
    slc5 = (y >= -234000)
    return(slc1, slc2, slc3, slc4, slc5)

def rolling_avg(y, n):
    ya = np.zeros((len(y),))
    f = n//2
    for i in range(len(y)):
        idx1 = i - f
        idx2 = i + f + 1
        if idx1 < 0:
            idx2 += idx1
            idx1 = 0
        if idx2 > len(y):
            idx1 += (idx2 - len(y))
            idx2 = len(y)
        ya[i] = y[idx1:idx2].mean()
    return(ya)

#-------------------------------------------------------------------------------
#MAIN

#load the real topography
y = np.fromfile(fn_yin)
z = np.fromfile(fn_zin)

#create some slices through different sections of the profile
slc1, slc2, slc3, slc4, slc5 = y_slices(y)

#cut off some of the hills north of Gale
zt = z.copy()
zt[slc3 & (zt > -2e3)] = -2.5e3

#take a broad rolling average before fitting
zs = rolling_avg(zt, 50)

#fit various curves to sections of the profile
fits, _ = curve_fit(fsigmoid, y[slc1 | slc3], zs[slc1 | slc3])
fitg, _ = curve_fit(fgaussian, y[slc2], z[slc2], (-4e3, 2.5e9, -3e5, -1.75e3))
fitq, _ = curve_fit(fquad, y[slc5], zs[slc5])
sigma = np.ones((len(y),))
sigma[slc2] *= 7
fitf, _ = curve_fit(funk, y, z, list(fits) + list(fitg), sigma=sigma)

#evaluate the big fit, more densely sampled
y_orig = y.copy()
y = np.linspace(y[0], y[-1], int(1e6))
ztop = funk(y, *fitf)

#update slices through different sections of the profile
slc1, slc2, slc3, slc4, slc5 = y_slices(y)

#make a line out of the northern part
ya, yb, za, zb = y[slc5][0], y[slc5][-1], ztop[slc5][0], zs[-100:].mean()
m = (zb - za)/(yb - ya)
b = za - m*ya
ztop[slc5] = y[slc5]*m + b

#smooth everything out a little
ztop = rolling_avg(ztop, 7500)

#extend the northern lowlands a little
y = np.append(y, y[-1] + (y[1]-y[0])*np.arange(1,int(250e3/(y[1]-y[0]))))
ztop = np.append(ztop, [ztop[-1]]*(len(y) - len(ztop)))

#smooth the last segment and extension
_, _, _, _, slc5 = y_slices(y)
ztop[slc5] = rolling_avg(ztop[slc5], len(ztop[slc5])//2)

#write the idealized topography to file
y.tofile(fn_yout)
ztop.tofile(fn_zout)

#plot the topography to check and to show
fig, ax = plt.subplots(1, 1, figsize=(7,2.5))
ax.plot(y_orig/1e3, z/1e3, '.', color='gray', markersize=1, label="Topography")
ax.plot(y/1e3, ztop/1e3, 'C1', label='Idealized Topography')
ax.legend()
ax.set_ylabel('Elevation (km)')
ax.set_xlabel('x (km, South to North)')
fig.tight_layout()
plt.savefig(fn_plot, dpi=500)
plt.show()
