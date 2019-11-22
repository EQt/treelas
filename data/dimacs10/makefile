all: europe.h5 asia.h5 belgium.h5

unbz2: unbz2.cpp makefile
	$(CXX) -o $@ -O3 -Wall -pedantic -std=c++14 $< -lbz2 -mtune=native -flto

%.bz2:
	snakemake $<

%.h5: %.bz2
	julia convert.jl $< $@
