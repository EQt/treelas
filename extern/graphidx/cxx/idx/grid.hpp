/** "Index" on a grid graph */
#include <cstdint>

enum GridPointKinds {
    LL, LM, LR,
    ML, MM, MR,
    RL, RM, RR
};

struct GridNeighbors;


struct GridNeighborsIter
{
    const GridNeighbors *gn;
    int k;

    GridNeighborsIter(const GridNeighbors *gn, int k) : gn(gn), k(k) { }

    GridNeighborsIter& operator++() {
        this->k++;
        return *this;
    }

    bool operator==(const GridNeighborsIter &other) const {
        return this->k == other.k;
    }

    int operator*() const;
};


struct GridNeighbors
{
    int n1, n2, i1, i2;


    GridNeighbors(int n1, int n2, int i1, int i2) :
        n1(n1), n2(n2), i1(i1), i2(i2) { }

    GridNeighborsIter begin() const {
        return {this, 0};
    }

    GridNeighborsIter end() const {
        return {this, 4};
    }
};


int
GridNeighborsIter::operator*() const
{
    int
        i1 = gn->i1,
        i2 = gn->i2;
    switch (this->k) {
    case 0: i2++; break;
    case 1: i1--; break;
    case 2: i2--; break;
    case 3: i1++; break;
    }
    return -1;
}


class GridIndex
{
public:
    GridIndex(int n1, int n2)  : n1(n1), n2(n2) { }

    GridNeighbors operator[](int i) {
        const int
            i1 = i % n1,
            i2 = i / n2;
        return {n1, n2, i1, i2};
    }

    

private:
    int n1, n2;
};
