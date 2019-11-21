//! \file bous_therm_model.cc

#include "bous_therm_model.h"

//------------------------------------------------------------------------------
//construct and destruct

BousThermModel::BousThermModel (const std::string &griddir_, long neq_, Settings *stg_, const char *dirout_) :
    BousThermNumerics (griddir_, neq_) {

    std::cout << "constructing model" << std::endl;
    //give the ODE base class a name
    name_ = "bous_therm";
    //store the output directory
    dirout = dirout_;
    //bind the settings structure
    stg = stg_;
    //solution vector to zero at the beginning
    for (long i=0; i<long(get_neq()); i++)
        set_sol(i, 0.0);

    //alias the ode solution array, which is buried under many classes
    H = get_sol(); //water table
    T = new double*[Nx+1]; //temperature profiles
    for (long j=0; j<Nx+1; j++)
        T[j] = get_sol() + Nx + Nz*j; //pointer arithmetic

    //allocate space for ode aliases
    Tin = new double*[Nx+1];
    dTdt = new double*[Nx+1];

    //------------------------------------------------------------------
    //get anything from the settings struct

    ktherm = stg->kTr;

    //------------------------------------------------------------------

    //initial temperature profile
    for (long j=0; j<Nx+1; j++)
        for (long i=0; i<Nz; i++)
            T[j][i] = f_surf_temp(get_t(), htope[j], stg->Ts0, stg->Tsf, stg->Tsgam, stg->TsLR) - stg->fTgeo*zc[i]/ktherm;

    //intial head profile
    for (long j=0; j<Nx; j++) H[j] = ztopc[j] - stg->Hdep0;

    //------------------------------------------------------------------

    //static, depth dependent, physical parameters
    poro = new double[Nz];
    perm = new double[Nz];
    for (long i=0; i<Nz; i++) {
        poro[i] = f_poro(-zc[i], stg->poro0, stg->porogam);
        perm[i] = f_perm(-zc[i], stg->perm0, stg->permgam);
    }

    //dynamic physical parameters
    qH = new double[Nx+1];
    qT = new double*[Nx+1];
    Kint = new double[Nx+1];
    Tsurf = new double[Nx+1];
    aqbot = new double[Nx+1];
    wsat = new double*[Nx+1];
    isat = new double*[Nx+1];
    captherm = new double*[Nx+1];
    for (long j=0; j<Nx+1; j++) {
        qT[j] = new double[Nz+1];
        captherm[j] = new double[Nz];
        wsat[j] = new double[Nz];
        isat[j] = new double[Nz];
    }

    //derivatives and such
    gradH = new double[Nx+1];
    Hedge = new double[Nx+1];
    gradT = new double*[Nx+1];
    for (long j=0; j<Nx+1; j++)
        gradT[j] = new double[Nz+1];

    //tracking/snapping variables
    evap = new double[Nx];
    evapw = new double[Nx];
    cumevap = new double[Nx];

    //surface porosity
    poro_surf = f_poro(0.0, stg->poro0, stg->porogam);

    //------------------------------------------------------------------

    std::cout << "model constructor finished" << std::endl;
}

BousThermModel::~BousThermModel () {

    //allocated aliases
    frei(T);
    frei(Tin);
    frei(dTdt);
    //static physical
    frei(poro);
    frei(perm);
    //dynamic physical
    frei(qH);
    frei(qT, Nx+1);
    frei(Kint);
    frei(Tsurf);
    frei(aqbot);
    frei(wsat, Nx+1);
    frei(isat, Nx+1);
    frei(captherm, Nx+1);
    //gradients and such
    frei(gradH);
    frei(Hedge);
    frei(gradT, Nx+1);
    //trakers and snappers
    frei(evap);
    frei(evapw);
    frei(cumevap);
}

//------------------------------------------------------------------------------
//monitoring functions

double BousThermModel::total_evap () {
    //get the current total evaporation rate (m^2/s)
    double e = 0.0;
    for (long j=0; j<Nx; j++) e += evap[j];
    return(e);
}

double BousThermModel::total_evap_per_width () {
    //get the current total evaporation rate (m/2)
    double e = 0.0, w = 0.0;
    for (long j=0; j<Nx; j++) {
        if (evap[j] > 0.0) {
            e += evap[j];
            w += delx[j];
        }
    }
    return(e/w);
}

double BousThermModel::max_H_depth () {
    double *d = new double[Nx];
    for (long j=0; j<Nx; j++) d[j] = ztopc[j] - H[j];
    return( max(d,Nx) );
    delete [] d;
}

double BousThermModel::min_H_depth () {
    double *d = new double[Nx];
    for (long j=0; j<Nx; j++) d[j] = ztopc[j] - H[j];
    return( min(d,Nx) );
    delete [] d;
}

//------------------------------------------------------------------------------
//spatial discretization

double BousThermModel::f_K (double perm, double temp) {
    return( RHO_W*GRAV*perm/f_visc(temp) );
}

double BousThermModel::f_Kint (double aqbot, double zedge, double *Tin) {

    //if the bottom of the aquifer is above the water table, no mobility
    if (aqbot >= zedge) return(0.0);

    //find the deepest partially thawed cell
    long idxl = point_inside(ze, aqbot, Nz+1);
    //find the highest partially saturated cell
    long idxt = point_inside(ze, zedge, Nz+1);

    //integrate hydraulic conductivity
    double Kint = 0.0;
    //freezing point and water table are inside a single cell
    if (idxl == idxt) {
        Kint = (zedge - aqbot)*f_K(perm[idxl], Tin[idxl]);
    //freezing point and water table are in different cells
    } else if (idxt > idxl) {
        //lowest cell containing the freezing point
        Kint += (ze[idxl+1] - aqbot)*f_K(perm[idxl], Tin[idxl]);
        //intermediate cells
        for (long i=idxl+1; i<idxt; i++)
            Kint += delz[i]*f_K(perm[i], Tin[i]);
        //cell containing the water table
        Kint += (zedge - ze[idxt])*f_K(perm[idxt], Tin[idxt]);
    }

    return(Kint);
}

double BousThermModel::f_qT (double gradT) {
    return( -ktherm*gradT );
}

double BousThermModel::f_qH (double gradH, double Kint) {
    return( -gradH*Kint );
}

double BousThermModel::f_dTdt (double fl, double fr, double delz, double cap) {

    return( (fl - fr)/(delz*cap) );
}

double BousThermModel::f_dHdt (double fl, double fr, double po, double delx) {
    /*
    Compute the time derivative of the water table for a cell, given fluxes
    and other quantities
    inputs:
        fl - flux into the cell from the left edge (m^2/s)
        fr - flux out of the cell from the right edge (m^2/s)
        re - recharge into the cell (m/s)
        po - porosity at the current water table level (dimensionless)
        dx - width of the cell (m)
    returns:
        dHdt - the time derivative of the water table level (m/s)
    */

    return( (fl - fr)/(delx*po) );
}

void BousThermModel::update_sat (double zedge, double aqbot, double *Tin, double *wsat, double *isat) {

    long fidx = point_inside(ze, aqbot, Nz+1);

    for (long i=0; i<Nz; i++) {
        if ( zc[i] < zedge ) {
            if ( i == fidx) {
                isat[i] = (aqbot - ze[i])/delz[i];
                wsat[i] = 1.0 - isat[i];
            } else {
                if ( Tin[i] < TFREEZE ) {
                    isat[i] = 1.0;
                    wsat[i] = 0.0;
                } else {
                    isat[i] = 0.0;
                    wsat[i] = 1.0;
                }
            }
        } else {
            isat[i] = 0.0;
            wsat[i] = 0.0;
        }
    }
}

void BousThermModel::ode_fun (double *solin, double *fout) {

    //----------------------------------------------------------

    //aliases for the inputs
    Hin = solin;
    for (long j=0; j<Nx+1; j++)
        Tin[j] = solin + Nx + Nz*j;
    //aliases for the outputs
    dHdt = fout;
    for (long j=0; j<Nx+1; j++)
        dTdt[j] = fout + Nx + Nz*j;

    //----------------------------------------------------------

    //update surface temperature
    for (long j=0; j<Nx+1; j++)
        Tsurf[j] = f_surf_temp(get_t(), htope[j], stg->Ts0, stg->Tsf, stg->Tsgam, stg->TsLR);

    //hydraulic gradients and edge values
    //left edge
    gradH[0] = 0.0;
    Hedge[0] = Hin[0];
    //interior edges
    for (long j=1; j<Nx; j++) {
        gradH[j] = (Hin[j] - Hin[j-1])/(xc[j] - xc[j-1]);
        Hedge[j] = Hin[j-1] + gradH[j]*(xe[j] - xc[j-1]);
        //check the edge value is below the surface
        if ( Hedge[j] > ztope[j] )
            Hedge[j] = ztope[j];
    }
    //right edge
    gradH[Nx] = 0.0;
    Hedge[Nx] = Hin[Nx-1];

    //big parallel loop computes
    //  thermal gradients
    //  aquifer bottom points
    //  saturation fractions of water and ice
    //  updated thermal capacities
    //  thermal fluxes
    //  vertically integrated hydraulic conductivities
    //  hydraulic fluxes
    #pragma omp parallel for
    for (long j=0; j<Nx+1; j++) {
        //geothermal gradient
        gradT[j][0] = -stg->fTgeo/ktherm;
        //interior edges
        for (long i=1; i<Nz; i++)
            gradT[j][i] = (Tin[j][i] - Tin[j][i-1])/(zc[i] - zc[i-1]);
        //surface gradient
        gradT[j][Nz] = (Tsurf[j] - Tin[j][Nz-1])/(delz[Nz-1]/2.0);
        //find the aquifer bottom
        aqbot[j] = f_aquifer_bottom(Tin[j], Tsurf[j]);
        //update saturation fractions
        update_sat(Hedge[j] - ztope[j], aqbot[j], Tin[j], wsat[j], isat[j]);
        //compute thermal fluxes
        for (long i=0; i<Nz+1; i++)
            qT[j][i] = f_qT(gradT[j][i]);
        //update thermal capacities and compute thermal time derivatives
        for (long i=0; i<Nz; i++) {
            captherm[j][i] = f_captherm(poro[i], Tin[j][i], wsat[j][i], isat[j][i]);
            dTdt[j][i] = f_dTdt(qT[j][i], qT[j][i+1], delz[i], captherm[j][i]);
        }
        //compute hydraulic conductivity for the edge
        Kint[j] = f_Kint(aqbot[j], Hedge[j] - ztope[j], Tin[j]);
        //compute GW flux for the edge
        qH[j] = f_qH(gradH[j], Kint[j]);
    }

    //----------------------------------------------------------

    //compute hydraulic time derivatives
    for (long j=0; j<Nx; j++)
        dHdt[j] = f_dHdt(qH[j], qH[j+1], poro[point_inside(ze, Hin[j] - ztopc[j], Nz+1)], delx[j]);

}

void BousThermModel::update_evaporation () {
    //take water out of the top as necessary, as evaporation
    for (long j=0; j<Nx; j++) {
        if ( H[j] > ztopc[j] ) {
            //evaporation rate by area (m^2/s)
            evap[j] = (H[j] - ztopc[j])*delx[j]*poro_surf/get_dt();
            //evaporation rate by depth (m/s)
            evapw[j] = (H[j] - ztopc[j])*poro_surf/get_dt();
            //contribution to cumulative evaporation array (m^2)
            cumevap[j] += evap[j]*get_dt();
            //remove excess water
            H[j] = ztopc[j];
        } else {
            evap[j] = 0.0;
            evapw[j] = 0.0;
        }
    }
}

//------------------------------------------------------------------------------
//extras (which are still important to the integration process)

void BousThermModel::before_solve () {

    //zero out certain things before starting
    for (long j=0; j<Nx; j++) {
        evap[j] = 0.0;
        evapw[j] = 0.0;
        cumevap[j] = 0.0;
    }
    //write depth dependent physical params
    write_double(dirout + '/' + "poro", poro, Nz);
    write_double(dirout + '/' + "perm", perm, Nz);
    //start the clock
    start_time = omp_get_wtime();
}

void BousThermModel::after_step (double t) {

    (void)t; //suppress unused variable warning

    //update evaporation arrays
    update_evaporation();
    //apply maximum recharge if called for in settings
    if (stg->Rmax) for (long j=0; j<Nx; j++) H[j] = ztopc[j];
    //update output vectors
    o_t.push_back( get_t() );
    o_evap.push_back( total_evap() );
    o_evapw.push_back( total_evap_per_width() );
    o_maxaqbot.push_back( max(aqbot, Nx+1) );
    o_minaqbot.push_back( min(aqbot, Nx+1) );
}

void BousThermModel::after_snap (std::string dirout, long isnap, double t) {

    (void)t; //suppress unused variable warning

    //write a bunch of files
    std::string sisnap = std::to_string(isnap);
    write_double(dirout + '/' + "gradH_" + sisnap, gradH, Nx+1);
    write_double(dirout + '/' + "Hedge_" + sisnap, Hedge, Nx+1);
    write_double(dirout + '/' + "qH_" + sisnap, qH, Nx+1);
    write_double(dirout + '/' + "Kint_" + sisnap, Kint, Nx+1);
    write_double(dirout + '/' + "Tsurf_" + sisnap, Tsurf, Nx+1);
    write_double(dirout + '/' + "aqbot_" + sisnap, aqbot, Nx+1);
    write_double(dirout + '/' + "evap_" + sisnap, evap, Nx);
    write_double(dirout + '/' + "evapw_" + sisnap, evapw, Nx);
    write_double(dirout + '/' + "cumevap_" + sisnap, cumevap, Nx);
    write_doubles(dirout + '/' + "captherm_" + sisnap, captherm, Nx+1, Nz);
    write_doubles(dirout + '/' + "gradT_" + sisnap, gradT, Nx+1, Nz+1);
    write_doubles(dirout + '/' + "qT_" + sisnap, qT, Nx+1, Nz+1);
    write_doubles(dirout + '/' + "wsat_" + sisnap, wsat, Nx+1, Nz);
    write_doubles(dirout + '/' + "isat_" + sisnap, isat, Nx+1, Nz);
    //print some info
    int h, m;
    double s;
    double ttot = omp_get_wtime() - start_time;
    s2hms(omp_get_wtime() - start_time, &h, &m, &s);
    printf("      total wall time (HMS) ...... %02d:%02d:%04.1f\n", h, m, s);
    printf("      time steps taken ........... %llu\n", nstep_);
    printf("      average time per step ...... %g sec/step\n", ttot/double(get_nstep()));
    printf("      model time ................. %g yr\n", get_t()/YEAR_SEC);
    printf("      water table depth range .... [%.2e, %.2e] m\n", min_H_depth(), max_H_depth());
    printf("      hydraulic gradient range ... [%.2e, %.2e] %%\n", 100*absmin(gradH, Nx-1), 100*absmax(gradH, Nx-1));
    printf("      Kint range ................. [%.2e, %.2e] m^2/s\n", min(Kint, Nx+1), max(Kint, Nx+1));
    printf("      total evap ................. %g m^2/s\n", total_evap());
    printf("      total evap per width ....... %g m/s\n", total_evap_per_width());
    printf("      surface temp range ......... [%.2e, %.2e] K\n", min(Tsurf, Nx+1), max(Tsurf, Nx+1));
    printf("      temperature range .......... [%.2e, %.2e] K\n", min(T, Nx+1, Nz), max(T, Nx+1, Nz));
    printf("      max freezing point dep ..... %g m\n", absmax(aqbot, Nx+1));
    std::cout << std::endl;

    //check that the solution hasn't gone off the rails
    //if ( absmax(gradH, Nx+1) > 1.0 ) {
    //    std::cout << "FAILURE: Extremely high hydraulic gradient detected" << std::endl;
    //    exit(EXIT_FAILURE);
    //}
}

void BousThermModel::after_solve () {

    std::cout << "writing final output files" << std::endl;
    long unsigned n = stg->nmaxout;
    write_double_vec(dirout + '/' + "o_t", sub_vec(o_t, n));
    write_double_vec(dirout + '/' + "o_evap", sub_vec(o_evap, n));
    write_double_vec(dirout + '/' + "o_evapw", sub_vec(o_evapw, n));
    write_double_vec(dirout + '/' + "o_maxaqbot", sub_vec(o_maxaqbot, n));
    write_double_vec(dirout + '/' + "o_minaqbot", sub_vec(o_minaqbot, n));
}

//------------------------------------------------------------------------------
//dynamic instantiation function

BousThermModel init_model (std::string &griddir, Settings *stg, const char *dirout) {

    //number of vertical (z) nodes
    long Nz = read_one_long(griddir, "Nz.txt");
    //number of horizontal (x) nodes
    long Nx = read_one_long(griddir, "Nx.txt");
    //initialize a model object
    BousThermModel model(griddir, Nz*(Nx+1) + Nx, stg, dirout);
    //return the model
    return(model);
}
