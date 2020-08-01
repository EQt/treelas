#include <iostream>

#include "../min_cut.hpp"


void
print_min_cut(const std::vector<bool> &above)
{
    std::cout << "min_cut = {";
    for (size_t i = 0; i < above.size(); i++)
        if (above[i]) {
            std::cout << " " << i << ",";
        }
    std::cout << "}" << std::endl;
}


int
main()
{
    // test_example1();
    // test_example2();
    std::cerr << "not implemented, yet" << std::endl;
    return 1;
}

// Local Variables:
// compile-command: "cd ../../build && COLOR=0 make  maxflow && ./maxflow"
// End:
