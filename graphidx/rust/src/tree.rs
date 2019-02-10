/// Find the root node of a tree given its `parent` vector
pub fn find_root(parent: &[usize]) -> Option<usize> {
    let n = parent.len();
    for i in 0..n {
        if parent[i] == i {
            return Some(i);
        }
    }
    None
}

#[derive(PartialEq,Debug)]
pub struct ChildrenIndex {
    idx: Vec<usize>,
    child: Vec<usize>,
}

impl ChildrenIndex {
    pub fn from_tree(parent: &[usize], root: usize) -> Self {
        let n = parent.len();
        assert!(n > 0);
        let mut idx: Vec<usize> = Vec::new();
        idx.resize(n + 1, 0);
        for p in parent {
            idx[*p] += 1_usize;
        }
        idx[root] -= 1;

        {
            let mut acc: usize = 1;
            let mut deg_i: usize = 0;
            let mut deg_ii = idx[0];
            for i in 0..n {
                idx[i] = acc;
                acc += deg_i;
                deg_i = deg_ii;
                deg_ii = idx[i+1];
            }
            assert_eq!(acc, n);
            idx[n] = acc;
        }

        let mut child: Vec<usize> = Vec::new();
        child.resize(n, std::usize::MAX);
        child[0] = root;

        for v in 0..n {
            let p = parent[v];
            if v == root {
                continue;
            }
            child[idx[p+1]] = v;
            idx[p+1] += 1;
        }
        assert!(idx[n] == n);
        Self {
            idx: idx,
            child: child,
        }
    }

    pub fn from_parent(parent: &[usize]) -> Option<Self> {
        return Some(Self::from_tree(parent, 0));
    }

    pub fn root_node(self: &Self) -> usize {
        return self.child[0];
    }

    pub fn len(self: &Self) -> usize {
        return self.child.len();
    }
}

impl<'a> ChildrenIndex {
    // type Output = &'a [usize];

    pub fn index(self: &'a Self, i: usize) -> &'a [usize] {
        assert!(i < self.len());
        &self.child[self.idx[i]..self.idx[i+1]]
    }
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

    #[test]
    fn children_idx_3() {
        let pi: Vec<usize> = vec![0, 0, 1];
        let r: usize = 0;
        let n = pi.len();
        assert_eq!(n, 3);
        assert_eq!(find_root(&pi).unwrap(), r);

        let cidx = ChildrenIndex::from_tree(&pi, r);
        assert_eq!(cidx.len(), 3);
        assert_eq!(cidx.idx.len(), n + 1);
        assert_eq!(cidx.child.len(), n);
        assert_eq!(cidx.root_node(), r);
        assert_eq!(cidx.idx, vec![1, 2, 3, 3]);
        assert_eq!(cidx.child, vec![0, 1, 2]);
        assert_eq!(cidx.index(0), vec![1].as_slice());
        assert_eq!(cidx.index(1), vec![2].as_slice());
        assert_eq!(cidx.index(2), vec![].as_slice());

        let cidx2 = ChildrenIndex::from_parent(&pi).unwrap();
        assert_eq!(cidx, cidx2);

    }
}
