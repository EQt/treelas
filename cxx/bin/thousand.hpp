#pragma once
#include <locale>
#include <iostream>


struct space_out : std::numpunct<char>
{
    const char sep;
    space_out(const char sep = '_') : sep(sep) { }
    char do_thousands_sep()   const { return sep; }  // separate with spaces
    std::string do_grouping() const { return "\3"; } // groups of 3 digit
};


inline
void
set_thousand_sep(std::ostream &s, const char sep = '_')
{
    s.imbue(std::locale(std::cout.getloc(), new space_out(sep)));
}
