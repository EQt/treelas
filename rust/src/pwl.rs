use crate::generics::Bool;
use std::f64;
use std::ops::Range;

#[derive(Debug, PartialEq)]
pub struct Event {
    pub x: f64,
    pub slope: f64,
}

impl Default for Event {
    fn default() -> Self {
        Event {
            x: 0.0,
            slope: 0.0,
        }
    }
}

impl Event {
    fn offset(&self) -> f64 {
        -self.x / self.slope
    }
}

pub const EPS: f64 = 1e-9;

pub fn clip<Forward: Bool>(
    elements: &mut [Event],
    pq: &mut Range<usize>,
    mut slope: f64,
    mut offset: f64,
) -> f64 {
    let mut start = pq.start;
    let mut stop = pq.end - 1;
    let mut e = &elements[if Forward::is_true() {
        start
    } else {
        stop
    }];
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
    if slope.abs() <= EPS {
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
        elements[prev] = Event {
            x: x,
            slope: slope,
        };
        *pq = start..stop+1;
        x
    }
}
