#include <doctest/doctest.h>
#include <cerrno>
#include <regex>

#include "../include/minih5.hpp"
#include "path.hpp"


// https://github.com/google/googletest/blob/master/googletest/docs/Primer.md
class HDF5Test
{
public:
    static constexpr bool delete_after = true;
    const char *fname = "test.h5";
    HDF5Test() {
        if (delete_after && path::exists(fname)) {
            INFO("fname=" << fname << " exists in CWD=" << path::cwd());
            const auto r = path::remove(fname);
            INFO("errno=" << errno);
            REQUIRE(r == 0);
        }
    }

    ~HDF5Test() {
        if (delete_after) {
            if (path::exists(fname)) {
                const auto r = path::remove(fname);
                INFO("errno=" << errno << ", EACCES=" << EACCES);
                REQUIRE(r == 0);
            }
            INFO("CWD=" << path::cwd() << ", fname=" << fname);
            REQUIRE(!path::exists(fname));
        } else {
            REQUIRE(path::exists(fname));
            fprintf(stderr, "Leaving %s/%s\n", path::cwd().c_str(), fname);
        }
    }
};


/*
TEST_CASE("HDF5lib: DISABLED_libversion")
{

    ASSERT_TRUE(std::regex_match(HDF5::libversion(), std::regex("1\\.8\\..*")))
        << "libversion=" << HDF5::libversion();
}
*/

TEST_CASE_FIXTURE(HDF5Test, "create")
{
    if (delete_after) {
        REQUIRE_THROWS_AS(HDF5(fname, "r"), std::runtime_error);
    }
    {
        HDF5 io (fname, "w");
    }   // automatically close it
    REQUIRE(path::exists(fname));
    REQUIRE_NOTHROW(HDF5 io (fname, "r"));
}


TEST_CASE_FIXTURE(HDF5Test, "has_nothing")
{
    HDF5 io (fname, "w");
    REQUIRE(!io.has("blub"));
}


TEST_CASE_FIXTURE(HDF5Test, "write_double")
{
    const std::vector<double> x ({1, 2, 3});
    {   // write
        HDF5 io (fname, "w");
        REQUIRE(!io.has("x"));
        io.write("x", x);
    }
    {   // read
        HDF5 io (fname, "r");
        REQUIRE(io.has("x"));
        {
            const auto dims = io.dimensions("x");
            REQUIRE(1 == dims.size());
            REQUIRE(3 == dims[0]);
        }
        // auto xr = io.read<double>("x");
        // REQUIRE(xr == x);
    }
}


TEST_CASE_FIXTURE(HDF5Test, "write_int")
{
    const std::vector<int> x ({1, 2, 3});
    {   // write
        HDF5 io (fname, "w");
        REQUIRE(!io.has("x"));
        io.write("x", x);
    }
    {   // read
        HDF5 io (fname, "r");
        REQUIRE(io.has("x"));
        auto xr = io.read<int>("x");
        REQUIRE(xr == x);
    }
}


TEST_CASE_FIXTURE(HDF5Test, "compress_int")
{
    const std::vector<int> x ({1, 2, 3});
    {   // write
        HDF5 io (fname, "w");
        REQUIRE_THROWS_AS(io.set_compression(10), std::range_error);
        io.set_compression(5);
        REQUIRE(!io.has("x"));
        io.write("x", x);
        // check via `h5dump -p -H ./test.h5`
    }
    {   // read
        HDF5 io (fname, "r");
        REQUIRE(io.has("x"));
        auto xr = io.read<int>("x");
        REQUIRE(xr == x);
    }
}


TEST_CASE_FIXTURE(HDF5Test, "group_empty0")
{
    HDF5 io (fname, "w");
    REQUIRE("/" == io.group());
}


TEST_CASE_FIXTURE(HDF5Test, "group_empty")
{
    HDF5 io (fname, "w");
    REQUIRE("/" == io.group());
    REQUIRE("/bla" == io.group("bla"));
}


TEST_CASE_FIXTURE(HDF5Test, "group_empty2")
{
    HDF5 io (fname, "w");
    REQUIRE("/" == io.group());
    REQUIRE("/bla" == io.group("bla"));
    REQUIRE("/bla/bli" == io.group("bli"));
}


TEST_CASE_FIXTURE(HDF5Test, "group")
{
    const std::vector<int> x ({1, 2, 3});
    {   // write
        HDF5 io (fname, "w");
        REQUIRE("/" == io.group());
        REQUIRE("/blub" == io.group("blub"));
        REQUIRE("/blub" == io.group());
        io.write("x", x);
    }
    // {   // read, absolute
    //     HDF5 io (fname, "r");
    // REQUIRE("/" == io.group());
    // REQUIRE(io.has("/blub"));
    // REQUIRE("/blub" == io.group("blub"));
    // REQUIRE("/blub" == io.group());
    // REQUIRE(io.has("x"));
    //     auto xr = io.read<int>("/blub/x");
    // REQUIRE(xr == x);
    // }
    // {   // read, relative
    //     HDF5 io (fname, "r");
    // REQUIRE_THROWS_AS(io.group("bla"), std::runtime_error);
    //     io.group("blub");
    // REQUIRE(io.has("x"));
    //     auto xr = io.read<int>("x");
    // REQUIRE(xr == x);
    // }
}


TEST_CASE_FIXTURE(HDF5Test, "read_not_exists")
{
    {
        HDF5 io (fname, "w");
    }
    {
        HDF5 io (fname);
        REQUIRE_THROWS_AS(io.read<int>("bb"), std::runtime_error);
    }
}



TEST_CASE_FIXTURE(HDF5Test, "write_readonly")
{
    {
        HDF5 io (fname, "w");
    }
    {
        HDF5 io (fname, "r");
        REQUIRE_THROWS_AS(io.write<int>("bb", std::vector<int>({1})),
                          std::runtime_error);
    }
}
