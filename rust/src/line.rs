use crate::pwl::{clip_front, Event};
use std::ops::Range;

pub struct LineDP {
    event: Vec<Event>,
    lb: Vec<f64>,
    ub: Vec<f64>,
}

impl LineDP {
    pub fn new(n: usize) -> Self {
        let mut dp = LineDP {
            event: Vec::with_capacity(2 * n),
            lb: Vec::with_capacity(n),
            ub: Vec::with_capacity(n),
        };
        dp.event.resize_with(2 * n, Default::default);
        dp.lb.resize(n, std::f64::NAN);
        dp.ub.resize(n, std::f64::NAN);
        return dp;
    }

    pub fn solve<W1, W2>(&mut self, x: &mut [f64], y: &[f64], lam: W1, mu: W2)
    where
        W1: graphidx::weights::Weights<f64>,
        W2: graphidx::weights::Weights<f64>,
    {
        let n = y.len();
        assert!(n == x.len());
        assert!(mu.len() >= n);
        assert!(x.len() - 1 >= lam.len());
        let mut r = Range { start: n, end: n-1 };
        let mut offset = 0.0;
        for i in 0..n {
            let (o, s) = clip_front(&mut r, &self.event, mu[i], offset - lam[i]);
            offset += o;
            r.start -= 1;
            if s.abs() > 1e-9 {
                let x = -o/s;
                self.event[r.start] = Event{ x: x, slope: s };
                self.lb[i] = x;
            }
        }
        unimplemented!();
    }
}

#[cfg(test)]
mod tests {
    // use super::*;

    #[test]
    fn test_linedp() {
        assert!(true);
    }
}
