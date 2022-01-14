use serde::Deserialize;
use toml;

#[derive(Debug, Deserialize, PartialEq)]
struct TestLam {
    lam: f64,
}

#[test]
fn parse_test() -> Result<(), toml::de::Error> {
    let tl: TestLam = toml::from_str(r#"lam = 1 "#)?;
    assert!(tl.lam == 1.0);
    Ok(())
}
