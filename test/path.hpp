#pragma once
#include <iostream>
#include <cstdio>
#include <gtest/internal/gtest-filepath.h>


class path
{
public:
    static bool exists(const char *fname) {
        std::FILE *io = fopen(fname, "r");
        if (io) {
            fclose(io);
            return true;
        }
        return false;
    }


    static void remove(const char *fname) {
        ::remove(fname);
    }

    static std::string cwd() {
        return ::testing::internal::FilePath::GetCurrentDir().string();
    }


    static std::string abspath(const char *fname) {
        return path::cwd() + "/" + fname;
    }
};
