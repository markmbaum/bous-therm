import sys
import os
from os.path import join, isdir
import numpy as np
from numpy import sin, cos, pi
from scipy.interpolate import interp1d

#-------------------------------------------------------------------------------
#INPUTS/SETTINGS

#grid directory
griddir = 'grid'
#clear the grid directory before writing files
cldir = True

#starting number of points in x dimension
Nx0 = 100
#lower x domain boundary
xa = -2e6
#higher x domain boundary
xb = 5e5
#grid refinement thresholds, higher causes more points
# (vertical difference between points, 1st deriv, 2nd deriv)
# prefactor of 12 yields nice a density distribution
G = 12*np.array([1e-4, 2e-3, 1e2])

#thickness of z domain, which will be [-zdepth, 0]
zdepth = 2e3
#size of smallest cell at the surface
delz0 = 0.3
#cell size increase factor with depth
fdelz = 1.01

#read topography from a file?
readtopo = True
#topography file with horizontal coordinates (binary float64)
fnxtopo = join('data', 'gale-dichotomy-topo', 'y_idealized')
#topography file with vertical coordinates (binary float64)
fnztopo = join('data', 'gale-dichotomy-topo', 'z_idealized')
#interpolation mode
interp_type = 'linear'
#topography function if not reading from file
f_topo = lambda x: 0

#-------------------------------------------------------------------------------
#FUNCTIONS

#quick finite diff formulas
fd1 = lambda f, x, h: (f(x + h) - f(x - h))/(2*h)
fd2 = lambda f, x, h: (f(x - h) - 2*f(x) + f(x + h))/(h*h)

def div0(a, b):
    """divide by zero without a warning or error"""
    if b != 0:
        return(a/b)
    else:
        return(np.inf)

def refined_grid(xa, xb, Nx0, zrange, f, g0, g1, g2, smooth=True):
    """Generate a spatially refined grid that is more dense in regions
    where the first and second derivatives are high
    args:
        xa - lower edge of the domain
        xb - upper edge of the domain
        Nx0 - number of evenly spaced points to start with
        zrange - entire range of topography/elevation
        f - function for evaluating the topography
        g0 - parameter setting density w/r/t z differences (not slope)
        g1 - parameter setting density w/r/t slope (first deriv)
        g2 - parameter setting density w/r/t curvature (second deriv)
    optional args:
        smooth - bool, if True, the grid spacing will not change more than
                 a factor of 2 across neighboring cells
    returns:
        x - horizontal coordinates of grid points
        ztop - vertical coordinates of grid points"""

    #initial evenly spaced grid, minimum number of points
    x = np.linspace(xa, xb, Nx0)
    delx0 = (x[1] - x[0])/1e3
    #fill in points between points as needed
    done = False
    ztop = f(x)
    h = (xb - xa)/1e6
    while not done:
        #midpoints
        xmid = x[:-1]/2 + x[1:]/2
        #boolean array for filling
        b = np.zeros((len(xmid),), dtype='bool')
        #check each midpoint
        for i in range(len(xmid)):
            delx = x[i+1] - x[i]
            rho = 1.0/delx
            c0 = div0(rho*zrange, abs(ztop[i+1] - ztop[i]))
            c1 = div0(rho, abs(fd1(f, xmid[i], h)))
            c2 = div0(rho, abs(fd2(f, xmid[i], delx0)))
            #test each condition
            if c0 < g0 or c1 < g1 or c2 < g2:
                b[i] = True
        #add points if needed
        if np.any(b):
            x = np.sort(np.append(x, [xmid[b]]))
        else:
            done = True
        #recalculate topography
        ztop = f(x)

    #smooth jumps between different spacings
    if smooth:
        done = False
        while not done:
            done = True
            for i in range(len(x) - 2):
                rdelx = (x[i+1] - x[i])/(x[i+2] - x[i+1])
                if rdelx > 2.001:
                    x = np.sort(np.append(x, [x[i+1]/2 + x[i]/2]))
                    done = False
                    break
                elif rdelx < 1/2.001:
                    x = np.sort(np.append(x, [x[i+2]/2 + x[i+1]/2]))
                    done = False
                    break
        ztop = f(x)

    return(x, ztop)

#write a single integer into a text file
def write_one_int(i, fn, griddir=griddir):
    with open(join(griddir, fn), 'w') as ofile: ofile.write(str(i))

#write a single float into a text file
def write_one_float(x, fn, griddir=griddir):
    with open(join(griddir, fn), 'w') as ofile: ofile.write('%.16g' % x)

#write an array into a binary file
def write_array(a, fn, dtype='float64', griddir=griddir):
    a.astype(dtype).tofile(join(griddir, fn))

#-------------------------------------------------------------------------------
#MAIN

print('generating grid...')

#get the f_topo function, interpolation or from analytical form
if readtopo:
    f_topo = interp1d(np.fromfile(fnxtopo),
                      np.fromfile(fnztopo),
                      kind=interp_type,
                      bounds_error=False,
                      fill_value='extrapolate')

#variably spaced finite differences grid
xe, ztope = refined_grid(xa, xb, Nx0, zdepth, f_topo, *G)
#cell widths
delx = np.diff(xe)
#cell center coordinates
xc = (xe[1:] + xe[:-1])/2.0
#topography at cell centers
ztopc = f_topo(xc)
#recompute cell edge topography to comforn with midpont method
ztope = f_topo(xe)
#number of cells
Nx = len(xc)

#create depth grid edges
ze = [0, delz0]
while ze[-1] < zdepth:
    ze.append( ze[-1] + fdelz*(ze[-1] - ze[-2]) )
ze[-1] = zdepth
ze = -np.array(ze)[::-1]
#cell depths
delz = np.diff(ze)
#depth grid centers
zc = ze[:-1] + delz/2.0
#cell count
Nz = len(zc)

#print some info
print('  grid generated')
print('  Nx = %5d, minimum x spacing fraction: %g' % (Nx, np.diff(xe).min()/(xb-xa)));
print('  Nz = %5d, minimum z spacing fraction: %g' % (Nz, np.diff(ze).min()/zdepth));

#make the directory if necessary
if not isdir(griddir):
    os.makedirs(griddir)
    print('grid directory created')
elif cldir: #otherwise clear files if flagged
    print('clearing files...')
    for fn in os.listdir(griddir):
        if fn[0] != '.' and fn[0] != '_':
            os.remove(join(griddir, fn))
    print('  files cleared')

#write the information
print('writing files...')
# -- x --
write_one_int(Nx, 'Nx.txt')
write_array(xe, 'xe')
write_array(xc, 'xc')
write_array(delx, 'delx')
# -- z --
write_one_int(Nz, 'Nz.txt')
write_array(ze, 'ze')
write_array(zc, 'zc')
write_array(delz, 'delz')
# -- topography --
write_array(ztope, 'ztope')
write_array(ztopc, 'ztopc')
print('  files written')
print('grid and model files are in the "%s" directory' % griddir)

print('done')
