use serde::Deserialize;
use std::path::Path;
use std::path::PathBuf;

type Map<K, V> = std::collections::BTreeMap<K, V>;

#[derive(Debug, Deserialize, PartialEq)]
#[serde(untagged)]
pub enum Weights<T> {
    Const(T),
    Array(Vec<T>),
}

/// All data needed in a fused lasso line problem
///
/// Usually the instance is stored in a TOML-file and can be loaded
/// from a file by
/// [`Instance::from_path`], or from string by
/// [`Instance::from_str`].
///
/// [`Instance::from_path`]: #method.from_path
/// [`Instance::from_str`]: #method.from_str
#[derive(Debug, Deserialize, PartialEq)]
pub struct Instance {
    name: Option<String>,
    pub y: Vec<f64>,
    pub lam: Weights<f64>,
    pub x: Option<Vec<f64>>,
    nr: Option<u32>,
    pub mu: Option<Weights<f64>>,
    pub parent: Option<Vec<i64>>,
    pub root: Option<i64>,
}

#[allow(clippy::len_without_is_empty)]
impl Instance {
    pub fn from_path<P: AsRef<Path>>(
        fname: P,
        kind: &str,
    ) -> Result<Vec<Self>, Box<dyn std::error::Error>> {
        let instances: Map<String, Vec<Self>> = {
            let buf = std::fs::read(fname)?;
            toml::from_slice(&buf)
        }?;
        Ok(Self::from_map(instances, kind))
    }

    pub fn from_map(mut instances: Map<String, Vec<Self>>, kind: &str) -> Vec<Self> {
        if let Some(mut trees) = instances.remove(kind) {
            for (i, mut t) in trees.iter_mut().enumerate() {
                t.nr = Some((i + 1) as u32);
            }
            trees
        } else {
            vec![]
        }
    }

    pub fn from_str(toml: &str, kind: &str) -> Result<Vec<Self>, toml::de::Error> {
        let inst: Map<String, Vec<Self>> = toml::from_str(toml)?;
        Ok(Self::from_map(inst, kind))
    }

    #[allow(dead_code)]
    pub fn len(&self) -> usize {
        self.y.len()
    }
}

/// Directory of the test data.
pub fn data_dir() -> std::io::Result<PathBuf> {
    Ok(std::env::current_dir()?.join("data"))
}

#[cfg(test)]
mod tests {
    use super::*;

    mod test_wrapped_f64 {
        use super::*;

        #[derive(Debug, Deserialize, PartialEq)]
        #[serde(untagged)]
        enum Wrap {
            Val(f64),
        }

        #[derive(Debug, Deserialize, PartialEq)]
        struct TestLam {
            lam: Wrap,
        }

        #[test]
        fn parse_test() -> Result<(), toml::de::Error> {
            let tl: TestLam = toml::from_str(r#"lam = 1 "#)?;
            assert!(tl.lam == Wrap::Val(1.0));
            Ok(())
        }
    }

    #[test]
    fn parse_from_str() -> Result<(), toml::de::Error> {
        let toml_str = r#"
        [[tree]]
        name = "unit test"
        y = [0, 1]
        lam = 1
        "#;

        let tests = Instance::from_str(toml_str, "tree")?;
        assert!(tests.len() == 1);
        let test = &tests[0];
        assert_eq!(
            test,
            &Instance {
                name: Some(String::from("unit test")),
                y: vec![0.0, 1.0],
                lam: Weights::Const(1.0),
                x: None,
                nr: Some(1),
                mu: None,
                parent: None,
                root: None
            }
        );
        Ok(())
    }

    #[test]
    fn test_data_dir() {
        let ddir = data_dir().unwrap();
        assert!(ddir.exists(), "ddir = {:?}", ddir);
        assert!(ddir.is_dir());
    }

    #[test]
    fn test_data_test_dir() {
        let ddir = data_dir().unwrap();
        assert!(ddir.exists(), "ddir = {:?}", ddir);
        assert!(ddir.is_dir());
    }

    #[test]
    fn parse_tree0() -> Result<(), Box<dyn std::error::Error>> {
        let fname = data_dir()?.join("tree0.toml");
        assert!(fname.exists(), "fname = {:?}", fname);
        let tests: Vec<Instance> = Instance::from_path(fname, "tree")?;
        assert!(tests.len() >= 1, "len = {len}", len = tests.len());
        Ok(())
    }

    #[test]
    fn parse_lines() -> Result<(), Box<dyn std::error::Error>> {
        let fname = data_dir()?.join("lines.toml");
        assert!(fname.exists(), "fname = {:?}", fname);
        let tests: Vec<Instance> = Instance::from_path(fname, "test")?;
        assert!(tests.len() >= 5, "len = {len}", len = tests.len());
        Ok(())
    }
}

pub(crate) trait SolveInstance {
    fn solve_instance(&mut self, x: &mut [f64], inst: &Instance);
}

impl SolveInstance for treelars::line::LineDP<f64> {
    fn solve_instance(&mut self, x: &mut [f64], inst: &Instance) {
        use graphidx::weights::{ArrayRef, Const, Ones};

        let mu: Option<&Weights<f64>> = inst.mu.as_ref();
        let lam: &Weights<f64> = &inst.lam;
        match (lam, mu) {
            (Weights::Const(lam), Some(Weights::Const(mu))) => {
                let lam: Const<_> = lam.into();
                let mu: Const<_> = mu.into();
                self.solve(x, &inst.y, &lam, &mu);
            }
            (Weights::Array(lam), Some(Weights::Const(mu))) => {
                let lam: ArrayRef<_> = lam.into();
                let mu: Const<_> = mu.into();
                self.solve(x, &inst.y, &lam, &mu);
            }
            (Weights::Const(lam), Some(Weights::Array(mu))) => {
                let lam: Const<_> = lam.into();
                let mu: ArrayRef<_> = mu.into();
                self.solve(x, &inst.y, &lam, &mu);
            }
            (Weights::Array(lam), Some(Weights::Array(mu))) => {
                let mu: ArrayRef<_> = mu.into();
                let lam: ArrayRef<_> = lam.into();
                self.solve(x, &inst.y, &lam, &mu);
            }
            (Weights::Array(lam), None) => {
                let mu: Ones<f64> = Ones::default();
                let lam: ArrayRef<_> = lam.into();
                self.solve(x, &inst.y, &lam, &mu);
            }
            (Weights::Const(lam), None) => {
                let mu: Ones<f64> = Ones::default();
                let lam: Const<_> = lam.into();
                self.solve(x, &inst.y, &lam, &mu);
            }
        };
    }
}
