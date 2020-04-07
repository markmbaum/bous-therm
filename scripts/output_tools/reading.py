from pickle import dump
import bz2
from numpy import *
import pandas as pd
import os
from os.path import join, isfile

#-------------------------------------------------------------------------------
# FUNCTIONS

def isnum(x):
    #check if an object is a number (can be converted to float)
    try:
        x = float(x)
    except ValueError:
        return(False)
    else:
        return(True)

def isint(x):
    #check if an object can be converted to an integer
    if isnum(x):
        if int(float(x)) == float(x):
            return(True)
    return(False)

def tonum(x):
    #convert an object to an integer or float
    if isint(x):
        return(int(x))
    else:
        return(float(x))

#read a binary file from a directory (assuming float64 format)
join_read = lambda dirname, var: fromfile(join(dirname, var))

def read_grid_num(gdir, fn, dtype=int):
    """read a text file containing a single number from the grid dir
    args:
        gdir - grid directory
        fn - name of file to read"""
    #check extension
    if fn[-4:] != '.txt': fn += '.txt'
    #read
    with open(join(gdir, fn), 'r') as ifile:
        num = dtype(ifile.read().strip())
    return(num)

def read_snaps(tdir, snapname, reshape=None, transpose=False):
    """read all snap variables from an output directory into a 2d array
    args:
        tdir - directory with snaps in it
        snapname - name of snapped variable
    returns:
        snaps - list of snaps"""

    #get file names for snaps
    fns = os.listdir(tdir)
    #remove files that don't fit the expected format
    fns = [fn for fn in fns if snapname in fn]
    fns = [fn for fn in fns if ('_'.join(fn.split('_')[:-1]) == snapname)]
    #sort by the last digit
    fns = sorted(fns, key=lambda fn: int(fn.split('_')[-1]))
    #get arrays for snaps, in order
    snaps = [fromfile(join(tdir, fn)) for fn in fns]
    #reformat if desired
    if reshape is not None:
        snaps = [s.reshape(reshape) for s in snaps]
    if transpose:
        snaps = [s.T for s in snaps]

    return(snaps)

def read_T_H_snaps(tdir, gdir):
    """read the snaps output by the ode solver directly (snap_0, snap_1, etc.)
    and split them into H and T
    args:
        tdir - output directory
        gdir - grid directory
    returns:
        T - 3d array of temperature snaps
        H - 2d array of water table snaps"""

    #get file names for snaps
    fns = os.listdir(tdir)
    fns.remove('bous_therm_snap_t')
    fns = [join(tdir, fn) for fn in fns if ('bous_therm_snap_' in fn)]
    #remove the snap times
    #sort by the last digit
    fns = sorted(fns, key=lambda fn: int(fn.split('_')[-1]))
    #get arrays for snaps, in order
    snaps = [fromfile(fn) for fn in fns]
    #get size of grids
    Nx = read_grid_num(gdir, 'Nx', int)
    Nz = read_grid_num(gdir, 'Nz', int)
    #slice the snaps
    H = [s[:Nx] for s in snaps]
    T = [s[Nx:].reshape(Nx+1,Nz).T for s in snaps]

    return(T, H)

def read_settings(fn):
    """read the values of a settings file into a dictionary
    args:
        fn - settings file path
    returns:
        stg - dictionary of settings values"""

    stg = {}
    with open(fn, 'r') as ifile:
        for line in ifile.readlines():
            line = line.strip()
            if line:
                if line[0] != '#':
                    k, v = line.split('=')
                    k, v = k.strip(), v.strip()
                    stg[k] = tonum(v)
    return(stg)

def read_single(tdir, gdir):

    t = Trial(tdir, gdir)
    g = Grid(gdir)
    return(t, g)

def walk_settings(topdir, fnset='settings.txt'):
    """read settings files in all subdirectories
    args:
        topdir - top directory in the walk
    optional args:
        fn - name of settings files to look for
    returns:
        stgs - DataFrame of settings dictionaries"""

    stgs = {}
    for d, _, fns in os.walk(topdir):
        if fnset in fns:
            stgs[d] = read_settings(join(d,fnset))
    stgs = pd.DataFrame(stgs).T
    return(stgs)

#-------------------------------------------------------------------------------
# CLASSES

class Grid:

    def __init__(self, gdir):

        self.xc = join_read(gdir, 'xc')
        self.xe = join_read(gdir, 'xe')
        self.ztopc = join_read(gdir, 'ztopc')
        self.ztope = join_read(gdir, 'ztope')
        self.zc = join_read(gdir, 'zc')
        self.ze = join_read(gdir, 'ze')
        self.delz = join_read(gdir, 'delz')
        self.delx = join_read(gdir, 'delx')

    @property
    def Nx(self):
        return(len(self.xc))

    @property
    def Nz(self):
        return(len(self.zc))

class Trial:

    def __init__(self, tdir, gdir, fnset=None):

        Nx = read_grid_num(gdir, 'Nx', int)
        Nz = read_grid_num(gdir, 'Nz', int)

        z = fromfile(join(gdir, 'ze'))
        self._za, self._zb = z.min(), z.max()

        self.snap_t = join_read(tdir, 'bous_therm_snap_t')
        self.aqbot = read_snaps(tdir, 'aqbot')
        #self.captherm = read_snaps(tdir, 'captherm', (Nx+1,Nz), True)
        self.cumevap = read_snaps(tdir, 'cumevap')
        self.evap = read_snaps(tdir, 'evap')
        self.evapw = read_snaps(tdir, 'evapw')
        self.gradH = read_snaps(tdir, 'gradH')
        #self.gradT = read_snaps(tdir, 'gradT', (Nx+1,Nz+1), True)
        self.Hedge = read_snaps(tdir, 'Hedge')
        #self.isat = read_snaps(tdir, 'isat', (Nx+1,Nz), True)
        #self.wsat = read_snaps(tdir, 'wsat', (Nx+1,Nz), True)
        self.Kint = read_snaps(tdir, 'Kint')
        self.qH = read_snaps(tdir, 'qH')
        self.qT = read_snaps(tdir, 'qT', (Nx+1,Nz+1), True)
        self.Tsurf = read_snaps(tdir, 'Tsurf')

        self.T, self.H = read_T_H_snaps(tdir, gdir)

        self.perm = join_read(tdir, 'perm')
        self.poro = join_read(tdir, 'poro')

        self.o_evap = join_read(tdir, 'o_evap')
        self.o_evapw = join_read(tdir, 'o_evapw')
        self.o_maxaqbot = join_read(tdir, 'o_maxaqbot')
        self.o_minaqbot = join_read(tdir, 'o_minaqbot')
        self.o_t = join_read(tdir, 'o_t')

        self.set = None
        if fnset is not None:
            try:
                self.set = read_settings(fnset)
            except FileNotFoundError:
                print('settings file "%s" not found' % fnset)
        else:
            try:
                self.set = read_settings(join(tdir, 'settings.txt'))
            except FileNotFoundError:
                pass

    @property
    def nsnap(self):
        return(len(self.snap_t))

class Evap:

    def __init__(self, tdir, gdir):

        z = fromfile(join(gdir, 'ze'))
        self._za, self._zb = z.min(), z.max()

        self.t = join_read(tdir, 'o_t')
        self.evap = join_read(tdir, 'o_evap')
        self.evapw = join_read(tdir, 'o_evapw')
        self.maxaqbot = join_read(tdir, 'o_maxaqbot')
        self.minaqbot = join_read(tdir, 'o_minaqbot')

        self.tf = self.t[self.frozen]
        self.evapf = self.evap[self.frozen]

    @property
    def nt(self):
        return(len(self.t))

    @property
    def thawed(self):
        return( isclose(self.minaqbot, self._za) )

    @property
    def frozen(self):
        return( ~self.thawed )
