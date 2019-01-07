max_gap_tree(y::Matrix{Float64}, g::FLSA.ImgGraph; params...) =
    reshape(max_gap_tree(vec(y), g; params...), g.n1, g.n2)

ONE_FUNCTION = i -> 1.0

function max_gap_tree(y::Vector{Float64}, g::FLSA.ImgGraph;
                      c0::Real = 0.0,
                      dual_mu::Bool = false,
                      mu_f::Function = ONE_FUNCTION,
                      mu::Vector{Float64} = Vector{Float64}(),
                      alpha::Vector{Float64} = Vector{Float64}(),
                      eps_gap::Float64 = 1e-14,
                      max_iter::Integer=1,
                      logger::LoggerT = LoggerT(),
                      random_tree::Bool=false,
                      abs_tree::Bool=false,
                      verbose::Bool=true,
                      process::Function=x->nothing,
                      dprocess::Function=α->nothing,
                      assert_decreasing::Bool=false,
                      verbose_debug::Bool=false,
                      root_node::Int = 1)
    function logg(msg...)
        if verbose_debug
            println(msg...)
        end
    end


    x = copy(y)
    if length(alpha) <= 0
        alpha = c0 * sign.(g.D*vec(y))
        logg("sign alpha")
    else
        x = vec(y) - g.D' * alpha
        logg("predfined x")
    end
    total = 0.0
    gap = Inf
    tic()
    for it = 0:max_iter
        if gap <= eps_gap
            break
        end
        if verbose
            time = toq()
            total += time
            _field(logger, "time", time)
            _field(logger, "flsa", dual_mu ? flsa0(x, y, g.D, mu)
                                           : flsa(x, y, g.D))
            gap = FLSA.duality_gap(vec(y), alpha, g)
            _field(logger, "gap", gap)
            _field(logger, "dual", FLSA.dual_obj(alpha, y, g.D))
            process(x)
            dprocess(alpha)
            println(@sprintf("%4d %f %f %f",
                             it,
                             logger["flsa"][end],
                             logger["dual"][end],
                             logger["gap"][end]))
            if assert_decreasing && length(logger["flsa"]) >= 2
                @assert logger["flsa"][end] <= logger["flsa"][end-1]
            end
        end
        it >= max_iter && break

        tic()
        if random_tree
            weights = randn(num_edges(g.graph))
        elseif abs_tree
            weights = abs(alpha)
        else
            weights = - FLSA.gap_vec(vec(y), alpha, g)
        end
        logg("weights")
        mst, wmst = kruskal_minimum_spantree(g.graph, weights)

        logg("tree")
        t = FLSA.subtree(g.graph, mst, root_node)
        logg("created subtree")
        z = vec(y) - FLSA.non_tree(g.D, mst)'*alpha
        logg("non_tree")
        Lam = fill(Inf, length(y))
        for e in mst
            v, u = source(e), target(e)
            if t.parent[v] == u
                Lam[v] = g.lambda[e.index]
            else
                Lam[u] = g.lambda[e.index]
            end
        end
        logg("sub_lambda")

        x = if length(mu) > 0
            FLSA.dp_tree(z, Lam, mu, t)
        else
            FLSA.dp_tree(z, i->Lam[i], mu_f, t)
        end
        @assert all(isfinite.(x))
        @assert !any(isnan.(x))
        logg("dp_tree")
        alpha_t = dual_mu ? FLSA.dual_tree0(z, x, t, mu_f) :
                            FLSA.dual_tree(z, x, t)
        @assert all(isfinite.(alpha_t))
        @assert !any(isnan.(alpha_t))
        logg("dual_tree: \n$(alpha_t[1:min(5, length(alpha_t))])")

        for (i,e) in enumerate(mst)
            alpha[e.index] = alpha_t[i] / g.lambda[e.index]
            @assert abs(g.lambda[e.index]) > 1e-15 (@val e.index)
        end
        @assert all(isfinite.(alpha))
        @assert !any(isnan.(alpha))
    end
    return x
end
