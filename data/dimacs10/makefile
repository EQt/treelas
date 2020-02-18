GINC = ../../deps/graphidx/cxx/include
MINC = ../../deps/minih5/include
H5INC = /usr/include/hdf5/serial/
H5LIB = -L/usr/lib/x86_64-linux-gnu/hdf5/serial/ -lhdf5 -lhdf5_hl
FLAGS = -I$(GINC) -I$(MINC) -I$(H5INC) $(H5LIB)

all: europe.h5 asia.h5 belgium.h5

%.bz2:
	snakemake $@

%.h5: %.bz2
	julia convert.jl $< $@
