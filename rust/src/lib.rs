use graphidx;

pub struct TreeDP {
    post_order: Vec<u64>,
}

impl TreeDP {
    pub fn solve<W1, W2>(x: &mut [f64], y: &[f64], lam: W1, mu: W2)
    where
        W1: graphidx::weights::Weights<f64>,
    {
    }
}

pub struct Event {
    x: f64,
    slope: f64,
}

impl Default for Event {
    fn default() -> Self {
        Event {
            x: 0.0,
            slope: 0.0,
        }
    }
}

impl Event {
    fn offset(&self) -> f64 {
        -self.x / self.slope
    }
}

pub struct LineDP {
    event: Vec<Event>,
    lb: Vec<f64>,
    ub: Vec<f64>,
}


fn clip_front(e: &[Event], slope: f64, offset: f64) -> (f64, f64) {
    unimplemented!();
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

    pub fn solve<W1, W2>(x: &mut [f64], y: &[f64], lam: W1, mu: W2) {
    }
}

#[cfg(test)]
mod tests {
    // use super::*;

    #[test]
    fn test_hello() {
        assert!(true);
    }
}
