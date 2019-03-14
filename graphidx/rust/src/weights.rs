use std::ops::Index;

#[derive(PartialEq, Debug)]
struct ConstantWeights<T> {
    c: T,
}

impl<T> Index<usize> for ConstantWeights<T> {
    type Output = T;
    fn index(&self, _i: usize) -> &Self::Output {
        &self.c
    }
}

#[derive(PartialEq, Debug)]
struct ArrayWeights<T> {
    a: Vec<T>,
}

impl<T> Index<usize> for ArrayWeights<T> {
    type Output = T;
    fn index(&self, i: usize) -> &Self::Output {
        &self.a[i]
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn constant_weights_13() {
        let w = ConstantWeights { c: 13.5 };
        assert_eq!(w[5], 13.5);
        assert_eq!(w[0], 13.5);
        assert_eq!(w[13], 13.5);
    }

    #[test]
    fn array_weights_123() {
        let w = ArrayWeights { a : vec![1, 2, 5] };
        assert_eq!(w[2], 5);
        assert_eq!(w[0], 1);
        assert_eq!(w[1], 2);
        let result = std::panic::catch_unwind(|| w[3]);
        assert!(result.is_err());
    }
}
