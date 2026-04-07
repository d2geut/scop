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

bool isBMPFile(const std::string& filename) {
    size_t length = filename.length();
    if (length < 4)
        return false;

    std::string fileformat = filename.substr(filename.length()-4);
    if (fileformat != ".bmp")
        return false;
    return true;
}
