julia -e "import Pkg; Pkg.develop(Pkg.PackageSpec(path=\`"deps/graphidx/julia\`"))"
julia -e "import Pkg; Pkg.develop(Pkg.PackageSpec(path=\`"julia\`"))"
julia -e "import Pkg; Pkg.instantiate()"
julia -e "import Pkg; Pkg.build(\`"TreeLas\`")"
type C:\projects\treelas\julia\deps\build.log
