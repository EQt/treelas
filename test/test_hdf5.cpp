#include <gtest/gtest.h>
#include "path.hpp"
#include "../cxx/utils/hdf5.hpp"

// https://github.com/google/googletest/blob/master/googletest/docs/Primer.md
class HDF5Test : public ::testing::Test
{
public:
    static constexpr auto fname = "test.h5";
    static constexpr bool delete_after = true;
protected:
    virtual void SetUp() {
        if (delete_after) {
            ASSERT_FALSE(path::exists(fname));
        }
    }

    virtual void TearDown() {
        if (delete_after) {
            if (path::exists(fname)) {
                path::remove(fname);
            }
            ASSERT_FALSE(path::exists(fname));
        } else {
            ASSERT_TRUE(path::exists(fname));
            fprintf(stderr, "Leaving %s/%s\n", path::cwd().c_str(), fname);
        }
    }
};


TEST_F(HDF5Test, create)
{
    if (delete_after) {
        ASSERT_THROW(HDF5(fname, "r"), std::runtime_error);
    }
    {
        HDF5 io (fname, "w");
    }   // automatically close it
    ASSERT_TRUE(path::exists(fname));
    ASSERT_NO_THROW(HDF5 io (fname, "r"));
}


TEST_F(HDF5Test, write_double)
{
    const std::vector<double> x ({1, 2, 3});
    {   // write
        HDF5 io (fname, "w");
        ASSERT_FALSE(io.has("x"));
        io.write("x", x);
    }
    {   // read
        HDF5 io (fname, "r");
        ASSERT_TRUE(io.has("x"));
        auto xr = io.read<double>("x");
        ASSERT_EQ(x, xr);
    }
}


TEST_F(HDF5Test, write_int)
{
    const std::vector<int> x ({1, 2, 3});
    {   // write
        HDF5 io (fname, "w");
        ASSERT_FALSE(io.has("x"));
        io.write("x", x);
    }
    {   // read
        HDF5 io (fname, "r");
        ASSERT_TRUE(io.has("x"));
        auto xr = io.read<int>("x");
        ASSERT_EQ(x, xr);
    }
}


TEST_F(HDF5Test, compress_int)
{
    const std::vector<int> x ({1, 2, 3});
    {   // write
        HDF5 io (fname, "w");
        ASSERT_THROW(io.set_compression(10), std::range_error);
        io.set_compression(5);
        ASSERT_FALSE(io.has("x"));
        io.write("x", x);
        // check via `h5dump -p -H ./test.h5`
    }
    {   // read
        HDF5 io (fname, "r");
        ASSERT_TRUE(io.has("x"));
        auto xr = io.read<int>("x");
        ASSERT_EQ(x, xr);
    }
}


TEST_F(HDF5Test, group)
{
    const std::vector<int> x ({1, 2, 3});
    {   // write
        HDF5 io (fname, "w");
        ASSERT_EQ(io.group(), "/");
        ASSERT_EQ(io.group("blub"), "/blub");
        ASSERT_EQ(io.group(), "/blub");
        io.write("x", x);
    }
    {   // read, absolute
        HDF5 io (fname, "r");
        ASSERT_TRUE(io.has("/blub/x"));
        auto xr = io.read<int>("/blub/x");
        ASSERT_EQ(x, xr);
    }
    {   // read, relative
        HDF5 io (fname, "r");
        ASSERT_THROW(io.group("bla"), std::runtime_error);
        io.group("blub");
        ASSERT_TRUE(io.has("x"));
        auto xr = io.read<int>("x");
        ASSERT_EQ(x, xr);
    }
}


TEST_F(HDF5Test, read_not_exists)
{
    {
        HDF5 io (fname, "w");
    }
    {
        HDF5 io (fname);
        ASSERT_THROW(io.read<int>("bb"), std::runtime_error);
    }
}


TEST_F(HDF5Test, write_readonly)
{
    {
        HDF5 io (fname, "w");
    }
    {
        HDF5 io (fname, "r");
        ASSERT_THROW(io.write<int>("bb", std::vector<int>({1})),
                     std::runtime_error);
    }
}
