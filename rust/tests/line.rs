use treelas::instance::{Instance, data_dir};
use treelas::line::LineDP;


#[test]
fn lines_toml() -> Result<(), Box<dyn std::error::Error>> {
    let fname = data_dir()?.join("test").join("lines.toml");
    let instances = Instance::from_path(fname, "test")?;
    assert!(instances.len() >= 5);
    for inst in instances {
        let x_correct: &[f64] = &inst.x.as_ref().unwrap();
        let n = x_correct.len();
        let mut solver = LineDP::new(n);
        let mut x = Vec::new();
        x.resize(n, std::f64::NAN);
        solver.solve_instance(&mut x, &inst);
    }
    Ok(())
}
