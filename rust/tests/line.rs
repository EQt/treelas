use treelars::graphidx::lina::l1_diff;
use treelars::instance::{Instance, data_dir};
use treelars::instance::Weights as i;
use treelars::graphidx::weights as w;

#[test]
fn lines_toml() -> Result<(), Box<dyn std::error::Error>> {
    use treelars::line_64::LineDP;

    let fname = data_dir()?.join("test").join("lines.toml");
    let instances = Instance::from_path(fname, "test")?;
    assert!(instances.len() >= 5);
    for inst in instances {
        let x_correct = &inst.x.as_ref().unwrap();
        let n = x_correct.len();
        println!("\nsolving: {:?}\n", &inst);
        let mut solver = LineDP::new(n);
        let mut x = Vec::new();
        x.resize(n, std::f64::NAN);
        solver.solve_instance(&mut x, &inst);
        let diff = l1_diff(&x, &x_correct);
        assert!(
            diff <= 1e-10,
            "err: diff = {:.3}, x = {:.3?}, expected = {:?}",
            diff,
            x,
            &x_correct,
        );
    }
    Ok(())
}

#[test]
fn lines_toml_32() -> Result<(), Box<dyn std::error::Error>> {
    use treelars::line_32::LineDP;

    let fname = data_dir()?.join("test").join("lines.toml");
    let instances = Instance::from_path(fname.clone(), "test")?;
    assert!(instances.len() >= 5);
    for inst in instances {
        let x_correct: Vec<_> = if let Some(x_correct) = &inst.x {
            x_correct.iter().map(|&xi| xi as f32).collect()
        } else {
            panic!("Can't test {:?} because `x` is missing", &fname)
        };
        let n = x_correct.len();
        println!("\nsolving: {:?}\n", &inst);
        let mut solver = LineDP::new(n);
        let mut x = Vec::new();
        x.resize(n, std::f32::NAN);
        let y: Vec<_> = inst.y.iter().map(|&yi| yi as f32).collect();
        match (inst.lam, inst.mu) {
            (i::Const(lam), Some(i::Const(mu))) => {
                let lam = w::Const::new(lam as f32);
                let mu =  w::Const::new(mu as f32);
                solver.solve(&mut x, &y, &lam, &mu);
            },
            (i::Array(lam), Some(i::Const(mu))) => {
                let lam = w::Array::new(lam.iter().map(|&yi| yi as f32).collect());
                let mu =  w::Const::new(mu as f32);
                solver.solve(&mut x, &y, &lam, &mu);
            },
            (i::Const(lam), Some(i::Array(mu))) => {
                let lam = w::Const::new(lam as f32);
                let mu =  w::Array::new(mu.iter().map(|&yi| yi as f32).collect());
                solver.solve(&mut x, &y, &lam, &mu);
            },
            (i::Array(lam), Some(i::Array(mu))) => {
                let lam = w::Array::new(lam.iter().map(|&yi| yi as f32).collect());
                let mu =  w::Array::new(mu.iter().map(|&yi| yi as f32).collect());
                solver.solve(&mut x, &y, &lam, &mu);
            },
            (i::Const(lam), None) => {
                let lam = w::Const::new(lam as f32);
                let mu =  w::Ones::new();
                solver.solve(&mut x, &y, &lam, &mu);
            },
            (i::Array(lam), None) => {
                let lam = w::Array::new(lam.iter().map(|&yi| yi as f32).collect());
                let mu =  w::Ones::new();
                solver.solve(&mut x, &y, &lam, &mu);
            },
        };
        let diff = l1_diff(&x, &x_correct);
        assert!(
            diff <= 1e-6,
            "err: diff = {:.3}, x = {:.3?}, expected = {:?}",
            diff,
            x,
            &x_correct,
        );
    }
    Ok(())
}
