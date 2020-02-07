#TEMPLATE MAKEFILE CONFIG

#-------------------------------------------------------------------------------
#these variables may need to be modified

#compiler
cxx=g++ # <--- GNU C++ compiler
#cxx=icpc # <--- Intel C++ compiler

#standard compilation flags
flags=-std=c++11 -Wall -Wextra -pedantic -O3 # <--- GNU compiler flags
#cflags=-std=c++11 -Wall -O3 # <--- Intel compiler flags

#compilation flag for openmp
omp=-fopenmp
#omp=-qopenmp

#top directory of ode library for solver classes (libode)
odepath=<path/to/libode>
