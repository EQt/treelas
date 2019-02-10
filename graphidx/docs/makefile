.PHONY: cxx rust

cxx:
	doxygen
	sphinx-build -b html src/cxx html/cxx

rust:
	cd .. && cargo doc
	cp -R ../rust/target/doc/ html/rust
