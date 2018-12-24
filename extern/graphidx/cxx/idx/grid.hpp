/** "Index" on a grid graph */
#include <cstdint>
#include <stdexcept>


enum GridPointKind { // L = 0b10, R = 0b01, M = 0b00
    LL = 0b1010, LR = 0b1001, LM = 0b1000,
    RL = 0b0110, RR = 0b0101, RM = 0b0100,
    ML = 0b0010, MR = 0b0001, MM = 0b0000
};


inline int
num_neighbors(GridPointKind k)
{
    switch (k) {
    case LL: 
    case LR: return 2;
    case LM: return 3;
    case RL:
    case RR: return 2;
    case RM: return 3;
    case ML:
    case MR: return 3;
    case MM: return 4;
    default: return -2;
            
    }
    return -1;
}


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

    GridNeighbors operator[](int i) const {
        const int
            i1 = i % n1,
            i2 = i / n2;
        return {n1, n2, i1, i2};
    }

    GridPointKind kind(int i) const {
        const int
            i1 = i % n1,
            i2 = i / n1;
        int k = 0;
        if (i1 == 0)
            k = 0b10;
        else if (i1 == n1 - 1)
            k = 0b01;
        k <<= 2;
        if (i2 == 0)
            k |= 0b10;
        else if (i2 == n2 - 1)
            k |= 0b01;
        if (k > 0b1010)
            throw std::runtime_error("Should not happen");
        return GridPointKind(k);
    }

private:
    int n1, n2;
};
