"""
Wrapper around the C++ libtreelas
"""
module Cxx

import Libdl

const BUILD_CONFIG = Sys.iswindows() ? "Release" : ""
const BUILD_DIR = joinpath(@__DIR__, "..", "deps", "build")
const lib = joinpath(BUILD_DIR, BUILD_CONFIG,
                     (Sys.isunix() ? "lib" : "") * "treelas.$(Libdl.dlext)")


function _find_python_extension()::String
    let regex = Regex("_treelas.+\\.$(Libdl.dlext)")
        _libs = filter(s -> match(regex, s) != nothing, readdir(BUILD_DIR))
    end
    @assert length(_libs) == 1
    return joinpath(BUILD_DIR, _libs[1])
end


function _find_all_tree_dp()
    ids = readlines(pipeline(`nm -D $(Cxx.lib)`, `grep tree_dp`, `awk '{print $3}'`))
    ids_cxx = map(s->readchomp(`c++filt $s`), ids)
    return ids, ids_cxx
end


function _find_tree_dp(; verbose=false)
    header = """
    const double*
    tree_dp<false, true>(
    const size_t n,
    double *x,
    const double *y,
    const int *parent,
    const double lam,
    const double mu,
    const int root
    );
    """

    header_lines = split(header, "\n")
    sig = findfirst(s -> occursin("tree_dp", s), header_lines)
    sig = header_lines[sig]
    ids, cxx = _find_all_tree_dp()
    i = findfirst(s -> occursin(sig, s), cxx)
    cxx = cxx[i]
    if verbose
        param_names = map(s -> replace(s, r".+[ *](\w+),?" => s"\1"),
                          header_lines[3:end-2])
        param_types = String.(split(replace(cxx, r".+\((.+)\)" => s"\1"), ", "))
        map!(param_types, param_types) do s::String
            for (a, b) in ["const" => "", " *" => "*", "unsigned long" => "size_t"]
                s = replace(s, a => b)
            end
            s
        end
        param_jltypes = map(param_types) do s
            if s[end] == '*'
                Ptr{eval(Symbol("C" * s[1:end-1]))}
            else
                eval(Symbol("C" * s))
            end
        end

        println("ccall(")
        println("    (:$(ids[i]), lib),")
        println("    Ref{Cdouble},")
        println("    (")
        for s in  param_types
            t = if s[end] == '*'
                "Ref{C" * s[1:end-1] * "}"
            else
                "C" * s
            end
            println("        ", t, ",")
        end
        println("    ),")
        for n in param_names
            println("    ", n, ",")
        end
        println(")")
    end
    return ids[i], cxx
end


function cxx_tree_dp(
    y::Array{Float64},
    parent::Vector{I},
    root::I,
    lam::Float64,
    mu::Float64 = 1.0,
) where {I}
    x = similar(y)
    @assert parent[root] == root
    n = length(y)
    @assert length(parent) == n
    parent = Vector{Cint}(copy(parent))
    parent .-= 1
    root -= 1
    ccall((:timer_disable, lib), Cvoid, ())
    ccall(
        (:tree_dp_f64_i32, lib),
        Ref{Cdouble},
        (
            Csize_t,
            Ref{Cdouble},
            Ref{Cdouble},
            Ref{Cint},
            Cdouble,
            Cdouble,
            Cint,
        ),
        n,
        x,
        y,
        parent,
        lam,
        mu,
        root,
    )
    return x
end


function cxx_tree_dual!(
    alpha::Array{Float64},
    parent::Vector{I},
    root::I,
) where {I}
    @assert parent[root] == root
    n = length(parent)
    @assert length(alpha) == n
    parent = Vector{Cint}(copy(parent))
    parent .-= 1
    root -= 1
    x = copy(alpha)
    post_order = Ptr{Cint}(0)
    ccall((:timer_disable, lib), Cvoid, ())
    ccall((:tree_dual_f64_i32, lib),
        Ref{Cdouble},
        (
            Csize_t,
            Ref{Cdouble},
            Ref{Cint},
            Ptr{Cint},
            Ref{Cdouble},
            Cint,
            Cuchar,
        ),
        n,
        x,
        parent,
        post_order,
        alpha,
        root,
        1,  # tree_orientation
    )
    return vec(alpha)
end


end
