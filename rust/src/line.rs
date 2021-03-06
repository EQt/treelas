use crate::float::Float;
use crate::generics::{Bool, False, True};
use crate::pwl::{clip, Event, EPS};
use graphidx::weights::Weighted;
use std::ops::Range;

pub struct LineDP<F: Float> {
    lb: Vec<F>,
    ub: Vec<F>,
    event: Vec<Event<F>>,
    pq: Range<usize>,
    startx: Option<F>,
}

type Forward = True;
type Reverse = False;

impl<F: Float> LineDP<F> {
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
        let startx = None;
        Self { lb, ub, event, pq, startx}
    }

    pub fn get_bounds(&self) -> (&[F], &[F]) {
        (&self.lb[..], &self.ub[..])
    }

    fn clip<D: Bool, W: Weighted<F>>(&mut self, slope: F, offset: F) -> F {
        if W::is_const() {
            clip::<F, D, False>(&mut self.event, &mut self.pq, slope, offset)
        } else {
            clip::<F, D, True>(&mut self.event, &mut self.pq, slope, offset)
        }
    }

    /// Dynamic Programming Forward Pass.
    pub fn dp_optimize<L, M>(&mut self, y: &[F], lam: &L, mu: &M) -> F
    where
        L: graphidx::weights::Weighted<F>,
        M: graphidx::weights::Weighted<F> + std::fmt::Debug,
    {
        let n = y.len();
        assert!(
            mu.len() >= n,
            "mu.len() = {}, n = {}: {:?}",
            mu.len(),
            n,
            mu
        );
        assert!(lam.len() >= n - 1);
        let mut lam0: F = 0.0.into();
        for i in 0..n - 1 {
            self.lb[i] =
                self.clip::<Forward, M>(mu[i], -mu[i] * y[i] - lam0 + lam[i]);
            self.ub[i] =
                self.clip::<Reverse, M>(-mu[i], mu[i] * y[i] - lam0 + lam[i]);
            lam0 = if M::is_const() || mu[i] > EPS.into() {
                lam[i]
            } else {
                lam0.min(lam[i])
            };
        }
        let s = self.clip::<Forward, M>(mu[n - 1], -mu[n - 1] * y[n - 1] - lam0);
        self.startx = Some(s);
        s
    }

    pub fn segments(&self) -> Vec<(Range<usize>, F)> {
        if let Some(mut x) = self.startx {
            let mut i = 0;
            let mut segs = Vec::new();
            for j in (0..self.lb.len()).rev() {
                if x > self.ub[j] {
                    segs.push((i..j, x));
                    x = self.ub[j];
                    i = j;
                } else if x < self.lb[j] {
                    segs.push((i..j, x));
                    x = self.lb[j];
                    i = j;
                }
            }
            segs.push((i..self.lb.len() + 1, x));
            segs
        } else {
            panic!("call LineDP::<_>::dp_optimize(...) first!");
        }
    }

    /// Store optimal solution in x
    pub fn solve<L, M>(&mut self, x: &mut [F], y: &[F], lam: &L, mu: &M)
    where
        L: graphidx::weights::Weighted<F>,
        M: graphidx::weights::Weighted<F> + std::fmt::Debug,
    {
        let n = y.len();
        assert!(n == x.len());

        x[n - 1] = self.dp_optimize(y, lam, mu);
        for i in (0..n - 1).rev() {
            x[i] = x[i + 1].clip(self.lb[i], self.ub[i]);
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

    #[test]
    fn test_line_3_f64() {
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
