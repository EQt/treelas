#pragma once
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)


/** information about compilation */
constexpr const char*
compiler_info()
{
    return "Compiled with "
#ifdef __clang__
    "Clang++ " __clang_version__
#elif defined __GNUC__
    "GCC (g++) " __VERSION__
#elif defined _MSC_VER
    "MSVC " STR(_MSC_VER)
#endif
     " on "  __DATE__  " " __TIME__
         ;
}
