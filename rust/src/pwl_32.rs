use crate::generics::Bool;
use std::ops::Range;

pub(crate) const EPS: f32 = 1e-9;

#[derive(Debug, PartialEq, Default)]
pub(crate) struct Event {
    pub(crate) x: f32,
    pub(crate) slope: f32,
}

impl Event {
    fn offset(&self) -> f32 {
        -self.x * self.slope
    }
}

pub(crate) fn clip<Forward: Bool, Check: Bool>(
    elements: &mut [Event],
    pq: &mut Range<usize>,
    mut slope: f32,
    mut offset: f32,
) -> f32 {
    let mut start: usize = pq.start;
    let mut stop: usize = pq.end - 1;
    let mut e = &elements[if Forward::is_true() { start } else { stop }];
    while start <= stop && slope * e.x + offset < 0.0 {
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
    let x = if Check::is_true() && slope.abs() <= EPS {
        if Forward::is_true() {
            f32::NEG_INFINITY
        } else {
            f32::INFINITY
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
        let e = Event {
            x: -0.15,
            slope: 2.0,
        };
        assert_eq!(e.offset(), 0.3);
    }
}
