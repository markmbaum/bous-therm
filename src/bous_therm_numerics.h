#ifndef BOUS_THERM_NUMERICS_H_
#define BOUS_THERM_NUMERICS_H_

//! \file bous_therm_numerics.h

#include <iostream>
#include <string>
#include <cmath>

#include "bous_therm_param.h"
#include "bous_therm_util.h"
#include "bous_therm_grid.h"

/* explicit, single-step ODE solver */
#include "ode_trapz.h"

//------------------------------------------------------------------------------
//model class

//!Inherits from the BousThermGrid class and an ODE integrating class from libode
/*!
Implements some of the numerical functions needed for evaluating the spatial discretization
*/
class BousThermNumerics : public BousThermGrid, public OdeTrapz {

public:

    //!constructs
    /*!
    \param[in] griddir_ path to directory with grid files
    \param[in] neq_ size of ODE system
    */
    BousThermNumerics (const std::string &griddir_, long neq_);

    //!finds a point on a line, given two points and the value to locate
    /*!
    \param[in] xa x coordinate of first point
    \param[in] ya y coordinate of first point
    \param[in] xb x coordinate of second point
    \param[in] yb y coordinate of second point
    \param[in] y value to locate
    */
    double f_linfind (double xa, double ya, double xb, double yb, double y);

    //!find the freezing point
    /*!
    \param[in] Tin temperature column
    \param[in] Tsurf surface temperature
    */
    double f_aquifer_bottom (double *Tin, double Tsurf);

    //!find the index of the cell containing a point
    /*!
    if the point lies outside of all the edges, the index of the cell on the appropriate boundary is returned
    \param[in] edges array of cell edges
    \param[in] pt point to locate
    \param[in] nedge length of the edges array
    */
    long point_inside (double *edges, double pt, long nedge);
};


#endif
