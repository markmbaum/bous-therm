//! \file bous_therm_util.cc

#include "bous_therm_util.h"

double max (double *a, long n) {
    double r = 0.0;
    long i;
    //first find a minimum or let m stay at zero
    for (i=0; i<n; i++) if (a[i] < r) r = a[i];
    //now that m is less than or equal to the lowest element of a, find the max
    for (i=0; i<n; i++) if (a[i] > r) r = a[i];
    return(r);
}
double max (double **a, long n, long m) {
    double r = 0.0;
    long i,j;
    //first find a minimum or let m stay at zero
    for (i=0; i<n; i++)
        for (j=0; j<m; j++)
            if (a[i][j] < r) r = a[i][j];
    //now that m is less than or equal to the lowest element of a, find the max
    for (i=0; i<n; i++)
        for (j=0; j<m; j++)
            if (a[i][j] > r) r = a[i][j];
    return(r);
}

double min (double *a, long n) {
    double r = 0.0;
    long i;
    //first find a maximum or let m stay at zero
    for (i=0; i<n; i++) if (a[i] > r) r = a[i];
    //now that m is greater than or equal to the largest element of a, find the min
    for (i=0; i<n; i++) if (a[i] < r) r = a[i];
    return(r);
}
double min (double **a, long n, long m) {
    double r = 0.0;
    long i,j;
    //first find a maximum or let m stay at zero
    for (i=0; i<n; i++)
        for (j=0; j<m; j++)
            if (a[i][j] > r) r = a[i][j];
    //now that m is greater than or equal to the largest element of a, find the min
    for (i=0; i<n; i++)
        for (j=0; j<m; j++)
            if (a[i][j] < r) r = a[i][j];
    return(r);
}

double absmax (double *a, long n) {
    double r = 0.0;
    long i;
    //first find a minimum or let m stay at zero
    for (i=0; i<n; i++) if (fabs(a[i]) < r) r = fabs(a[i]);
    //now that m is less than or equal to the lowest element of a, find the max
    for (i=0; i<n; i++) if (fabs(a[i]) > r) r = fabs(a[i]);
    return(r);
}
double absmax (double **a, long n, long m) {
    double r = 0.0;
    long i,j;
    //first find a minimum or let m stay at zero
    for (i=0; i<n; i++)
        for (j=0; j<m; j++)
            if (fabs(a[i][j]) < r) r = fabs(a[i][j]);
    //now that m is less than or equal to the lowest element of a, find the max
    for (i=0; i<n; i++)
        for (j=0; j<m; j++)
            if (fabs(a[i][j]) > r) r = fabs(a[i][j]);
    return(r);
}

double absmin (double *a, long n) {
    double r = 0.0;
    long i;
    //first find a maximum or let m stay at zero
    for (i=0; i<n; i++) if (fabs(a[i]) > r) r = fabs(a[i]);
    //now that m is greater than or equal to the largest element of a, find the min
    for (i=0; i<n; i++) if (fabs(a[i]) < r) r = fabs(a[i]);
    return(r);
}
double absmin (double **a, long n, long m) {
    double r = 0.0;
    long i,j;
    //first find a maximum or let m stay at zero
    for (i=0; i<n; i++)
        for (j=0; j<m; j++)
            if (fabs(a[i][j]) > r) r = fabs(a[i][j]);
    //now that m is greater than or equal to the largest element of a, find the min
    for (i=0; i<n; i++)
        for (j=0; j<m; j++)
            if (fabs(a[i][j]) < r) r = fabs(a[i][j]);
    return(r);
}

double sum (double *a, long n) {
    double s=0.0;
    for (long i=0; i<n; i++) s += a[i];
    return(s);
}

bool has_nan (double *a, long n) {
    for (long i=0; i<n; i++) if (std::isnan(a[i])) return (true);
    return(false);
}

bool has_inf (double *a, long n) {
    for (long i=0; i<n; i++) if (!std::isfinite(a[i])) return (true);
    return(false);
}

bool is_close (double a, double b, double thresh) {
    //check equality
    if (a == b) return(true);
    //check signs
    if ((a > 0 && b > 0) || (a < 0 && b < 0)) {
        //remove signs
        double absa = fabs(a);
        double absb = fabs(b);
        double absd = fabs(a - b);
        //check relative differnence against a threshold
        if ((absd/absa < thresh) && (absd/absb < thresh)) {
            return(true);
        }
    }
    //otherwise the numbers aren't close
    return(false);
}

void s2hms (double sec, int *h, int *m, double *s) {

    //get hours
    *h = long(sec) / 3600;
    sec -= (*h)*3600;
    //get minutes
    *m = long(sec)/60;
    sec -= (*m)*60;
    //get remaining seconds
    *s = sec;
}

std::vector<double> sub_vec (std::vector<double> v, unsigned long n) {

    //calculate the approximate interval size
    unsigned long size = v.size();
    unsigned long step = size/n;
    //fill a new vector
    if ( (step == 0) || (step == 1) ) {
        //just copy the vector
        std::vector<double> r(v.begin(), v.end());
        //finish
        return(r);
    } else {
        //step is not zero, subsample the vector
        std::vector<double> r;
        r.push_back( v[0] );
        unsigned long i;
        for (i=step; i<size; i+=step) r.push_back(v[i]);
        if (i < size - 1) r.push_back( v.back() );
        //finish
        return(r);
    }
}
