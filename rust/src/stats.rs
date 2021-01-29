use crate::float::Float;

/// https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Welford%27s_online_algorithm
pub fn welford_mean<'a, I, F>(y: I) -> F
where
    I: IntoIterator<Item = &'a F>,
    F: Float + 'a,
{
    let mut mean: F = 0.into();
    let mut i: F = 0.into();
    for x in y.into_iter() {
        i += 1.into();
        mean += (x.clone() - mean.clone()) / i.clone();
    }
    mean
}

pub fn lambda_max<'a, I, F>(y: I) -> F
where
    I: IntoIterator<Item = &'a F> + Clone,
    F: Float + 'a,
{
    let y_mean = welford_mean(y.clone());
    let zero: F = 0.into();
    y.into_iter()
        .map(|yi| (yi.clone() - y_mean.clone()))
        .scan(zero, |acc, x| {
            *acc += x;
            Some(acc.clone())
        })
        .map(|x| x.abs())
        .max_by(|a, b| a.partial_cmp(b).unwrap())
        .unwrap()
}

pub fn lambda_max_factor<'a, I, J, F>(y: I, lam: J) -> F
where
    I: IntoIterator<Item = &'a F> + Clone,
    J: IntoIterator<Item = &'a F>,
    F: Float + 'a,
{
    let y_mean = welford_mean(y.clone());
    let zero: F = 0.into();
    y.into_iter()
        .map(|yi| (yi.clone() - y_mean.clone()))
        .scan(zero, |acc, x| {
            *acc += x;
            Some(acc.clone())
        })
        .zip(lam.into_iter())
        .map(|(x, l)| x.abs() / l.clone())
        .max_by(|a, b| a.partial_cmp(b).unwrap())
        .unwrap()
}

#[cfg(test)]
mod tests {
    use super::*;
    use approx::assert_relative_eq;

    #[test]
    fn test_welford_mean() {
        let data1: Vec<f64> = vec![
            -0.2711336,
            1.20002575,
            0.69102151,
            -0.56390913,
            -1.62661382,
            -0.0613969,
            0.39876752,
            -0.99619281,
            1.12860854,
            -0.61163405,
        ];
        assert_relative_eq!(welford_mean(&data1), -0.071245699);
    }

    #[test]
    fn test_lam_max_const() {
        let l = lambda_max(&[0.5, 0.5, 0.5]);
        assert!(l.abs() < 1e-17, "l={}", l);
    }

    #[test]
    fn test_lam_max_0() {
        assert_relative_eq!(lambda_max(&[0.0, 3.0, 0.0]), 1.0);
    }

    #[test]
    fn test_lam_max_1() {
        assert_relative_eq!(lambda_max(&[0.15, 0.15, 0.7, 1.0]), 0.7);
    }

    #[test]
    fn test_lam_max_factor_1() {
        use crate::graphidx::weights::{Array, Ones};
        use crate::line::LineDP;

        let y = [0.15, 0.15, 0.7, 1.0];
        let lam = [1.0, 0.3, 1.0];
        let lam0 = 7.0 / 3.0;
        assert_relative_eq!(lambda_max_factor(&y, &lam), lam0);
        let mu = Ones::new();
        let lama = Array::new(lam.iter().map(|&l| l * lam0).collect());
        assert_eq!(LineDP::new(y.len()).dp_optimize(&y, &lama, &mu).dof(), 1);
        let lama = Array::new(lam.iter().map(|&l| l * (lam0 - 1e-9)).collect());
        assert_eq!(LineDP::new(y.len()).dp_optimize(&y, &lama, &mu).dof(), 2);
    }
}
