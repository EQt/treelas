use crate::generics::Bool;
use std::ops::Range;

pub(crate) const EPS: f64 = 1e-9;

const DEBUG: bool = false;

#[derive(Debug, PartialEq, Default)]
pub(crate) struct Event {
    pub(crate) x: f64,
    pub(crate) slope: f64,
}

impl Event {
    fn offset(&self) -> f64 {
        -self.x * self.slope
    }
}

pub(crate) fn clip<Forward: Bool, Check: Bool>(
    elements: &mut [Event],
    pq: &mut Range<usize>,
    mut slope: f64,
    mut offset: f64,
) -> f64 {
    let dir = || if Forward::is_true() { "f" } else { "b" }; // for debugging
    let mut start: usize = pq.start;
    let mut stop: usize = pq.end - 1;
    let mut e = &elements[if Forward::is_true() { start } else { stop }];
    if DEBUG {
        println!(
            "clip_{}: ({:+}, {:+.3}): {:.3?}\n\t {:.3?}",
            dir(),
            slope,
            offset,
            e,
            &elements[start..stop + 1],
        );
    }
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
        if DEBUG {
            println!(
                " lip_{}: ({:+}, {:+.3}): {:.3?} offset: {:.3}\n\t {:.3?}",
                dir(),
                slope,
                offset,
                e,
                e.offset(),
                &elements[start..stop + 1],
            );
        }
    }
    let x = if Check::is_true() && slope.abs() <= EPS {
        if Forward::is_true() {
            f64::NEG_INFINITY
        } else {
            f64::INFINITY
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
    if DEBUG {
        println!(
            "  ip_{}: ({:+}, {:+.3}):\n\t {:.3?}",
            dir(),
            slope,
            offset,
            &elements[start..stop + 1],
        );
    }
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
