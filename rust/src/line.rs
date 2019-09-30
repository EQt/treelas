use crate::pwl::{Event, PWL};

pub struct LineDP {
    lb: Vec<f64>,
    ub: Vec<f64>,
    event: Vec<Event>,
    pwl: PWL,
}

impl LineDP {
    pub fn new<'a>(n: usize) -> Self {
        let mut dp = LineDP {
            event: Vec::with_capacity(2 * n),
            lb: Vec::with_capacity(n),
            ub: Vec::with_capacity(n),
            pwl: PWL::new(n),
        };
        dp.event.resize_with(2 * n, Default::default);
        dp.lb.resize(n, std::f64::NAN);
        dp.ub.resize(n, std::f64::NAN);
        return dp;
    }

    pub fn solve<W1, W2>(&mut self, x: &mut [f64], y: &[f64], lam: W1, mu: W2)
    where
        W1: graphidx::weights::Weighted<f64>,
        W2: graphidx::weights::Weighted<f64>,
    {
        let n = y.len();
        assert!(n == x.len());
        assert!(mu.len() >= n);
        assert!(x.len() - 1 >= lam.len());
        for i in 0..n {
            self.lb[i] = self
                .pwl
                .clip_front(&mut self.event, -lam[i], mu[i])
                .unwrap_or(-std::f64::INFINITY);

            self.ub[i] = self
                .pwl
                .clip_back(&mut self.event, lam[i], mu[i])
                .unwrap_or(std::f64::INFINITY);
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
