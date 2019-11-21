#configuration file specifying compiler, flags, paths, etc.
include config.mk

#-------------------------------------------------------------------------------
#variables/lists
#non-class objects to be built, just functions
objs=bous_therm_io.o \
     bous_therm_param.o \
     bous_therm_util.o \
     bous_therm_settings.o
#model class objects to be built
cobjs=bous_therm_grid.o \
      bous_therm_numerics.o \
      bous_therm_model.o
#testing executables to be built
texecs=test_root.exe \
       test_quad.exe

#-------------------------------------------------------------------------------
#local directories

#source code directory
dirs=src
#test code directory
dirt=test
#built object directory
diro=obj
#built executable directory
dirb=bin
#flags for inclusion of ode header files
odesrc=-I$(odepath)/src
#flags for use of libode.a archive
odelib=-L$(odepath)/bin -lode

#-------------------------------------------------------------------------------
#prepend directories to names

o=$(patsubst %,$(diro)/%,$(objs))
no=$(patsubst %,$(diro)/%,$(nobjs))
co=$(patsubst %,$(diro)/%,$(cobjs))
te=$(patsubst %,$(dirb)/%,$(texecs))

#-------------------------------------------------------------------------------
#main targets

all: libodemake $(dirb)/bous_therm.exe

tests: $(te)

#-------------------------------------------------------------------------------
#rule for moving into the top directory of odelib and calling its makefile

libodemake:
	$(MAKE) -C $(odepath)

#-------------------------------------------------------------------------------
#compile objects

$(o): $(diro)/%.o: $(dirs)/%.cc $(dirs)/%.h
	$(cxx) $(flags) -o $@ -c $<

$(no): $(diro)/%.o: $(dirs)/%.cc $(dirs)/%.h
	$(cxx) $(flags) -o $@ -c $<

n=bous_therm_grid
$(diro)/$(n).o: $(dirs)/$(n).cc $(o)
	$(cxx) $(flags) -o $@ -c $< -I$(dirs)

n=bous_therm_numerics
$(diro)/$(n).o: $(dirs)/$(n).cc $(dirs)/$(n).h $(o) $(no) $(diro)/bous_therm_grid.o
	$(cxx) $(flags) $(omp) -o $@ -c $< -I$(dirs) $(odesrc) $(odelib)

n=bous_therm_model
$(diro)/$(n).o: $(dirs)/$(n).cc $(dirs)/$(n).h $(o) $(no) $(diro)/bous_therm_grid.o $(diro)/bous_therm_numerics.o
	$(cxx) $(flags) $(omp) -o $@ -c $< -I$(dirs) $(odesrc)

#-------------------------------------------------------------------------------
#compile executables

$(dirb)/bous_therm.exe: $(dirs)/main.cc $(o) $(no) $(co)
	$(cxx) $(flags) $(omp) -o $@ $< $(o) $(no) $(co) -I$(dirs) $(odesrc) $(odelib)

$(te): $(dirb)/%.exe: $(dirt)/%.cc $(no) $(o)
	$(cxx) $(flags) -o $@ $< $(no) $(o) -I$(dirs)

#-------------------------------------------------------------------------------
#other things

#target to remove all the compiled files
clean:
	rm  $(diro)/*.o $(dirb)/*.exe

.PHONY: clean
