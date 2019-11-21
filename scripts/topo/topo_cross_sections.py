import sys
import numpy as np
from os.path import join
import matplotlib.pyplot as plt
from scipy.interpolate import interp2d

#plt.rc('font', family='serif')
#plt.rc('text', usetex=True)

#-------------------------------------------------------------------------------
#INPUT

#path to topography binary file
fntopo = join('..', '..', 'data', 'gale-dichotomy-topo', 'Gale_dichotomy_topo.img')

#output directory
dirout = join('..', '..', 'data', 'gale-dichotomy-topo')

#directory to save plots in
plotout = join('..', '..', 'plots')

#dimensions of topography grid
ncol, nrow = 3506, 7011

#easting and northing of top left corner of topo grid
easting = -3318734.3008
northing = 344977.9621

#spacing of topo grid pixels
dx = 463.057667199087
dy = 463.057667208672

#x and y coordinates of cross sections
yinterp = np.linspace(-2e6, 0, 1000)
xinterp = [-2.55301e6, -2.5042e6, -2.45788e6]

#y coordinates of Gale along each cross section
galey = [(-369776, -262519), (-394046, -241381), (-381520, -255473)]

#colormap to use
cmap = 'magma'

#-------------------------------------------------------------------------------
#MAIN

#read in the topography tile with Gale in it, spanning -44 to 0 lat, 90 to 180 lon
b = np.fromfile(fntopo, dtype='int16').reshape(nrow, ncol).astype('float32')

#create x,y grid
xe = np.linspace(easting, easting + dx*ncol, ncol+1)
ye = np.linspace(northing, northing - dy*nrow, nrow+1)

#create an interpolating function
interp = interp2d((xe[1:] + xe[:-1])/2, (ye[1:] + ye[:-1])/2, b)

#interpolate for cross sections and write them to file
N = len(xinterp)
xs = []
lab = ['A', 'B', 'C']
for i in range(N):
    xs.append( interp(xinterp[i], yinterp).flatten() )
    fn = join(dirout, 'profile_%s_z_float64' % lab[i])
    xs[-1].tofile(fn)
    print('file written: %s' % fn)
    fn = join(dirout, 'profile_%s_x_float64' % lab[i])
    np.array([xinterp[i]]*len(xs[-1])).tofile(fn)
    print('file written: %s' % fn)
    fn = join(dirout, 'profile_%s_y_float64' % lab[i])
    yinterp.tofile(fn)
    print('file written: %s' % fn)

#make a nice plot
plt.figure()
axs = [
    plt.subplot2grid((3,2), (0,0), rowspan=3),
    plt.subplot2grid((3,2), (0,1)),
    plt.subplot2grid((3,2), (1,1)),
    plt.subplot2grid((3,2), (2,1)),
]

#first the whole grid showing the regional topography
ax = axs[0]
r = ax.pcolorfast(xe, ye, b/1e3, cmap=cmap)
cb = plt.colorbar(r, ax=ax)
cb.set_label('Elevation (km)', rotation=270, va='bottom')
ax.set_aspect('equal')
ax.set_xlim(min(xe), max(xe))
ax.grid(False)
ax.set_xticks([])
ax.set_yticks([])
ax.spines['right'].set_visible(True)
ax.spines['top'].set_visible(True)

#now each of the cross sections
ha = ['right', 'center', 'left']
va1 = ['center', 'bottom', 'center']
va2 = ['center', 'top', 'center']
for i in range(N):
    ax = axs[i+1]
    ax.plot(yinterp/1e3, xs[i]/1e3, 'k')
    yl = ax.get_ylim()
    ax.plot([galey[i][0]/1e3]*2, yl, ':', color='gray', alpha=0.5, zorder=-1)
    ax.plot([galey[i][1]/1e3]*2, yl, ':', color='gray', alpha=0.5, zorder=-1)
    ax.text(sum(galey[i])/2e3, yl[1], 'Gale', va='top', ha='center')
    ax.set_title("%s - %s'" % (lab[i],lab[i]))
    axs[0].plot([xinterp[i]]*len(yinterp), yinterp, linewidth=0.5, alpha=0.5, color='white')
    axs[0].text(xinterp[i], yinterp[-1], lab[i] + "'", ha=ha[i], va=va1[i], color='white')
    axs[0].text(xinterp[i], yinterp[0], lab[i], ha=ha[i], va=va2[i], color='white')
    ax.set_ylabel('Elevation (km)')
    if i == N-1:
        ax.set_xlabel('Distance (km)')
    else:
        ax.set_xticklabels([])
axs[-1].set_xlabel('Distance (km)')

plt.tight_layout()
fn = join(dirout, 'Gale_dichotomy_topo_plot')
plt.savefig(fn + '.png', format='png', dpi=300)
plt.savefig(fn + '.pdf', format='pdf', dpi=300)
plt.show()
