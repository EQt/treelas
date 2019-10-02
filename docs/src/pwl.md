# Piece-wise Linear Functions

The dynamic programming algorithm computes for each node `i` (from bottom leaf to top root) the derivatives of the objective function restricted to the subtree starting at  `i`.
It turns out that the derivative is always a piecewise-linear function.
Therefore it suffices to store the kinks.

Passing over a kink is called an event; the information that defines it is stored in
```@docs
TreeLas.Pwl
TreeLas.Pwl.Event
```
The change (Delta) in the intercept is given by
```@docs
TreeLas.Pwl.intercept
```
