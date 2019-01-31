#pragma once
#include <type_traits>      // std::is_integral
#include <limits>

/**
   Negate numbers in a way that `neg(x) != x`.
   Additionally we require that `neg` is self-inverse, i.e. `neg(neg(x)) == x` for all `x`.
   This way, also `0` obtains a "sign".
   On the bit level, we just negate every single bit.
 */
template <typename Int>
Int
neg(const Int x)
{
    static_assert(std::is_integral<Int>::value, "Works only for integers");
    static_assert(std::is_signed<Int>::value, "Negation needs sign");
    // return -(x + Int(1));    // mathematical interpretation
    return Int(~x);             // fewer overflow errors
}


template <typename Int>
bool
is_pos(const Int x)
{
    static_assert(std::is_integral<Int>::value, "Works only for integers");
    static_assert(std::is_signed<Int>::value, "Negation needs sign");
    // return !((Int(1) << std::numeric_limits<Int>::digits) & x);
    return x >= 0;
}
