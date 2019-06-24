//! Functions regarding (rooted) tree graphs.
mod children;
mod root;

pub use children::*;
pub use root::find_root;

/// A tree represented by a `parent` array that tells the parent node.
/// For the `root` node it must hold `parent[root] == root`.
#[allow(dead_code)]
pub struct Tree {
    root: usize,
    parent: Vec<usize>,
}

impl Tree {
    /// Compute the `root` node by `find_root`.
    pub fn from_parent(parent: Vec<usize>) -> Option<Self> {
        Some(Self {
            root: find_root(&parent)?,
            parent: parent
        })
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_from_root() {
        let tree = Tree::from_parent(vec![1, 1, 0]).unwrap();
        assert_eq!(tree.root, 1);
    }
}
