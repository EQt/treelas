use crate::generics::{Bool, False, True};
use crate::pwl_32::{clip, Event, EPS};
use graphidx::weights::Weighted;
use std::ops::Range;

#[inline]
fn clamp(mut x: f32, min: f32, max: f32) -> f32 {
    debug_assert!(min <= max);
    x = if x < min { min } else { x };
    x = if x > max { max } else { x };
    x
}

pub struct LineDP {
    lb: Vec<f32>,
    ub: Vec<f32>,
    event: Vec<Event>,
    pq: Range<usize>,
}

type Forward = True;
type Reverse = False;

impl LineDP {
    pub fn new(n: usize) -> Self {
        let mut event = Vec::with_capacity(2 * n);
        let mut lb = Vec::with_capacity(n - 1);
        let mut ub = Vec::with_capacity(n - 1);
        let pq = n..n;
        unsafe {
            event.set_len(2 * n);
            lb.set_len(n - 1);
            ub.set_len(n - 1);
        }
        Self { lb, ub, event, pq }
    }

    fn clip<F: Bool, W: Weighted<f32>>(&mut self, slope: f32, offset: f32) -> f32 {
        if W::is_const() {
            clip::<F, False>(&mut self.event, &mut self.pq, slope, offset)
        } else {
            clip::<F, True>(&mut self.event, &mut self.pq, slope, offset)
        }
    }

    pub fn solve<L, M>(&mut self, x: &mut [f32], y: &[f32], lam: &L, mu: &M)
    where
        L: graphidx::weights::Weighted<f32>,
        M: graphidx::weights::Weighted<f32> + std::fmt::Debug,
    {
        let n = y.len();
        assert!(n == x.len());
        assert!(
            mu.len() >= n,
            "mu.len() = {}, n = {}: {:?}",
            mu.len(),
            n,
            mu
        );
        assert!(lam.len() >= x.len() - 1);
        let mut lam0: f32 = 0.0;
        for i in 0..n - 1 {
            self.lb[i] = self.clip::<Forward, M>(mu[i], -mu[i] * y[i] - lam0 + lam[i]);
            self.ub[i] = self.clip::<Reverse, M>(-mu[i], mu[i] * y[i] - lam0 + lam[i]);
            lam0 = if M::is_const() || mu[i] > EPS {
                lam[i]
            } else {
                lam0.min(lam[i])
            };
        }
        x[n - 1] = self.clip::<Forward, M>(mu[n - 1], -mu[n - 1] * y[n - 1] - lam0 + 0.0);
        for i in (0..n - 1).rev() {
            x[i] = clamp(x[i + 1], self.lb[i], self.ub[i]);
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_line_3() {
        let y = vec![1., 2., 1.];
        let lam = graphidx::weights::Const::new(0.1);
        let mu = graphidx::weights::Const::new(1.0);
        let mut solver = LineDP::new(y.len());
        let mut x: Vec<f32> = Vec::with_capacity(y.len());
        x.resize(y.len(), std::f32::NAN);
        solver.solve(&mut x, &y, &lam, &mu);
        assert!(graphidx::lina::l1_diff(&x, &[0.0, 0.0, 1.0]) > 1.0);
        let diff: f32 = graphidx::lina::l1_diff(&x, &[1.1, 1.8, 1.1]);
        assert!(
            diff <= 2e-7,
            "diff = {:e}, x = {:?}, lb = {:?}, ub = {:?}",
            diff,
            x,
            solver.lb,
            solver.ub
        );
    }
}
