use crate::float::Float;
use crate::generics::{Bool, False, True};
use crate::pwl::{clip, Event};
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
        Self {
            lb,
            ub,
            event,
            pq,
            startx,
        }
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
    pub fn dp_optimize<L, M>(&mut self, y: &[F], lam: &L, mu: &M) -> &mut Self
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
        let mut lam0: F = 0.into();
        for i in 0..n - 1 {
            self.lb[i] = self.clip::<Forward, M>(
                mu[i].clone(),
                -mu[i].clone() * y[i].clone() - lam0.clone() + lam[i].clone(),
            );
            self.ub[i] = self.clip::<Reverse, M>(
                -mu[i].clone(),
                mu[i].clone() * y[i].clone() - lam0.clone() + lam[i].clone(),
            );
            lam0 = if M::is_const() || mu[i] > F::eps() {
                lam[i].clone()
            } else {
                lam0.min(lam[i].clone())
            };
        }
        let s = self.clip::<Forward, M>(
            mu[n - 1].clone(),
            -mu[n - 1].clone() * y[n - 1].clone() - lam0,
        );
        self.startx = Some(s);
        self
    }

    pub fn segment_iter(&self) -> impl Iterator<Item=(Range<usize>, F)> + '_ {
        if let Some(mut x) = self.startx.clone() {
            let n = self.lb.len() + 1;
            let mut i = n;
            (0..n).rev().filter_map(move |j| {
                if j == 0 {
                    Some((0..i, x.clone()))
                } else if x > self.ub[j-1] {
                    let n = (j..i, x.clone());
                    x = self.ub[j-1].clone();
                    i = j;
                    Some(n)
                } else if x < self.lb[j-1] {
                    let n = (j..i, x.clone());
                    x = self.lb[j-1].clone();
                    i = j;
                    Some(n)
                } else {
                    None
                }
            })
        } else {
            panic!("call LineDP::<_>::dp_optimize(...) first!");
        }
    }

    pub fn segments(&self) -> Vec<(Range<usize>, F)> {
        if let Some(mut x) = self.startx.clone() {
            let mut i = self.lb.len() + 1;
            let mut segs = Vec::new();
            for j in (0..self.lb.len()).rev() {
                if x > self.ub[j] {
                    segs.push((j + 1..i, x));
                    x = self.ub[j].clone();
                    i = j + 1;
                } else if x < self.lb[j] {
                    segs.push((j + 1..i, x));
                    x = self.lb[j].clone();
                    i = j + 1;
                }
            }
            segs.push((0..i, x));
            segs.reverse();
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
        self.dp_optimize(y, lam, mu);

        x[n - 1] = self.startx.as_ref().unwrap().clone();
        for i in (0..n - 1).rev() {
            x[i] = x[i + 1]
                .clone()
                .clip(self.lb[i].clone(), self.ub[i].clone());
        }
    }

    /// Compute the degrees of freedom of the current solution.
    /// [LineDP::dp_optimize] must be executed before.
    /// If not executed before, return `0usize`.
    pub fn dof(&self) -> usize {
        self.segments().len()
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use graphidx::weights::{Array, Const, Ones};

    #[test]
    fn test_line_3() {
        let y = vec![1., 2., 1.];
        let lam = Const::new(0.1);
        let mu = Const::new(1.0);
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
        let lam = Const::new(0.1);
        let mu = Const::new(1.0);
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

    fn round12(x: f64) -> f64 {
        let b = 1e12;
        (x * b).round() / b
    }

    #[test]
    fn test_segments_1() {
        let y = [1.0, 2.0, 1.0];
        let lam = Const::new(0.1);
        let mu = Ones::new();
        let mut segs = LineDP::new(y.len()).dp_optimize(&y, &lam, &mu).segments();
        segs.iter_mut().for_each(|(_, x)| *x = round12(*x));
        assert_eq!(segs, [(0..1, 1.1), (1..2, 1.8), (2..3, 1.1)]);
    }

    #[test]
    fn test_segments_2() {
        let y = [0.0, 3.0, 0.0];
        let lam = Const::new(1.0);
        let mu = Ones::new();
        let segs = LineDP::new(y.len()).dp_optimize(&y, &lam, &mu).segments();
        assert_eq!(segs, [(0..3, 1.0)]);
    }

    #[test]
    fn test_segments_3() {
        let y = [0.0, 0.0, 0.0, 2.0];
        let mu = Array::new(vec![1.0, 0.0, 0.0, 1.0]);
        let lam = Array::new(vec![1.0, 0.3, 1.0]);
        let mut segs = LineDP::new(y.len()).dp_optimize(&y, &lam, &mu).segments();
        segs.iter_mut().for_each(|(_, x)| *x = round12(*x));
        assert_eq!(segs, [(0..2, 0.3), (2..4, 1.7)]);
    }
    #[test]
    fn test_segments_iter() {
        let y = [0.0, 0.0, 0.0, 2.0];
        let mu = Array::new(vec![1.0, 0.0, 0.0, 1.0]);
        let lam = Array::new(vec![1.0, 0.3, 1.0]);
        let mut solver = LineDP::new(y.len());
        solver.dp_optimize(&y, &lam, &mu);
        let mut segs = solver.segments();
        segs.reverse();
        assert_eq!(solver.segment_iter().collect::<Vec<_>>(), segs);
    }

}
