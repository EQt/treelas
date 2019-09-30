use crate::generics::{Bool, False, True};
use crate::pwl::{clip, Event, EPS};
use std::ops::Range;

#[inline]
fn clamp(x: f64, min: f64, max: f64) -> f64 {
    let mut x = x;
    if x < min { x = min; }
    if x > max { x = max; }
    x
}

pub struct LineDP {
    lb: Vec<f64>,
    ub: Vec<f64>,
    event: Vec<Event>,
    pq: Range<usize>,
}

impl LineDP {
    pub fn new<'a>(n: usize) -> Self {
        let mut dp = LineDP {
            event: Vec::with_capacity(2 * n),
            lb: Vec::with_capacity(n - 1),
            ub: Vec::with_capacity(n - 1),
            pq: n..(n - 1),
        };
        dp.event.resize_with(2 * n, Default::default);
        dp.lb.resize(n, std::f64::NAN);
        dp.ub.resize(n, std::f64::NAN);
        return dp;
    }

    pub fn clip<F: Bool>(&mut self, slope: f64, offset: f64) -> f64 {
        clip::<F>(&mut self.event, &mut self.pq, slope, offset)
    }

    pub fn solve<W1, W2>(&mut self, x: &mut [f64], y: &[f64], lam: W1, mu: W2)
    where
        W1: graphidx::weights::Weighted<f64>,
        W2: graphidx::weights::Weighted<f64>,
    {
        let n = y.len();
        assert!(n == x.len());
        assert!(mu.len() >= n);
        assert!(lam.len() >= x.len() - 1);
        let mut lam0: f64 = 0.0;
        for i in 0..n {
            self.lb[i] =
                self.clip::<True>(mu[i], -mu[i] * y[i] - lam[i] + lam0);
            self.ub[i] =
                self.clip::<False>(-mu[i], mu[i] * y[i] - lam[i] + lam0);
            lam0 = if mu[i] > EPS {
                lam[i]
            } else {
                lam0.min(lam[i])
            };
        }
        x[n-1] = self.clip::<True>(mu[n-1], -mu[n-1] * y[n-1] - lam0 + 0.0);
        for i in (0..n-2).rev() {
            x[i] = clamp(x[i+1], self.lb[i], self.ub[i]);
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_line_3() {
        let y = vec![1., 2., 1.];
        let lam = graphidx::weights::ConstantWeights::new(0.1);
        let mu = graphidx::weights::ConstantWeights::new(1.0);
        let mut solver = LineDP::new(y.len());
        let mut x: Vec<f64> = Vec::with_capacity(y.len());
        x.resize(y.len(), std::f64::NAN);
        solver.solve(&mut x, &y, lam, mu);
        assert!(x == vec![1.1, 1.8, 1.1], "x = {:?}, lb = {:?}", x, solver.lb);
    }
}
