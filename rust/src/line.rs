use crate::pwl::{clip_front, Event};
use std::ops::Range;

struct PWL {
    offset: f64,
    r: Range<usize>,
}

impl PWL {
    fn clip_front(
        &mut self,
        event: &mut [Event],
        offset: f64,
        slope: f64,
    ) -> Option<f64> {
        let (o, s) =
            clip_front(&mut self.r, event, offset + self.offset, slope);
        self.offset += o;
        if s.abs() > 1e-9 {
            let x = -o / s;
            self.r.start -= 1;
            event[self.r.start] = Event {
                x: x,
                slope: s,
            };
            Some(x)
        } else {
            None
        }
    }
}

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
            pwl: PWL {
                offset: 0.0,
                r: Range {
                    start: n,
                    end: n - 1,
                },
            },
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
        for i in 0..n {
            self.lb[i] = self
                .pwl
                .clip_front(&mut self.event, -lam[i], mu[i])
                .unwrap_or(-std::f64::INFINITY);
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
