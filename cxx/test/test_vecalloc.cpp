#include <gtest/gtest.h>
#include "../cxx/utils/vecalloc.hpp"


TEST(vecalloc, null)
{
    ASSERT_THROW(VecAlloc<int>(nullptr, 5), std::invalid_argument);
}


TEST(vecalloc, with_null)
{
    const int *p = nullptr;
    {
        VecAlloc<const int> _ (&p, 5);
        ASSERT_NE(p, nullptr);
    }
    ASSERT_EQ(p, nullptr);
}


TEST(std_remove_point, const_int)
{
    const int *p = nullptr;
    using T = typename std::remove_pointer<decltype(p)>::type;
    static_assert(std::is_same<std::remove_pointer<int*>::type,
                  int>::value, "strange...");
    static_assert(std::is_same<T, const int>::value, "strange...");
}


TEST(vecalloc, with_allocated)
{
    std::vector<int> hidden (5);
    const int *p = hidden.data();
    ASSERT_EQ(p, hidden.data());
    {
        using T = typename std::remove_pointer<decltype(p)>::type;
        VecAlloc<T> _ (&p, 5);
        ASSERT_EQ(p, hidden.data());
    }
    ASSERT_EQ(p, hidden.data());
}    
            
