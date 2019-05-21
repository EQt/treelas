#include <gtest/gtest.h>
#include <vector>
#include "../event.hpp"
#include "../range.hpp"



TEST(reference, problem_clip_front)
{
    std::vector<Event3> elements =
        {{9, -99, 9},
         {4,  -8, 0},
         {3, -16, 0},
         {2, +24, 0},
         {1, +42, 0}};

    const auto ecopy (elements);

    Range pq {1, 3};
    ASSERT_EQ(elements, ecopy);
    {   // simulate old clip_front
        auto &e = elements[pq.start];
        ASSERT_EQ(e, ecopy[1]);
        while (pq.start <= pq.stop) {
            e = elements[++pq.start];
        }
    }
    ASSERT_EQ(pq.start, 4);
    ASSERT_NE(elements, ecopy);
    ASSERT_EQ(elements, std::vector<Event3> (
                  {ecopy[0], ecopy[4], ecopy[2], ecopy[3], ecopy[4]}));
}


TEST(reference, solved_clip_front)
{
    std::vector<Event3> elements =
        {{9, -99, 9},
         {4,  -8, 0},
         {3, -16, 0},
         {2, +24, 0},
         {1, +42, 0}};

    const auto ecopy (elements);

    Range pq {1, 3};
    ASSERT_EQ(elements, ecopy);
    {   // simulate old clip_front
        const auto *e = &elements[pq.start];
        ASSERT_EQ(*e, ecopy[1]);
        while (pq.start <= pq.stop) {
            e = &elements[++pq.start];
        }
    }
    ASSERT_EQ(pq.start, 4);
    ASSERT_EQ(elements, ecopy);
}


