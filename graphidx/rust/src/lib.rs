#[derive(Debug)]
pub struct ChildrenIndex {
    idx: Vec<usize>,
    child: Vec<usize>,
}

impl ChildrenIndex {
    pub fn from_parent(parent: &Vec<usize>) -> Self {
        let n = parent.len();
        let mut idx: Vec<usize> = Vec::new();
        idx.resize(n + 1, 0);
        for p in parent {
            idx[*p] += 1_usize;
        }

        let mut child: Vec<usize> = Vec::new();
        child.resize(n, std::usize::MAX);
        Self {
            idx: idx,
            child: child,
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn base_3() {
        let pi: Vec<usize> = vec![0, 0, 1];
        let n = pi.len();
        assert_eq!(n, 3);
        let cidx = ChildrenIndex::from_parent(&pi);
        assert_eq!(cidx.idx.len(), n + 1);
        assert_eq!(cidx.child.len(), n);
    }
}
