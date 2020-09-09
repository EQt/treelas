use std::time::Instant;
pub mod generics;
pub mod instance;
pub mod line;
pub mod pwl;
pub mod tree;

/// # Safety
/// Will crash if `x` or `y` does not point to an array of at least `n` times `f64`
/// elements
#[no_mangle]
pub unsafe extern "C" fn line_las(
    n: usize,
    x: *mut f64,
    y: *const f64,
    lam: f64,
) -> f64 {
    let mut x: &mut [f64] = std::slice::from_raw_parts_mut(x, n);
    let y = std::slice::from_raw_parts(y, n);
    let lam = graphidx::weights::Const::new(lam);
    let mu = graphidx::weights::Ones::<f64>::new();
    let mut solver = line::LineDP::new(y.len());
    let start_time = Instant::now();
    solver.solve(&mut x, &y, &lam, &mu);
    start_time.elapsed().as_secs_f64()
}
