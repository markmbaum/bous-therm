//! \file main.cc

/*!
\mainpage bous_therm

Welcome to the documentation pages for `bous_therm`, a model of unconfined groundwater flow in a downward thawing aquifer written in C++ with some Python scripts to help.

Setup
-----

Before the model can be run:
    1. Download and build [`libode`](https://github.com/wordsworthgroup/libode), a library of C++ integrators. A second-order explicit method is used by `bous_therm` by default. Information about how to compile `libode` can be found in its readme and documentation files. The library is small and self-contained (no dependencies), so it should be straightforward to build.
    2. Copy the `_config.mk` file in the top `bous_therm` directory to `config.mk` and change any compiler settings in the file to your specifications. In that file, the `odepath` variable should indicate where the top `libode` directory is. Then simply run `make` in the top `bous_therm` directory. If `libode` has been compiled, all the necessary linking is done in the makefile. Your compiler must have openmp.
    4. Edit the inputs/settings section of the `generate_grid.py` script, then run it to create the grid files needed by the model. The script can be configured to read binary files with whatever model topography is desired.
    5. Create a settings file or another file that will contain the model run settings. There should be a working file named `settings.txt` in the `bous_therm` repository, but the settings file can have any name.

Now the model is compiled, the grid files are written, and a settings file is prepared. Next, create a directory for model output and run the model with

    ./bin/bous_therm.exe <grid directory> <settings file> <output directory>

where
    + `<grid directory>` is the directory with grid files that you created with `generate_grid.py`
    + `<settings file>` is the settings file that you created for your model fun (probably called `settings.txt`)
    + `<output directory>` is the directory you created to save model output inside of

The model produces a slew of binary files. These can be read and plotted with the functions in the `reading.py` and `plotting.py` modules in the `scripts` directory. Also in that directory, the `plot_out.py` script has some commands for plotting a single model trial.

Structure
---------

The top-level modeling functions are implemented in the BousThermModel class. It initializes the water table and temperature profiles, computes time derivatives as they're seen by the inherited ODE solver, and writes output files. Below this class, the BousThermNumerics class contains some functions for numerical tasks. The Numerics class also inherits from an ODE solving class in [`libode`](https://github.com/wordsworthgroup/libode). Further below, the BousThermGrid class is simply a container for grid variables which are read from files upon construction. Several static physical parameters and functions for other physical parameters are defined in bous_therm_param.h.

A few other modules support the main model classes.
+ bous_therm_io.h: functions for reading and writing files
+ bous_therm_util.h: miscellaneous useful functions
+ bous_therm_settings.h: definition of the Settings structure
*/

#include <iostream>
#include <string>
#include <vector>
#include <string>

#include "omp.h"
#include "bous_therm_io.h"
#include "bous_therm_util.h"
#include "bous_therm_settings.h"
#include "bous_therm_model.h"

//!model driver
int main (int argc, char **argv) {

    //--------------------------------------------------------------------------
    //check input

    if ( argc < 4 ) {
        std::cout << "FAILURE: bous_therm requires three command line inputs\n  1) path to directory containing grid files\n  2) path to settings file\n  3) path to output directory\nAt least one of these inputs was missing." << std::endl;
        exit(EXIT_FAILURE);
    }

    //--------------------------------------------------------------------------
    //start the output

    std::cout << "\n-------------\nstarting main\n";
    std::cout << "omp max threads: " << omp_get_max_threads() << std::endl;

    //--------------------------------------------------------------------------
    //get command line arguments, input file and output directory

    std::string dirgrid = argv[1],
                fnset   = argv[2],
                dirout  = argv[3];

    //--------------------------------------------------------------------------
    //read settings from text file

    //read the settings file
    std::cout << "reading from settings file: " << fnset << std::endl;
    std::vector< std::vector< std::string > > sv = read_settings_file(fnset.c_str());
    //parse the settings into the struct
    Settings stg = parse_settings(sv);
    std::cout << "settings parsed" << std::endl;

    //--------------------------------------------------------------------------
    //instantiate the model

    BousThermModel mod = init_model(dirgrid, &stg, dirout.c_str());

    //--------------------------------------------------------------------------
    //run the model

    double tend_sec = stg.tend*stg.tunit;

    std::cout << "output directory: " << dirout << std::endl;
    printf("integrating for %g seconds (%g yr), %lu steps, %d snaps\n",
        tend_sec, tend_sec/YEAR_SEC, stg.nstep, stg.nsnap);
    std::cout << std::endl;

    mod.solve_fixed(tend_sec, tend_sec/double(stg.nstep), stg.nsnap, dirout.c_str());

    printf("trial complete\n");

    //--------------------------------------------------------------------------
    //finish

    std::cout << "\nmain finished\n-------------\n" << std::endl;
    return(0);
}
