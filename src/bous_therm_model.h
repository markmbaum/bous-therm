#ifndef BOUS_THERM_MODEL_H_
#define BOUS_THERM_MODEL_H_

//! \file bous_therm_model.h

#include <iostream>
#include <string>
#include <vector>

#include "omp.h"

#include "bous_therm_param.h"
#include "bous_therm_util.h"
#include "bous_therm_settings.h"
#include "bous_therm_numerics.h"

//!top-level modeling class implementing initialization, the ODE function, and output
/*!
BousThermModel is the main modeling class, inheriting from BousThermNumerics. The class defines functions for initializing the model, evaluating the spatial discretization of the shallow groundwater equation (Boussinesq equation) in finite-volume form, evaluating the spatial discretization of the heat equation in finite-volume form also, and managing output.
*/
class BousThermModel : public BousThermNumerics {

public:

    //!constructs
    /*!
    \param[in] griddir_ path to the grid directory
    \param[in] neq_ the size of the system of ODEs
    \param[in] stg_ a Settings structure (can be created by the parse_settings() function)
    \param[in] dirout_ path to directory where output is written
    */
    BousThermModel (const std::string &griddir_, long neq_, Settings *stg_, const char *dirout_);
    //!destructs
    ~BousThermModel ();

    //!output directory
    std::string dirout;

    //!settings structure
    Settings *stg;

    //------------------------------------------------------------------
    //aliases

    //!alias for part of the solution array containing temperatures
    double **T;
    //!alias for part of the solution array containing water table heights
    double *H;

    //!alias for ode hydraulic input
    double *Hin;
    //!alias for ode thermal input
    double **Tin;
    //!alias for ode hydraulic output
    double *dHdt;
    //!alias for ode thermal output
    double **dTdt;

    //------------------------------------------------------------------
    //main model variables

    //static physical parameters
    //!porosity with depth
    double *poro;
    //!permeability with depth
    double *perm;
    //!thermal conductivity
    double ktherm;
    //!porosity at the surface
    double poro_surf;

    //dynamic physical parameters and trackers
    //!groundwater flux
    double *qH;
    //!heat flux
    double **qT;
    //!vertically integrated hydraulic conductivity
    double *Kint;
    //!surface temperatures
    double *Tsurf;
    //!aquifer bottom locations
    double *aqbot;
    //!thermal capacity with depth
    double **captherm;
    //!water saturation fraction
    double **wsat;
    //!ice saturation fraction
    double **isat;

    //storage for some derivatives and such
    //!gradient of water table
    double *gradH;
    //!water table values at cell edges
    double *Hedge;
    //!gradient of temperature profile
    double **gradT;

    //------------------------------------------------------------------
    //trackers, snappers, monitors

    //!evaporation
    double *evap;
    //!evaporation per unit width
    double *evapw;
    //!cumulative evaporation
    double *cumevap;

    //!output times
    std::vector<double> o_t;
    //!output evaporation
    std::vector<double> o_evap;
    //!output evaporation per unit width
    std::vector<double> o_evapw;
    //!maximum aquifer bottom elevation
    std::vector<double> o_maxaqbot;
    //!minimum aquifer bottom elevation
    std::vector<double> o_minaqbot;

    //!wall clock start time
    double start_time;
    //!computes total evaporation at current model state
    double total_evap ();
    //!computes total evaporation per unit width at current model state
    double total_evap_per_width ();
    //!computes maximum water table depth at current model state
    double max_H_depth ();
    //!computes minimum water table depth at current model state
    double min_H_depth ();

    //------------------------------------------------------------------
    //spatial discretization

    //!hydraulic conductivity
    /*!
    viscosity is calculated as a function of temperature
    \param[in] perm permeability (m^2)
    \param[in] temp temperature (K)
    \return hydraulic conductivity (m/s)
    */
    double f_K (double perm, double temp);

    //!computes vertically integrated hydraulic conductivity
    /*!
    \param[in] aqbot bottom of the aquifer in z coordinates
    \param[in] zedge water table in z coordinates
    \param[in] Tin temperature array for the column
    \return vertically integrated hydraulic conductivity
    */
    double f_Kint (double aqbot, double zedge, double *Tin);

    //!computes fluxes for T
    /*!
    \param[in] gradT thermal gradient dT/dz
    */
    double f_qT (double gradT);

    //!computes fluxes for H
    /*!
    \param[in] gradH hydraulic gradient dH/dx
    \param[in] Kint vertically integrated hydraulic conductivity
    */
    double f_qH (double gradH, double Kint);

    //!computes temperature time derivatives
    /*!
    \param[in] fl thermal flux on lower cell edge
    \param[in] fr thermal flux on upper cell edge
    \param[in] delz cell height
    \param[in] cap cell thermal capacity
    */
    double f_dTdt (double fl, double fr, double delz, double cap);

    //!computes water table time derivatives
    /*!
    \param[in] fl hydraulic flux on lower cell edge
    \param[in] fr hydraulic flux on upper cell edge
    \param[in] po porosity at water table height
    \param[in] delx cell width
    */
    double f_dHdt (double fl, double fr, double po, double delx);

    //!computes saturation fractions (ice and water) for a temperature column
    /*!
    \param[in] zedge water table height of column in z coordinates
    \param[in] aqbot aquifer bottom elevation
    \param[in] Tin temperature array for column
    \param[out] sw water sat frac
    \param[out] si ice sat frac
    */
    void update_sat (double zedge, double aqbot, double *Tin, double *sw, double *si);

    //!evaluates time derivatives as the ODE solver sees them
    /*!
    \param[in] solin current solution array
    \param[out] fout evaluated time derivatives
    */
    void ode_fun (double *solin, double *fout);

    //!updates the evaporation arrays after a step
    void update_evaporation ();

    //------------------------------------------------------------------
    //extras (which are still important to the integration process)

    //!extra things to do before starting a solve
    virtual void before_solve ();

    //!extra things after EVERY step
    /*!
    \param t current model time
    */
    virtual void after_step (double t);

    //!extra things to do upon snapping
    /*!
    \param[in] dirout path to output directory
    \param[in] isnap index of snap being taken
    \param[t] current model time
    */
    virtual void after_snap (std::string dirout, long isnap, double t);

    //!extra things to do after completing a solve
    virtual void after_solve ();
};

//------------------------------------------------------------------------------

//!creates a BousThermModel object from grid files and settings file
/*!
An initialization function is required because the model class must know the
number of x grid points and z grid points (size of ODE system) when it is
constructed. So, the initializer function reads these numbers from the grid
directory, constructs the model, and returns it.
\param[in] griddir path to directory with grid files
\param[in] stg Settings structure
\param[in] dirout path to directory where
*/
BousThermModel init_model (std::string &griddir, Settings *stg, const char *dirout);

#endif
