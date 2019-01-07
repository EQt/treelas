#pragma once
#include <vector>
#include <iostream>
#include <sstream>
#include <cstdio>


template <typename B, typename E>
inline std::ostream&
print_it(std::ostream &o, const B &begin, const E &end)
{
    bool first = true;
    for (auto it = begin; it != end; it++) {
        auto x = *it;
        if (first) {
            o << "[";
            first = false;
        } else
            o << ", ";
        o << x;
    }
    return o << "]";
}


template<typename Container> void
print(const Container &x, const int prec = 4, FILE *out = stdout,
      const char * fmt = "%.*f")
{
    fprintf(out, "[");
    bool first = true;
    for (double xi : x) {
        if (first) { first = false; }
        else { fprintf(out, ", "); }
        fprintf(out, fmt, prec, xi);
    }
    fprintf(out, "]\n");
}


namespace std {

template <typename E>
inline ostream&
operator<<(ostream &o, const vector<E> &v)
{
    return ::print_it(o, v.begin(), v.end());
}


template<typename E>
std::string
to_string(const vector<E> &v) {
    ostringstream s;
    s << v;
    return s.str();
}

}   // namespace std::
