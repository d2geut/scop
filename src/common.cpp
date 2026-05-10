#include "common.h"
#include <iostream>
#include <fstream>
#include <sstream>

std::optional<std::string> LoadTextFile(const std::string& filename) {
    std::ifstream fin(filename);
    if (!fin.is_open()) {
        std::cerr << "failed to open file: " << filename << std::endl;
        return {};
    }
    std::stringstream text;
    text << fin.rdbuf();
    return text.str();
}

bool checkFileFormat(const std::string& filename, const std::string& format) {
    size_t length = filename.length();
    size_t f_length = format.length();
    if (length < f_length)
        return false;

    std::string fileformat = filename.substr(length-f_length);
    if (fileformat != format)
        return false;
    return true;
}
