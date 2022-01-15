pub use graphidx;
use std::time::Instant;
pub mod float;
mod generics;
// pub mod instance;
pub mod line;
// pub mod line_32;
// pub mod line_64;
mod pwl;
pub mod stats;
pub mod tree;

/// # Safety
/// Will crash if `x` or `y` do not point to an array of at least `n` times `f64`
/// elements
#[no_mangle]
pub unsafe extern "C" fn line_las(n: usize, x: *mut f64, y: *const f64, lam: f64) -> f64 {
    let x: &mut [f64] = std::slice::from_raw_parts_mut(x, n);
    let y = std::slice::from_raw_parts(y, n);
    let lam = graphidx::weights::Const::new(lam);
    let mu = graphidx::weights::Ones::<f64>::new();
    let mut solver = line::LineDP::<f64>::new(y.len());
    let start_time = Instant::now();
    solver.solve(x, y, &lam, &mu);
    start_time.elapsed().as_secs_f64()
}

/// Like `line_las` but with 32bit float arithmetic, underneath.
/// # Safety
/// Assumes that `x` and `y` point to an array of at least `n` items of `f64`.
#[no_mangle]
pub unsafe extern "C" fn line_las_f32(n: usize, x: *mut f32, y: *const f32, lam: f32) -> f64 {
    let x: &mut [f32] = std::slice::from_raw_parts_mut(x, n);
    let y = std::slice::from_raw_parts(y, n);
    let lam = graphidx::weights::Const::new(lam);
    let mu = graphidx::weights::Ones::<f32>::new();
    let mut solver = line::LineDP::<f32>::new(y.len());
    let start_time = Instant::now();
    solver.solve(x, y, &lam, &mu);
    start_time.elapsed().as_secs_f64()
}
