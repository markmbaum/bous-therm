#ifndef BOUS_THERM_IO_H_
#define BOUS_THERM_IO_H_

//! \file bous_therm_io.h

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <fstream>

//------------------------------------------------------------------------------
//TEXT MANIPULATION FUNCTIONS

//!removes white space at the beginning and end of a string
/*!
\param[in] s string to strip of white space
*/
void strip_string (std::string &s);

//!splits a string on the first instance of a specified character
/*!
\param[in] c character to split on
\param[in] s string to split
\param[out] s1 first part of split string
\param[out] s2 second part of split string
*/
void split_string (char c, std::string &s, std::string &s1, std::string &s2);

//------------------------------------------------------------------------------
//READ FUNCTIONS

//!checks if a file can be opened for reading
/*!
\param[in] fn path to file to check
*/
void check_file_read (const char *fn);

//!reads a single integer out of a file and into a long type
/*!
\param[in] dir directory of target file
\param[in] fn name of target file
    \return integer found in file
*/
long read_one_long (const std::string &dir, const char *fn);

//!reads a binary file of doubles into an array
/*!
\param[in] dir directory of target file
\param[in] fn name of target file
\param[in] a an array to read numbers into
\param[in] size length of array, number of doubles to read
*/
void read_double (const std::string &dir, const std::string &fn, double *a, long size);

//!allocates an array of doubles and read a binary file into it
/*!
\param[in] dir directory of target file
\param[in] fn name of target file
\param[in] a an array to read numbers into
\param[in] size length of array, number of doubles to read
*/
double *alloc_read_double(const std::string &dir, const char *fn, double *a, long size);

//!a special function for reading a settings file into a vector of vectors of strings
std::vector< std::vector< std::string > > read_settings_file (const char *fn);

//------------------------------------------------------------------------------
//WRITE FUNCTIONS

//!checks if a file can be written to
void check_file_write (const char *fn);

//!writes an array of doubles to a binary file
/*!
\param[in] fn target file path
\param[in] a array of numbers to write
\param[in] size length of array, number of integers to read
*/
void write_double (const std::string &fn, double *a, long size);

//!writes a group of equally sized arrays to a single file
/*!
\param[in] fn target file pathS
\param[in] a array of pointers to equally sized arrays of doubles
\param[in] n length of a
\param[in] m length of arrays in a
*/
void write_doubles (const std::string &fn, double **a, long n, long m);

//!writes a vector of doubles to a binary file
/*!
\param[in] fn target file path
\param[in] v vector of numbers to write
*/
void write_double_vec (const std::string &fn, std::vector<double> v);

#endif
