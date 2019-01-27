#pragma once
#include <fstream>
#include <gtest/internal/gtest-filepath.h>


class path
{
public:
    static bool exists(const char *fname) {
        std::ifstream f (fname);
        return f.good();
    }


    static int remove(const char *fname) {
        return ::remove(fname);
    }

    static std::string cwd() {
        return ::testing::internal::FilePath::GetCurrentDir().string();
    }


    static std::string abspath(const char *fname) {
        return path::cwd() + "/" + fname;
    }
};
