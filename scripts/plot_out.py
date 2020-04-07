import matplotlib.pyplot as plt

#plt.rc('font', family='calibri')
#plt.rc('font', size=18)

from output_tools import *

#-------------------------------------------------------------------------------
#INPUTS

#grid directory
gdir = join('..', 'results', 'grid')

#results directory
tdir = join('..', 'out')

#-------------------------------------------------------------------------------
#MAIN

#plot_context(Trial(tdir, gdir), Grid(gdir), legend=True)

plot_temperature(Trial(tdir, gdir), Grid(gdir), Tlim=inf)#, xmin=-750e3, xmax=0)

plt.show()
