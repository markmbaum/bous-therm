//! \file bous_therm_settings.cc

#include "bous_therm_settings.h"

bool eval_txt_bool (const char *s) {
    if (strcmp(s, "true") == 0 ) return(true);
    return(false);
}

bool cmp (const char *s1, const char *s2) {
    if ( strcmp(s1, s2) == 0 ) return(true);
    return(false);
}

long to_long(const char *val) {
    //convert a string to a long int, handling scientific notation
    return( long(std::atof(val)) );
}

Settings parse_settings ( std::vector< std::vector< std::string > > sv ) {

    Settings s;
    const char *set, *val;

    for (int i=0; i < int(sv.size()); i++) {

        //get the setting and value pair
        set = sv[i][0].c_str();
        val = sv[i][1].c_str();

        //get the setting

        if      ( cmp(set, "nstep") )   s.nstep   = to_long(val);
        else if ( cmp(set, "tend") )    s.tend    = std::atof(val);
        else if ( cmp(set, "tunit") )   s.tunit   = std::atof(val);
        else if ( cmp(set, "nsnap") )   s.nsnap   = to_long(val);
        else if ( cmp(set, "nmaxout") ) s.nmaxout = to_long(val);

        else if ( cmp(set, "Hdep0") )   s.Hdep0   = std::atof(val);
        else if ( cmp(set, "Rmax") )    s.Rmax    = std::atoi(val);
        else if ( cmp(set, "poro0") )   s.poro0   = std::atof(val);
        else if ( cmp(set, "porogam") ) s.porogam = std::atof(val);
        else if ( cmp(set, "perm0") )   s.perm0   = std::atof(val);
        else if ( cmp(set, "permgam") ) s.permgam = std::atof(val);
        else if ( cmp(set, "kTr") )     s.kTr     = std::atof(val);
        else if ( cmp(set, "fTgeo") )   s.fTgeo   = std::atof(val);
        else if ( cmp(set, "Ts0") )     s.Ts0     = std::atof(val);
        else if ( cmp(set, "Tsf") )     s.Tsf     = std::atof(val);
        else if ( cmp(set, "Tsgam") )   s.Tsgam   = std::atof(val);
        else if ( cmp(set, "TsLR") )    s.TsLR    = std::atof(val);

        else {
            std::cout << "FAILURE: unknown setting in settings file: " << set << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    return(s);
}
