/** "Index" on a grid graph */
#include <cstdint>


class GridIndex
{
public:
    GridIndex(int n1, int n2)  : n1(n1), n2(n2) { }

private:
    int n1, n2;
};
