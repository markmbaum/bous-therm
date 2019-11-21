import sys
import numpy as np
import pandas as pd
from os.path import join
from itertools import product

#-------------------------------------------------------------------------------
# INPUT

assert len(sys.argv) > 1, 'must give command line arg to batch_settings.py indicating path of output file'
fnout = sys.argv[1]
assert fnout[-4:] == '.csv', 'output file must have ".csv" extension'

#integer label of first/lowest directory/trial
i0 = 0

params = {
    'poro0': [0.2],
    'perm0': [1e-14, 1e-13, 1e-12, 1e-11],
    'permgam': [500, 1000, 2500],
    'kTr': [1, 3, 6],
    'Ts0': [220, 250],
    'Tsf': [275, 285, 295],
    'Rmax': [1, 0],
    'nsnap': [5],
    'tend': [20000],
    'tunit': [31557600],
    'nmaxout': [2500],
    'Hdep0': [0],
    'porogam': [2500],
    'fTgeo': [0.04],
    'Tsgam': [1],
    'TsLR': [0.003, 0.0044],
    'nstep': [int(150e6)]
}

#-------------------------------------------------------------------------------
# MAIN

cols = [k for k in params]
vals = [params[k] for k in cols]
prod = list(product(*vals))

df = pd.DataFrame(prod, columns=cols, index=range(i0, i0 + len(prod)))
df.to_csv(fnout, index_label='trial name')
print('batch settings file with %d trials written to:\n  %s' % (len(df), fnout))
