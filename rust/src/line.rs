use crate::generics::{Bool, False, True};
use crate::instance::{Instance, Weights};
use crate::pwl::{clip, Event, EPS};
use graphidx::weights::Weighted;
use std::ops::Range;

#[inline]
fn clamp(mut x: f64, min: f64, max: f64) -> f64 {
    debug_assert!(min <= max);
    x = if x < min { min } else { x };
    x = if x > max { max } else { x };
    x
}

pub struct LineDP {
    lb: Vec<f64>,
    ub: Vec<f64>,
    event: Vec<Event>,
    pq: Range<usize>,
}

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

    fn clip<F: Bool, W: Weighted<f64>>(&mut self, slope: f64, offset: f64) -> f64 {
        if W::is_const() {
            clip::<F, False>(&mut self.event, &mut self.pq, slope, offset)
        } else {
            clip::<F, True>(&mut self.event, &mut self.pq, slope, offset)
        }
    }

    pub fn solve<L, M>(&mut self, x: &mut [f64], y: &[f64], lam: &L, mu: &M)
    where
        L: graphidx::weights::Weighted<f64>,
        M: graphidx::weights::Weighted<f64> + std::fmt::Debug,
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
        let mut lam0: f64 = 0.0;
        for i in 0..n - 1 {
            self.lb[i] = self.clip::<True, M>(mu[i], -mu[i] * y[i] - lam0 + lam[i]);
            self.ub[i] = self.clip::<False, M>(-mu[i], mu[i] * y[i] - lam0 + lam[i]);
            lam0 = if M::is_const() || mu[i] > EPS {
                lam[i]
            } else {
                lam0.min(lam[i])
            };
        }
        x[n - 1] = self.clip::<True, M>(mu[n - 1], -mu[n - 1] * y[n - 1] - lam0 + 0.0);
        for i in (0..n - 1).rev() {
            x[i] = clamp(x[i + 1], self.lb[i], self.ub[i]);
        }
    }

    pub fn solve_instance(&mut self, mut x: &mut [f64], inst: &Instance) {
        use graphidx::weights::{ArrayRef, Const, Ones};

        let mu: Option<&Weights<f64>> = inst.mu.as_ref();
        let lam: &Weights<f64> = &inst.lam;
        match (lam, mu) {
            (Weights::Const(lam), Some(Weights::Const(mu))) => {
                let lam: Const<_> = lam.into();
                let mu: Const<_> = mu.into();
                self.solve(&mut x, &inst.y, &lam, &mu);
            }
            (Weights::Array(lam), Some(Weights::Const(mu))) => {
                let lam: ArrayRef<_> = lam.into();
                let mu: Const<_> = mu.into();
                self.solve(&mut x, &inst.y, &lam, &mu);
            }
            (Weights::Const(lam), Some(Weights::Array(mu))) => {
                let lam: Const<_> = lam.into();
                let mu: ArrayRef<_> = mu.into();
                self.solve(&mut x, &inst.y, &lam, &mu);
            }
            (Weights::Array(lam), Some(Weights::Array(mu))) => {
                let mu: ArrayRef<_> = mu.into();
                let lam: ArrayRef<_> = lam.into();
                self.solve(&mut x, &inst.y, &lam, &mu);
            }
            (Weights::Array(lam), None) => {
                let mu: Ones<f64> = Ones::default();
                let lam: ArrayRef<_> = lam.into();
                self.solve(&mut x, &inst.y, &lam, &mu);
            }
            (Weights::Const(lam), None) => {
                let mu: Ones<f64> = Ones::default();
                let lam: Const<_> = lam.into();
                self.solve(&mut x, &inst.y, &lam, &mu);
            }
        };
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
        let mut x: Vec<f64> = Vec::with_capacity(y.len());
        x.resize(y.len(), std::f64::NAN);
        solver.solve(&mut x, &y, &lam, &mu);
        assert!(graphidx::lina::l1_diff(&x, &[0.0, 0.0, 1.0]) > 1.0);
        let diff: f64 = graphidx::lina::l1_diff(&x, &[1.1, 1.8, 1.1]);
        assert!(
            diff <= 1e-8,
            "diff = {}, x = {:?}, lb = {:?}, ub = {:?}",
            diff,
            x,
            solver.lb,
            solver.ub
        );
    }
}
