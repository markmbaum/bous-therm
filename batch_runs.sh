: '
This script is for running a set of bous_therm models
defined by the columns of a batch settings file. This script
  1. runs the batch_settings.py script to generate a table of model trials
  2. runs the batch_setup.py script to create output directories and settings
     text files for each model trial
  3. submits a job for each of the model trials
The model has to be compiled and grid files must exist before running this
script. This script can only run in the top bous-therm directory.
'

#inputs
echo "Enter path to batch settings csv/xlsx file:"
read fnbatch
echo "Enter path of top folder containing folders for each run:"
read dirbatch
echo "Enter path to (already created) grid directory:"
read dirgrid

#make sure Python is available
module load python

#call the batch_settings.py script to prepare a table of model trials
python scripts/batch_settings.py $fnbatch

#call the batch_setup.py script to create output directories and settings files
#for all the model trials
python scripts/batch_setup.py $fnbatch $dirbatch

#store all the created directories
subdirsbatch=${dirbatch}/*/

#put settings and grid information in the top batch directory for safe keeping
mv $fnbatch $dirbatch/
cp -r $dirgrid $dirbatch/

#get current directory
cwd=$(pwd)

#update grid directory
dirgrid=${cwd}/${dirbatch}/$(basename $dirgrid)

#start jobs for each of the batch runs
for d in $subdirsbatch; do

  #get the name of the settings file
  fnset=${d}settings.txt
  #move into the output directory
  cd ${d%?}
  #RUN THE MODEL! (see bous_therm.batch for details)
  sbatch ${cwd}/bous_therm.batch ${cwd}/bin $dirgrid ${cwd}/$fnset .
  #move back to the original directory
  cd $cwd

done
