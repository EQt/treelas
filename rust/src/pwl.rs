use crate::float::Float;
use crate::generics::Bool;
use std::ops::Range;

pub const EPS: f32 = 1e-9;

#[derive(Debug, PartialEq, Default)]
pub(crate) struct Event<F: Float> {
    pub(crate) x: F,
    pub(crate) slope: F,
}

impl<F: Float> Event<F> {
    fn offset(&self) -> F {
        -self.x * self.slope.clone()
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
    let mut e = &elements[if Forward::is_true() { start } else { stop }];
    while start <= stop && slope * e.x + offset < 0.into() {
        offset += e.offset();
        slope += e.slope;
        let next = if Forward::is_true() {
            start += 1;
            start
        } else {
            stop -= 1;
            stop
        };
        e = &elements[next];
    }
    let x = if Check::is_true() && slope.abs() <= EPS.into() {
        if Forward::is_true() {
            -F::infinity()
        } else {
            F::infinity()
        }
    } else {
        let x = -offset / slope;
        let prev = if Forward::is_true() {
            start -= 1;
            start
        } else {
            stop += 1;
            stop
        };
        elements[prev] = Event { x, slope };
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
