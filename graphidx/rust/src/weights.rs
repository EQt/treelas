use std::ops::Index;

#[derive(PartialEq, Debug)]
struct ConstantWeights {
    c: f64,
}

impl Index<usize> for ConstantWeights {
    type Output = f64;
    fn index(&self, _i: usize) -> &Self::Output {
        &self.c
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn call_constant_weights() {
        let w = ConstantWeights { c: 13.5 };
        assert_eq!(w[5], 13.5);
    }
}
