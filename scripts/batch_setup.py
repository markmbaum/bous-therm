"""
This script is for reading a csv where each column contains the settings for
individual model runs and creating the necessary folders and files for those
model runs. It will create a top level batch directory with subdirectories
for each model run, named according to the columns in the input csv. It will
then write settings text files into each subdirectory for each model run.
This isn't useful for my laptop but makes it easy to run many jobs in
parallel on a big computer.
"""

import os
import sys
import shutil
import pandas as pd
from os.path import join, isdir

#-------------------------------------------------------------------------------
# INPUTS

assert len(sys.argv) > 1, 'first cmd line input must be batch settings csv/xlsx path'
fnbatchcsv = sys.argv[1]

assert len(sys.argv) > 2, 'second cmd line input must be top batch folder path'
dirbatch = sys.argv[2]

#-------------------------------------------------------------------------------
# FUNCTIONS

def toint(x):
    """convert something to an integer if it can be converted"""
    try:
        x = float(x)
    except ValueError:
        return(x)
    else:
        if round(x) == x:
            return(int(x))
        else:
            return(float(x))

#-------------------------------------------------------------------------------
# MAIN

#empty/create the dop level batch directory
if isdir(dirbatch):
    print('removing existing directory "%s"' % dirbatch)
    shutil.rmtree(dirbatch)
os.mkdir(dirbatch)
print('directory created: %s' % dirbatch)

#load the settings csv
if fnbatchcsv[-4:] == '.csv':
    stg = pd.read_csv(fnbatchcsv, index_col=0).T
elif fnbatchcsv[-5:] == '.xlsx':
    stg = pd.read_excel(fnbatchcsv, index_col=0).T
else:
    raise ValueError('input file must be csv or xlsx')

#create directories for each column and put a new settings file in them
for col in stg.columns:
    subdir = join(dirbatch, str(col))
    os.mkdir(subdir)
    print('  directory created: %s' % subdir)
    fnstg = join(subdir, 'settings.txt')
    with open(fnstg, 'w') as ofile:
        ofile.write('# settings for trial: %s\n\n' % col)
        for idx in stg.index:
            ofile.write('%s = %s\n' % (idx, toint(stg.at[idx,col])))
    print('    file written: %s' % fnstg)
