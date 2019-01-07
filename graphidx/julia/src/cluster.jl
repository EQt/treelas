module Cluster
export cluster

import Random: randperm, MersenneTwister

srand = MersenneTwister


"""
    cluster(x, neigh, eps=1e-5, seed=42)

Find a partition of x such that
`abs(x[i] - x[j]) < eps`
for all edges `j in neighidx[i]` where `i` and `j` are in the same
partition.
"""
function cluster(x, neighidx::N;
                 eps::Float64=1e-5, seed=42)::PartitionIndex where N
    n = length(x)
    s = Vector{Int}()
    sizehint!(s, n)
    partition = fill(-1, n)
    nextp = 0
    for i in randperm(srand(seed), n)
        if partition[i] < 0     # unexplored?
            push!(s, i)
            nextp += 1
            while !isempty(s)
                v = pop!(s)
                partition[v] = nextp
                for u in neighidx[v]
                    if abs(x[v] - x[u]) < eps && partition[u] < 0
                        push!(s, u)
                    end
                end
            end
        end
    end
    return PartitionIndex(partition)
end


struct PartitionIndex
    I::Vector{Int}
    V::Vector{Int}
end


function PartitionIndex(ps::Vector{Int})
    k = maximum(ps)
    n = length(ps)
    V = Vector{Int}(undef, n)

    I = zeros(Int, k+1)
    for p in ps
        I[p] += 1
    end

    # prefix sum
    acc = 1
    j = 0
    jj = I[1]
    for i = 1:k
        I[i] = acc
        acc += j
        j, jj = jj, I[i+1]
    end
    I[k+1] = acc

    for (i, p) in enumerate(ps)
        V[I[p+1]] = i
        I[p+1] += 1
    end
    
    return PartitionIndex(I, V)
end


@inline Base.getindex(c::PartitionIndex, j::Int) =
    view(c.V, c.I[j]:c.I[j+1]-1)

Base.length(c::PartitionIndex) = length(c.I) -1

end
