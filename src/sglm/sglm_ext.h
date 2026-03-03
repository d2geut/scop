#ifndef __SGLM_EXT_H__
#define __SGLM_EXT_H__

#include <cmath>

#define PI static_cast<float>(3.14159265358979323846)

namespace sglm {
    float radians(float r) {
        return r * (PI / 180);
    }

    // data¿« raw potiner
    template <typename T>
    const float* value_ptr(const T& v) {
        return &(v.data[0]);
    }
};


#endif