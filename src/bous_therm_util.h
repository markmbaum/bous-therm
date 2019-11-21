#ifndef BOUS_THERM_UTIL_H_
#define BOUS_THERM_UTIL_H_

//! \file bous_therm_util.h

#include <cmath>
#include <vector>

#include <cstdio>

//!find max of array
double max (double *a, long n);
//!find max of 2D array
double max (double **a, long n, long m);

//!find min of array
double min (double *a, long n);
//!find min of ar2d ray
double min (double **a, long n, long m);

//!find abs max of array
double absmax (double *a, long n);
//!find abs max of array
double absmax (double **a, long n, long m);

//!find abs min of array
double absmin (double *a, long n);
//!find abs min of array
double absmin (double **a, long n, long m);

//!simple sum of doubles
double sum (double *a, long n);

//!check for nans in an array
bool has_nan (double *a, long n);

//!check for infs in an array
bool has_inf (double *a, long n);

//!check if two doubles are essentially equal
bool is_close (double a, double b, double thresh);

//!convert seconds to hr:min:sec
void s2hms (double sec, int *h, int *m, double *s);

//!subsample a vector of floats, keeping first and last elements
std::vector<double> sub_vec (std::vector<double> v, unsigned long n);

//!free an array
template <class T>
void frei (T *a) {
    delete [] a;
}

//!free an array of arrays
template <class T>
void frei (T **a, long n) {
    for (long i=0; i<n; i++)
        delete [] a[i];
    delete a;
}

#endif
