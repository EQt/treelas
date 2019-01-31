#pragma once
#include <type_traits>      // std::is_integral

/**
   Negate numbers in a way that `neg(x) != x` (except for numerical overflows).
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
    return -(x + Int(1));
}
