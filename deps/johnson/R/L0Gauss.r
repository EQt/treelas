L0_GAUSS = as.integer(0)
L0_POIS = as.integer(1)
POIS_LIK = as.integer(0)
GAUSS_LIK = as.integer(1)

L0GaussByNseg = function(x, nSegs = 3, obsWts = NULL, maxSegs = 3000, avgNumBackptrSegs = 100) {
    n = length(x)
    vitTerms = matrix(nrow = 2, ncol = n, data = -1.0)
    vitTerms[1, ] = 2.0 * x

    if (!is.null(obsWts)) {
        vitTerms = vitTerms * obsWts
    }

    z = rep(0.0, n)
    .Call("EfamL0VitByNseg", vitTerms, nSegs, z, maxSegs, avgNumBackptrSegs)
    return(z)
}

L0Gauss = function(x, lambda2 = NULL, obsWts = NULL, maxSegs = 3000, avgNumBackptrSegs = 100) {
    n = length(x)
    vitTerms = matrix(nrow = 2, ncol = n, data = -1.0)
    vitTerms[1, ] = 2.0 * x

    if (!is.null(obsWts)) {
        vitTerms = vitTerms * obsWts
    }

    if (is.null(lambda2)) lambda2 = log(n)

    z = rep(0.0, n)
    .Call("EfamL0Vit", vitTerms, lambda2, z, maxSegs, avgNumBackptrSegs)
    return(z)
}

L0DpGaussByNseg = function(x, nSegs, expanded = FALSE) {
    #' Bellman's algorithm
    if (!is.double(x)) {
        x = as.double(x)
    }
    r = .Call("L0DpSegment", x, GAUSS_LIK, nSegs) + as.integer(1)

    if (expanded) {
        v = c(0, r, length(x))

        r2 = rep(0, length(x))
        for (k in 2:length(v)) {
            i1 = (v[k-1]+1):v[k]
            r2[i1] = mean(x[i1])
        }
        return(r2)
    } else {
        return(r)
    }
}
