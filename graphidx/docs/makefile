.PHONY: cxx rust

cxx:
	doxygen
	sphinx-build -b html src/cxx html/cxx

rust:
	cd $(realpath .)/.. && cargo doc
	cp -R $(realpath .)/../rust/target/doc/ html/rust

julia:
	julia --color=yes make.jl
