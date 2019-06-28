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

pub fn clip_front(events: &[Event], slope: f64, offset: f64) -> (f64, f64) {
    let mut slope = slope;
    let mut offset = offset;
    for e in events {
        if slope * e.x + offset >= 0.0 {
            break;
        }
        slope += e.slope;
        offset += e.offset();
    }
    (slope, offset)
}


