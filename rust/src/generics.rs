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
