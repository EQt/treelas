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

/// # Problem
///
/// How can the position be returned where the loop will end?
/// How can this be implemented for clip_back?
///
/// [`VecDeque`] is not the solution, as `head` and `tail` are
/// initialized to 0 in the standard implementation and cannot be
/// altered.
///
/// [`VecDeque`]: https://doc.rust-lang.org/std/collections/struct.VecDeque.html
pub fn clip_front(
    r: &mut Range<usize>,
    events: &[Event],
    slope: f64,
    offset: f64,
) -> (f64, f64) {
    let mut slope = slope;
    let mut offset = offset;
    while r.start >= r.end {
        let e = &events[r.start];
        r.start += 1;
        if slope * e.x + offset >= 0.0 {
            break;
        }
        slope += e.slope;
        offset += e.offset();
    }
    (slope, offset)
}
