use super::find_root;
use std::ops::Index;

/// Find all children of a node in constant time.
///
/// # Example
///
/// Have a rooted tree (root is 0) whereby nodes 1 and 2 have parent
/// 0, node 3 has parent 2:
///
/// ```rust
/// use graphidx::tree::ChildrenIndex;
/// let cidx = ChildrenIndex::from_parent(&[0, 0, 0, 2]).unwrap();
/// assert_eq!(cidx.len(), 4, "we have four nodes in the tree");
/// assert_eq!(cidx.root_node(), 0);
/// assert_eq!(cidx[0], [1, 2]);
/// assert_eq!(cidx[1], []);
/// assert_eq!(cidx[2], [3]);
/// ```
#[derive(PartialEq, Debug)]
pub struct ChildrenIndex {
    idx: Vec<usize>,
    child: Vec<usize>,
}

impl ChildrenIndex {
    /// Build a new `Children Index`.
    pub fn from_tree(parent: &[usize], root: usize) -> Self {
        let n = parent.len();
        assert!(n > 0);
        assert!(parent[root] == root);
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
                deg_ii = idx[i + 1];
            }
            idx[n] = acc;
            assert_eq!(
                acc + deg_i,
                n,
                "n={}, deg_i={}, deg_ii={}, idx={:?}",
                n,
                deg_i,
                deg_ii,
                idx
            );
        }

        let mut child: Vec<usize> = Vec::new();
        child.resize(n, std::usize::MAX);
        child[0] = root;

        for v in 0..n {
            let p = parent[v];
            if v == root {
                continue;
            }
            child[idx[p + 1]] = v;
            idx[p + 1] += 1;
        }
        assert!(idx[n] == n);
        Self {
            idx: idx,
            child: child,
        }
    }

    /// Construct a new `ChildrenIndex` by first finding the `root` node.
    pub fn from_parent(parent: &[usize]) -> Option<Self> {
        return Some(Self::from_tree(parent, find_root(parent)?));
    }

    /// The root node of the underlying tree.
    pub fn root_node(self: &Self) -> usize {
        return self.child[0];
    }

    /// Number of nodes in the underlying tree.
    pub fn len(self: &Self) -> usize {
        return self.child.len();
    }
}

impl Index<usize> for ChildrenIndex {
    type Output = [usize];

    fn index(&self, i: usize) -> &Self::Output {
        assert!(i < self.len());
        &self.child[self.idx[i]..self.idx[i + 1]]
    }
}

#[cfg(test)]
mod tests {
    use super::*;

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
        assert_eq!(&cidx[0], vec![1].as_slice());
        assert_eq!(&cidx[1], vec![2].as_slice());
        assert_eq!(&cidx[2], vec![].as_slice());

        let cidx2 = ChildrenIndex::from_parent(&pi).unwrap();
        assert_eq!(cidx, cidx2);
    }

    #[test]
    fn children_gen5() {
        let n = 10;
        let r = 0;
        let cidx = ChildrenIndex::from_tree(&[0, 0, 1, 2, 3, 0, 7, 8, 3, 8], r);
        assert_eq!(cidx.len(), n);
        assert_eq!(cidx.root_node(), r);
        assert_eq!(cidx[0], [1, 5]);
        assert_eq!(cidx[1], [2]);
        assert_eq!(cidx[2], [3]);
        assert_eq!(cidx[3], [4, 8]);
        assert_eq!(cidx[7], [6]);
        assert_eq!(cidx[8], [7, 9]);
        for i in &[4, 5, 6, 9] {
            assert_eq!(cidx[*i], []);
        }
    }

    #[test]
    fn binary_3() {
        let pi = [0, 0, 0, 1, 1, 2, 2];
        assert_eq!(find_root(&pi), Some(0));
        let cidx = ChildrenIndex::from_parent(&pi).unwrap();
        for i in 0..3 {
            assert_eq!(cidx[i].len(), 2);
        }
        for i in 3..cidx.len() {
            assert_eq!(cidx[i].len(), 0);
        }
    }

    #[test]
    fn children_gen5_perm() {
        let parent = [6, 6, 6, 7, 7, 7, 8, 8, 8];
        let r = 8;
        assert_eq!(find_root(&parent), Some(r));
        let cidx = ChildrenIndex::from_parent(&parent).unwrap();
        assert_eq!(cidx.root_node(), r);
        assert_eq!(cidx.idx, [1, 1, 1, 1, 1, 1, 1, 4, 7, 9]);
        assert_eq!(cidx.child, [8, 0, 1, 2, 3, 4, 5, 6, 7]);
    }

    #[test]
    fn children_panic() {
        let pi = [0, 2];
        let res =
            std::panic::catch_unwind(move || ChildrenIndex::from_parent(&pi));
        assert!(res.is_err());
    }
}
