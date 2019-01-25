#pragma once
#include <stdint.h>
#include <immintrin.h>

/**
   Count number of leading zeros (in binary representation).

   It uses specific machine instructions as described in [Wikipedia][1].
   For C see the list of [GCC builtins][2].

   [1]: https://en.wikipedia.org/wiki/Find_first_set
   [2]: https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html
   [3]: https://github.com/JuliaLang/julia/blob/master/src/runtime_intrinsics.c
   [4]: https://docs.microsoft.com/en-us/cpp/intrinsics/lzcnt16-lzcnt-lzcnt64?view=vs-2017
   [5]: https://clang.llvm.org/doxygen/lzcntintrin_8h_source.html
   [6]: https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_lzcnt_u64
 */
inline uint64_t
leading_zeros(uint64_t x)
{
    return _lzcnt_u64(x);
}


inline uint32_t
leading_zeros(uint32_t x)
{
    return _lzcnt_u32(x);
}


inline uint64_t
hyperfloor(uint64_t x)
{
    return uint64_t(1) << (64 - leading_zeros(x));
}
