use crate::float::Float;
use crate::generics::Bool;
use std::ops::Range;

#[derive(Debug, PartialEq, Default)]
pub(crate) struct Event<F: Float> {
    pub(crate) x: F,
    pub(crate) slope: F,
}

impl<F: Float> Event<F> {
    fn offset(&self) -> F {
        -self.x.clone() * self.slope.clone()
    }
}

pub(crate) fn clip<F: Float, Forward: Bool, Check: Bool>(
    elements: &mut [Event<F>],
    pq: &mut Range<usize>,
    mut slope: F,
    mut offset: F,
) -> F {
    let mut start: usize = pq.start;
    let mut stop: usize = pq.end - 1;
    let idx = if Forward::is_true() { start } else { stop };
    let mut e = unsafe { elements.get_unchecked(idx) };
    while start <= stop && slope.clone() * e.x.clone() + offset.clone() < 0.into() {
        offset += e.offset();
        slope += e.slope.clone();
        let next = if Forward::is_true() {
            start += 1;
            start
        } else {
            stop -= 1;
            stop
        };
        e = unsafe { elements.get_unchecked(next) };
    }
    let x = if Check::is_true() && slope.clone().abs() <= F::eps() {
        if Forward::is_true() {
            -F::infinity()
        } else {
            F::infinity()
        }
    } else {
        let x = -offset / slope.clone();
        let prev = if Forward::is_true() {
            start -= 1;
            start
        } else {
            stop += 1;
            stop
        };
        *unsafe { elements.get_unchecked_mut(prev) } = Event {
            x: x.clone(),
            slope,
        };
        x
    };
    *pq = start..(stop + 1);
    x
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_event_offset() {
        let e = Event::<f32> {
            x: -0.15,
            slope: 2.0,
        };
        assert_eq!(e.offset(), 0.3);
    }
}
