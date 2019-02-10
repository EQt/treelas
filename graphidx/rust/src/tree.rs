/// Find the root node of a tree given its `parent` vector
pub fn find_root(parent: &Vec<usize>) -> Option<usize> {
    let n = parent.len();
    for i in 0..(n - 1) {
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
    pub fn from_tree(parent: &Vec<usize>, root: usize) -> Self {
        let n = parent.len();
        let mut idx: Vec<usize> = Vec::new();
        idx.resize(n + 1, 0);
        for p in parent {
            idx[*p] += 1_usize;
        }

        let mut child: Vec<usize> = Vec::new();
        child.resize(n, std::usize::MAX);
        child[0] = root;
        Self {
            idx: idx,
            child: child,
        }
    }

    pub fn from_parent(parent: &Vec<usize>) -> Option<Self> {
        return Some(Self::from_tree(parent, 0));
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn base_3() {
        let pi: Vec<usize> = vec![0, 0, 1];
        let n = pi.len();
        let r: usize = 0;
        assert_eq!(find_root(&pi).unwrap(), r);
        assert_eq!(n, 3);
        let cidx = ChildrenIndex::from_tree(&pi, r);
        assert_eq!(cidx.idx.len(), n + 1);
        assert_eq!(cidx.child.len(), n);
        let cidx2 = ChildrenIndex::from_parent(&pi).unwrap();
        assert_eq!(cidx, cidx2);
    }
}
