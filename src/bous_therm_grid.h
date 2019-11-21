#ifndef BOUS_THERM_GRID_H_
#define BOUS_THERM_GRID_H_

//! \file bous_therm_grid.h

#include <iostream>
#include <cstdio>
#include <string>

#include "bous_therm_io.h"
#include "bous_therm_util.h"

//!Base class storing grid information
/*!
The BousThermGrid class is the base class of the model which contains grid spacing coordinates. It has no functionality and is just a container for grid variables.
*/
class BousThermGrid {

public:

    //!constructs
    /*!
    The constructor looks for files written by the `generate_grid.py` script. If any of them aren't found, an error is thrown.
    \param[in] griddir path to directory containing grid files
    */
    BousThermGrid (const std::string &griddir);
    //!destructs
    ~BousThermGrid ();

    //--------------------------------------------------------------------------
    //GRID VARIABLES

    //!directory with grid files
    std::string griddir;

    // --- Z ---
    //!number of vertical (thermal) cells in the domain for each horizontal edge
    long Nz;
    //!vertical (thermal) cell edge coordinates (edges not including the boundaries)
    double *ze;
    //!vertical (thermal) cell center coordinates
    double *zc;
    //!vertical (thermal) domain depth (the surface is at z=0)
    double zdepth;
    //!vertical (thermal) cell widths
    double *delz;

    // --- X ---
    //!number of horizontal cells in the domain
    long Nx;
    //!horizontal (hydraulic) cell edge coordinates (edges not including the boundaries)
    double *xe;
    //!horizontal (hydraulic) cell center coordinates
    double *xc;
    //!horizontal (hydraulic) domain lower boundary coordinate
    double xa;
    //!horizontal (hydraulic) domain upper boundary coordinate
    double xb;
    //!horizontal (hydraulic) cell widths
    double *delx;

    //!surface topography at horizontal (x) cell edges
    double *ztope;
    //!surface topography at horizontal (x) cell centers
    double *ztopc;
    //!topographic height above lowest point at cell edges
    double *htope;

};

#endif
