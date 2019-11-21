import matplotlib.pyplot as plt

#plt.rc('font', family='calibri')
#plt.rc('font', size=18)

from output import *

#-------------------------------------------------------------------------------
#INPUTS

#grid directory
gdir = join('..', 'results', 'grid')

#results directory
tdir = join('..', 'results', 'batch-runs', '114')

#-------------------------------------------------------------------------------
#MAIN

#plot_context(Trial(tdir, gdir), Grid(gdir), legend=True)

plot_temperature(Trial(tdir, gdir), Grid(gdir), Tlim=inf)#, xmin=-750e3, xmax=0)

plt.show()
