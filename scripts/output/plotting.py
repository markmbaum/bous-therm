from numpy import *
from scipy.interpolate import griddata
import matplotlib.pyplot as plt

from .reading import *

#plt.rc('font', family='serif')
#plt.rc('text', usetex=True)

#-------------------------------------------------------------------------------
# CONFIG

#distance unit as a multiple of meters
DU = 1e3
#label for distance unit
DL = 'km'
#time unit as a multiple of seconds
TU = 3600*24*365.25
#label for time unit
TL = 'yr'
#freezing point (K)
TF = 273.0

#-------------------------------------------------------------------------------
# FUNCTIONS

def plot_temperature(trial, grid, Tlim=27, H=False, xmin=-inf, xmax=inf):
    """Makes a two panel plot showing initial and final T for a single trial
    args:
        trial - Trial object
        grid - Grid object
    optional args:
        legend - whether to create a legend
        Tlim - range around the freezing point to use as colobar limits
        H - whether to plot the water table too
        xmin - lower lim of domain to plot
        xmax - upper lim of domain to plot"""

    #make subplots
    N = len(trial.T)
    fig, axs = plt.subplots(N, 1)
    fig.subplots_adjust(hspace=0.05)
    #plot temperature fields
    Tmin = min([trial.T[i].min() for i in range(N)])
    Tmax = max([trial.T[i].max() for i in range(N)])
    if Tmin < TF - Tlim or Tmax > TF + Tlim:
        vmin = TF - Tlim
        vmax = TF + Tlim
        if Tmax <= TF + Tlim:
            extend = 'min'
        elif Tmin >= TF - Tlim:
            extend = 'max'
        else:
            extend = 'both'
    else:
        v = max([abs(Tmax - TF), abs(TF - Tmin)])
        vmin = TF - v
        vmax = TF + v
        extend = 'neither'
    for i in range(N):
        T = trial.T[i].copy()
        x = grid.xe.copy()
        z = grid.zc.copy()
        X, Z = meshgrid(x, z)
        Z = add(Z, grid.ztope)
        xr = linspace(max([xmin, X.min()]), min([xmax, X.max()]), 2500)
        zr = linspace(Z.min(), Z.max(), 500)
        Xr, Zr = meshgrid(xr, zr)
        Tr = griddata(
            (X.flatten(), Z.flatten()),
            T.flatten(),
            (Xr.flatten(), Zr.flatten()),
            fill_value=nan
        ).reshape(Xr.shape)
        for j in range(Tr.shape[0]):
            for k in range(Tr.shape[1]):
                zz = interp(xr[k], grid.xe, grid.ztope)
                if Zr[j,k] > zz or Zr[j,k] < zz + grid.ze.min():
                    Tr[j,k] = nan

        r = axs[i].pcolormesh(Xr/DU, -Zr/DU, Tr,# shading='gouraud',
                cmap='RdBu_r', vmin=vmin, vmax=vmax)
        if H:
            axs[i].plot(grid.xc/DU, (grid.ztopc - trial.H[i])/DU, 'r')
        axs[i].contour(Xr/DU, -Zr/DU, Tr, levels=[273], colors='k', linewidths=0.5)
        axs[i].invert_yaxis()
        axs[i].text(0.01, 0.01, '{:,g} {}'.format(trial.snap_t[i]/TU, TL),
                ha='left', va='bottom', transform=axs[i].transAxes)
        if i != N-1:
            axs[i].set_xticklabels([])
            axs[i].tick_params(axis='x', length=0)

    cb = fig.colorbar(r, ax=axs, extend=extend)
    cb.set_label('Temperature (K)')
    fig.text(0.5, 0.04, 'Horizontal Extent (%s)' % DL, ha='center')
    fig.text(0.04, 0.5, 'Elevation (%s)' % DL, va='center', rotation='vertical')

    return(axs)

def plot_context(trial, grid, legend=False, Tlim=27):
    """Makes a three panel plot showing T and H snaps for a single trial
    args:
        trial - Trial object
        grid - Grid object
    optional args:
        legend - whether to create a legend
        Tlim - range around the freezing point to use as colobar limits"""

    #make subplots
    fig = plt.figure(figsize=(10,6))
    gs = fig.add_gridspec(3, 1)
    axTi = fig.add_subplot(gs[0])
    axTf = fig.add_subplot(gs[1])
    axH = fig.add_subplot(gs[2])
    #plot water table depths
    for i in range(trial.nsnap):
        label = '%g %s' % (trial.snap_t[i]/TU, TL)
        axH.plot(grid.xc/DU, grid.ztopc - trial.H[i], label=label)
    axH.invert_yaxis()
    axH.set_xlabel('Horizontal Extent (%s)' % DL)
    axH.set_ylabel('Water Table Depth\n(m)')
    if legend:
        axH.legend()
    #plot temperature fields
    Tmin = min([trial.T[0].min(), trial.T[-1].min()])
    Tmax = max([trial.T[0].max(), trial.T[-1].max()])
    if Tmin < TF - Tlim or Tmax > TF + Tlim:
        vmin = TF - Tlim
        vmax = TF + Tlim
        if Tmax <= TF + Tlim:
            extend = 'min'
        elif Tmin >= TF - Tlim:
            extend = 'max'
        else:
            extend = 'both'
    else:
        v = max([abs(Tmax - TF), abs(TF - Tmin)])
        vmin = TF - v
        vmax = TF + v
        extend = 'neither'
    r = axTi.pcolormesh(grid.xe/DU, -grid.zc/DU, trial.T[0],
            shading='gouraud', cmap='RdBu_r', vmin=vmin, vmax=vmax)
    r = axTf.pcolormesh(grid.xe/DU, -grid.zc/DU, trial.T[-1],
            shading='gouraud', cmap='RdBu_r', vmin=vmin, vmax=vmax)
    axTi.invert_yaxis()
    axTf.invert_yaxis()
    axTi.set_ylabel('Depth (%s)' % DL)
    axTf.set_ylabel('Depth (%s)' % DL)
    axTf.set_xlabel('Horizontal Extent (%s)' % DL)
    cb = fig.colorbar(r, ax=(axTi,axTf), extend=extend)
    cb.set_label('Temperature (K)')

    return(axH, axTi, axTf)
