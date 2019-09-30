//! Const Generics
//! ==============
//!
//! As long as [`#[feature(const_generics)]`][rfc] is unstable, we need a work around:
//! Define types with a unique value to be known at compile time.
//!
//! [rfc]: https://github.com/rust-lang/rfcs/blob/master/text/2000-const-generics.md

pub trait Bool {
    fn is_true() -> bool;
}

pub struct False;

impl Bool for False {
    fn is_true() -> bool {
        false
    }
}

pub struct True;

impl Bool for True {
    fn is_true() -> bool {
        true
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn _use<T: Bool>() -> bool {
        T::is_true()
    }

    #[test]
    fn test_use_bool() {
        assert!(!_use::<False>());
        assert!(_use::<True>());
    }
}
