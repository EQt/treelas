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
pub struct PWL {
    offset: f64,
    r: Range<usize>,
}

impl PWL {
    const EPS: f64 = 1e-9;

    pub fn new(n: usize) -> Self {
        Self {
            offset: 0.0,
            r: Range {
                start: n,
                end: n - 1,
            },
        }
    }

    pub fn clip_front(
        &mut self,
        event: &mut [Event],
        offset: f64,
        slope: f64,
    ) -> Option<f64> {
        let (o, s) = {
            let mut r = &mut self.r;
            let mut slope = slope;
            let mut offset = offset + self.offset;
            while r.start >= r.end {
                let e = &event[r.start];
                r.start += 1;
                if slope * e.x + offset >= 0.0 {
                    break;
                }
                slope += e.slope;
                offset += e.offset();
            }
            (slope, offset)
        };
        self.offset += o;
        if s.abs() > Self::EPS {
            let x = -o / s;
            self.r.start -= 1;
            event[self.r.start] = Event {
                x: x,
                slope: s,
            };
            Some(x)
        } else {
            None
        }
    }

    #[allow(unused_variables)]
    pub fn clip_back(
        &mut self,
        event: &mut [Event],
        offset: f64,
        slope: f64,
    ) -> Option<f64> {
        unimplemented!();
    }
}
