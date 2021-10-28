/// All floating point traits needed to do fused Lasso
pub trait Float:
    std::ops::Mul<Output = Self>
    + std::ops::Div<Output = Self>
    + std::ops::Add<Output = Self>
    + std::ops::Sub<Output = Self>
    + std::ops::Neg<Output = Self>
    + std::cmp::PartialOrd
    + std::ops::AddAssign
    + From<i16>
    + Clone
{
    fn eps() -> Self;

    #[inline]
    fn infinity() -> Self {
        Self::from(1) / Self::from(0)
    }

    #[inline]
    fn abs(self) -> Self {
        if self < 0.into() {
            -self
        } else {
            self
        }
    }

    #[inline]
    fn clip(mut self, min: Self, max: Self) -> Self {
        debug_assert!(min <= max);
        self = if self < min { min } else { self };
        self = if self > max { max } else { self };
        self
    }

    #[inline]
    fn min(self, other: Self) -> Self {
        if self < other {
            self
        } else {
            other
        }
    }
}

impl<T> Float for T
where
    T: std::ops::Mul<Output = Self>
        + std::ops::Div<Output = Self>
        + std::ops::Add<Output = Self>
        + std::ops::Sub<Output = Self>
        + std::ops::Neg<Output = Self>
        + std::cmp::PartialOrd
        + std::ops::AddAssign
        + From<i16>
        + Clone,
{
    fn eps() -> Self {
        let d = Self::from(10_000);
        Self::from(1) / (d.clone() * d)
    }
}
