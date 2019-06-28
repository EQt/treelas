use crate::pwl::{Event};

pub struct LineDP {
    event: Vec<Event>,
    lb: Vec<f64>,
    ub: Vec<f64>,
}

impl LineDP {
    pub fn new(n: usize) -> Self {
        let mut dp = LineDP {
            event: Vec::with_capacity(2*n),
            lb: Vec::with_capacity(n),
            ub: Vec::with_capacity(n),
        };
        dp.event.resize_with(2*n, Default::default);
        dp.lb.resize(n, std::f64::NAN);
        dp.ub.resize(n, std::f64::NAN);
        return dp;
    }

    pub fn solve<W1, W2>(x: &mut [f64], y: &[f64], lam: W1, mu: W2)
    where
        W1: graphidx::weights::Weights<f64>,
        W2: graphidx::weights::Weights<f64>,
    {
        let n = y.len();
        assert!(n == x.len());
        assert!(mu.len() >= n);
        assert!(x.len() - 1 >= lam.len());
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
