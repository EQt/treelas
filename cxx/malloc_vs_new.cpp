#include <vector>
#include <string>
#include <iostream>

#include "utils/malloc.hpp"
#include "utils/timer.hpp"
#include "utils/thousand.hpp"


void
test_malloc(const size_t n)
{
    Timer timer ("malloc");
    Malloc<char> a (n);
    char *b = a.data();
    timer.stop();
    for (size_t i = 0; i < n; i += 8)
        b[i] = char(i);
    Timer::startit("free");
}


void
test_vector(const size_t n)
{
    Timer timer ("vector");
    std::vector<char> a (n);
    char *b = a.data();
    timer.stop();
    for (size_t i = 0; i < n; i += 8)
        b[i] = char(i);
    Timer::startit("free");
}


int
main()
{
    set_thousand_sep(std::cout);
    const size_t n = 1024 * 1024 * 256;
    for (size_t i = 1; i < 10; i++) {
        std::cout << "n = " << i * n << std::endl;
        test_vector(i*n);
        Timer::stopit();
        test_malloc(i*n);
        Timer::stopit();
    }
    return 0;
}
