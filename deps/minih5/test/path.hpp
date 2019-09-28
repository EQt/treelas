#pragma once
#include <fstream>
#include <filesystem>


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
        return std::filesystem::current_path().string();
    }


    static std::string abspath(const char *fname) {
        return path::cwd() + "/" + fname;
    }
};
