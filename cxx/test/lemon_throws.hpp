#pragma once
#ifdef HAVE_LEMON
#include <stdexcept>

class lemon_assert : public std::invalid_argument {
public:
    lemon_assert(
            const char* /* file */,
            int /* line */,
            const char* /* function */,
            const char* message,
            const char* /* assertion */
        ) : std::invalid_argument(message) { }
};


#define LEMON_ASSERT_CUSTOM
#define LEMON_CUSTOM_ASSERT_HANDLER throw ::lemon_assert

#endif
