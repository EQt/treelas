.PHONY: julia

julia:
	julia --color=yes make.jl

rust:
	cd $(realpath .)/.. && cargo doc
	mkdir -p html/rust
	cp -R $(realpath .)/../rust/target/doc/* html/rust/

formula.svg: formula_main.dvi
	dvisvgm -n -o $@ $<

%.dvi: %.tex
	latex $<

cxx:
	doxygen
	sphinx-build -b html src/cxx html/cxx

python:
	sphinx-build -b html src/python html/python
