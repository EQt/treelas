/// Find the root node of a tree given its `parent` vector.
///
/// This is the first `i` such that `parent[i] == i`.
/// ```
/// use graphidx::tree::find_root;
/// assert_eq!(find_root(&[1, 1, 0]), Some(1));
/// assert_eq!(find_root(&[1, 0, 0]), None);
/// ```
pub fn find_root(parent: &[usize]) -> Option<usize> {
    let n = parent.len();
    for i in 0..n {
        if parent[i] == i {
            return Some(i);
        }
    }
    None
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_root_gen5() {
        let pi: Vec<usize> = vec![0, 0, 1, 2, 3, 0, 7, 8, 3, 8];
        let maybe_root = find_root(&pi);
        assert!(maybe_root.is_some());
        assert_eq!(maybe_root.unwrap(), 0);
    }

    #[test]
    fn test_root_8_not_exist() {
        let pi: Vec<usize> = vec![1, 0, 1, 2, 3, 0, 7, 8, 3, 8];
        let maybe_root = find_root(&pi);
        assert!(maybe_root.is_none());
    }

    #[test]
    fn test_root_perm() {
        let pi: Vec<usize> = vec!
          // 0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17
            [6,  6,  6,  7,  7,  7,  8,  8, 17, 15, 15, 15, 16, 16, 16, 17, 17, 17];
        assert_eq!(find_root(&pi).unwrap(), 17);
    }
}
