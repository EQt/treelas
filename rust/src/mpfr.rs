use crate::float;
pub use rug::Rational;

impl float::Float for Rational {
    fn eps() -> Self {
        Self::from_f32(1e-9).unwrap()
    }
}
