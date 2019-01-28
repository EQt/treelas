#pragma once
#include <cstdint>
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
   [7]: https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_tzcnt_u64
 */
inline uint64_t
leading_zeros(uint64_t x)
{
#if defined __APPLE__ && defined __clang__
    return 63 - _lzcnt_u32(x);
#else
    return _lzcnt_u64(x);
#endif
}


inline uint64_t
leading_zeros(int64_t x)
{
    return leading_zeros((uint64_t) x);
}


inline uint32_t
leading_zeros(uint32_t x)
{
#if defined __APPLE__ && defined __clang__
    return 31 - _lzcnt_u32(x);
#else
    return _lzcnt_u32(x);
#endif
}


inline uint32_t
leading_zeros(int32_t x)
{
    return leading_zeros((uint32_t) x);
}


inline uint64_t
shift1u64(int n)
{
    return uint64_t(1) << n;
}


inline int
shift1int(int n)
{
    return int(1) << n;
}


inline uint64_t
hyperfloor(uint64_t x)
{
    constexpr uint64_t b100 = uint64_t(1) << 63;
    static_assert(b100 == uint64_t(0x8000000000000000), "internal error");
    const uint64_t lz = leading_zeros(x);
    return lz < 64 ? b100 >> lz : 0;
}
