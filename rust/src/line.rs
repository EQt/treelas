use crate::event::Event;

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
