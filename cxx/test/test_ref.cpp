#include <doctest/doctest.h>
#include <vector>
#include "../event.hpp"
#include "../range.hpp"



TEST_CASE("reference: problem_clip_front")
{
    std::vector<Event> elements =
        {{9, -99, 9},
         {4,  -8, 0},
         {3, -16, 0},
         {2, +24, 0},
         {1, +42, 0}};

    const auto ecopy (elements);

    Range pq {1, 3};
    REQUIRE(ecopy == elements);
    {   // simulate old clip_front
        auto &e = elements[pq.start];
        REQUIRE(ecopy[1] == e);
        while (pq.start <= pq.stop) {
            e = elements[++pq.start];
        }
    }
    REQUIRE(4 == pq.start);
    REQUIRE(ecopy != elements);
    REQUIRE(elements == std::vector<Event> (
                {ecopy[0], ecopy[4], ecopy[2], ecopy[3], ecopy[4]}));
}


TEST_CASE("reference: solved_clip_front")
{
    std::vector<Event> elements =
        {{9, -99, 9},
         {4,  -8, 0},
         {3, -16, 0},
         {2, +24, 0},
         {1, +42, 0}};

    const auto ecopy (elements);

    Range pq {1, 3};
    REQUIRE(ecopy == elements);
    {   // simulate old clip_front
        const auto *e = &elements[pq.start];
        REQUIRE(ecopy[1] == *e);
        while (pq.start <= pq.stop) {
            e = &elements[++pq.start];
        }
    }
    REQUIRE(4 == pq.start);
    REQUIRE(ecopy == elements);
}


