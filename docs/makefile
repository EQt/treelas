.PHONY: julia

julia:
	julia --color=yes make.jl

formula.svg: formula_main.dvi
	dvisvgm -n -o $@ $<

%.dvi: %.tex
	latex $<

cxx:
	doxygen
	sphinx-build -b html src/cxx html/cxx
