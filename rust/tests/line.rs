use treelas::instance::{data_dir, Instance};
use treelas::line::{l1_norm, LineDP};

#[test]
fn lines_toml() -> Result<(), Box<dyn std::error::Error>> {
    let fname = data_dir()?.join("test").join("lines.toml");
    let instances = Instance::from_path(fname, "test")?;
    assert!(instances.len() >= 5);
    for inst in instances {
        let x_correct = &inst.x.as_ref().unwrap().clone();
        let n = x_correct.len();
        println!("\nsolving: {:?}\n", &inst);
        let mut solver = LineDP::new(n);
        let mut x = Vec::new();
        x.resize(n, std::f64::NAN);
        solver.solve_instance(&mut x, inst);
        let diff = l1_norm(&x, &x_correct);
        assert!(
            diff <= 1e-10,
            "err: diff = {}, x = {:?}, expected = {:?}",
            diff,
            x,
            x_correct
        );
    }
    Ok(())
}
