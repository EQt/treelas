"""
Wrapper around the C++ libtreelas
"""
module Cxx

import Libdl


function find_library()::String
    BUILD_DIR = joinpath(@__DIR__, "..", "deps", "build")
    _libs = filter(s -> match(Regex("_treelas.+\\.$(Libdl.dlext)"), s) != nothing, readdir(BUILD_DIR))
    @assert length(_libs) == 1
    return joinpath(BUILD_DIR, _libs[1])
end


const lib = find_library()


function find_tree_dp()
    ids = readlines(pipeline(`nm -D $(Cxx.lib)`, `grep tree_dp`, `awk '{print $3}'`))
    ids_cxx = map(s->readchomp(`c++filt $s`), ids)
end

end



