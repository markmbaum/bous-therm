from os import walk, remove
from os.path import join

#-------------------------------------------------------------------------------
# INPUT

#target directory to walk
rdir = join('..', 'results', 'batch-runs')

#strings contained by file names to remove
rfns = [
    'gradT_',
    'qT_',
    'captherm_',
    'isat_',
    'wsat_'
]

#-------------------------------------------------------------------------------
# MAIN

for d, _, fns in walk(rdir):
    for fn in fns:
        for rfn in rfns:
            if rfn in fn:
                remove(join(d, fn))
