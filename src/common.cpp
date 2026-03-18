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

sglm::vec3 GetAttenuationCoeff(float distance) {
    const auto linear_coeff = sglm::vec4(
        8.4523112e-05, 4.4712582e+00, -1.8516388e+00, 3.3955811e+01
    );
    const auto quad_coeff = sglm::vec4(
        -7.6103583e-04, 9.0120201e+00, -1.1618500e+01, 1.0000464e+02
    );

    float kc = 1.0f;
    float d = 1.0f / distance;
    auto dvec = sglm::vec4(1.0f, d, d*d, d*d*d);
    float kl = sglm::dot(linear_coeff, dvec);
    float kq = sglm::dot(quad_coeff, dvec);

    return sglm::vec3(kc, sglm::max(kl, 0.0f), sglm::max(kq*kq, 0.0f));
}