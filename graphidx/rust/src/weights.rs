//! Weight parameters associated to a collection, e.g. nodes or edges.
//!
//! There are mainly to different kinds of weights:
//!
//! 1. [`ConstantWeights`] provides the same weight for every element.
//!
//! 2. [`ArrayWeights`] provides access to a weight stored in a vector.
//!
//! Both classes provide access by index notation.
//!
//! [`ConstantWeights`]: struct.ConstantWeights.html
//! [`ArrayWeights`]: struct.ConstantWeights.html
use std::ops::Index;

pub trait Weights<T>: Index<usize> {}

/// Same weight for every element.
#[derive(PartialEq, Debug)]
pub struct ConstantWeights<T> {
    c: T,
}

impl<T> Index<usize> for ConstantWeights<T> {
    type Output = T;
    fn index(&self, _i: usize) -> &Self::Output {
        &self.c
    }
}

impl<T> Weights<T> for ConstantWeights<T> {
}

/// Weights stored in an array.
#[derive(PartialEq, Debug)]
pub struct ArrayWeights<T> {
    a: Vec<T>,
}

impl<T> Index<usize> for ArrayWeights<T> {
    type Output = T;
    fn index(&self, i: usize) -> &Self::Output {
        &self.a[i]
    }
}

impl<T> Weights<T> for ArrayWeights<T> {
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
        let w = ArrayWeights { a: vec![1, 2, 5] };
        assert_eq!(w[2], 5);
        assert_eq!(w[0], 1);
        assert_eq!(w[1], 2);
        let result = std::panic::catch_unwind(|| w[3]);
        assert!(result.is_err());
    }
}
