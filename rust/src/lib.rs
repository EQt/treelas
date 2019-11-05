use std::time::Instant;
pub mod generics;
pub mod instance;
pub mod line;
pub mod pwl;
pub mod tree;

#[no_mangle]
pub extern "C" fn line_las(
    n: usize,
    x: *mut f64,
    y: *const f64,
    lam: f64,
) -> f64 {
    let mut x: &mut [f64] = unsafe { std::slice::from_raw_parts_mut(x, n) };
    let y = unsafe { std::slice::from_raw_parts(y, n) };
    let lam = graphidx::weights::ConstantWeights::new(lam);
    let mu = graphidx::weights::ConstantWeights::new(1.0);
    let mut solver = line::LineDP::new(y.len());
    let start_time = Instant::now();
    solver.solve::<_, _, generics::False>(&mut x, &y, &lam, &mu);
    start_time.elapsed().as_secs_f64()
}



