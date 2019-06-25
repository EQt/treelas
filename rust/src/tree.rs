use graphidx;

pub struct TreeDP {
    post_order: Vec<u64>,
}

impl TreeDP {
    pub fn solve<W1, W2>(x: &mut [f64], y: &[f64], lam: W1, mu: W2)
    where
        W1: graphidx::weights::Weights<f64>,
    {
        unimplemented!();
    }
}
