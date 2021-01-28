use crate::float::Float;

/// https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Welford%27s_online_algorithm
pub fn welford_mean<I>(y: I) -> I::Item
where
    I: IntoIterator + Clone,
    I::Item: Float,
{
    let mut mean: I::Item = 0.into();
    let mut i: I::Item = 0.into();
    for x in y.into_iter() {
        i += 1.into();
        mean += (x - mean.clone()) / i.clone();
    }
    mean
}

pub fn lambda_max<I>(y: I) -> I::Item
where
    I: IntoIterator + Clone,
    I::Item: Float,
{
    let y_mean = welford_mean(y.clone());
    let zero: I::Item = 0.into();
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
        assert_relative_eq!(welford_mean(data1.iter().cloned()), -0.071245699);
    }

    #[test]
    fn test_lam_max_const() {
        let l = lambda_max([0.5, 0.5, 0.5].iter().cloned());
        assert!(l.abs() < 1e-17);
    }

    #[test]
    fn test_lam_max_0() {
        assert_relative_eq!(lambda_max([0.0, 3.0, 0.0].iter().cloned()), 1.0);
    }

    #[test]
    fn test_lam_max_1() {
        assert_relative_eq!(lambda_max([0.15, 0.15, 0.7, 1.0].iter().cloned()), 0.7);
    }

}
