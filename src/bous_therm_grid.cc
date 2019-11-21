//! \file bous_therm_grid.cc

#include "bous_therm_grid.h"

BousThermGrid::BousThermGrid (const std::string &griddir_) {

    //store the path to the grid directory
    griddir = griddir_;

    std::cout << "loading grid information from directory: " << griddir << std::endl;

    //--------------------------------------------------------------------------
    //read in all the grid info

    // --- Z ---
    //number of vertical nodes
    Nz = read_one_long(griddir, "Nz.txt");
    //cell edge coordinates
    ze = alloc_read_double(griddir, "ze", ze, Nz+1);
    //cell center coordinates
    zc = alloc_read_double(griddir, "zc", zc, Nz);
    //z domain depth
    zdepth = ze[0];
    //z cell widths
    delz = alloc_read_double(griddir, "delz", delz, Nz);

    // --- X ---
    //number of horizontal cells
    Nx = read_one_long(griddir, "Nx.txt");
    //cell edge coordinates
    xe = alloc_read_double(griddir, "xe", xe, Nx+1);
    //cell center coordinates
    xc = alloc_read_double(griddir, "xc", xc, Nx);
    //boundary coordinates
    xa = xe[0];
    xb = xe[Nx];
    //cell widths
    delx = alloc_read_double(griddir, "delx", delx, Nx);

    //topography
    ztope = alloc_read_double(griddir, "ztope", ztope, Nx+1);
    ztopc = alloc_read_double(griddir, "ztopc", ztopc, Nx);
    htope = new double[Nx+1];
    double ztopemin = min(ztope, Nx+1);
    for (long j=0; j<Nx+1; j++)
        htope[j] = ztope[j] - ztopemin;

    //-----------------------------------------------------------

    std::cout << "grid loaded" << std::endl;
    printf("  z domain is [%g,%g] with %li nodes\n", 0.0, zdepth, Nz);
    printf("  x domain is [%g,%g] with %li cells\n", xa, xb, Nx);
}

BousThermGrid::~BousThermGrid () {

    //free the x coordinates
    frei(ze);
    frei(zc);
    frei(delz);
    //free the x coordinates
    frei(xe);
    frei(xc);
    frei(delx);
    //free topography arrays
    frei(ztope);
    frei(ztopc);
    frei(htope);
}
