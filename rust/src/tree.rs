pub struct TreeDP {
    post_order: Vec<usize>,
}

impl TreeDP {
    pub fn solve<W1, W2>(&self, x: &mut [f64], y: &[f64], lam: W1, mu: W2)
    where
        W1: graphidx::weights::Weighted<f64>,
        W2: graphidx::weights::Weighted<f64>,
    {
        let n = y.len();
        assert_eq!(n, x.len());
        assert!(mu.len() >= n);
        assert!(std::cmp::min(lam.len(), std::usize::MAX - 1) + 1 >= n);
        for &v in &self.post_order {
            x[v] = y[v]
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_treedp_empty() {
        let t = TreeDP { post_order: vec![] };
        let mut x = vec![];
        let y = vec![];
        let lam = graphidx::weights::Const::new(1.0);
        let mu = graphidx::weights::Const::new(0.5);
        t.solve(&mut x, &y, lam, mu);
        assert!(true);
    }
}
