use graphidx;

pub struct TreeDP {
    post_order: Vec<u64>,
}

impl TreeDP {
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
    fn test_treedp() {
        assert!(true);
    }
}
