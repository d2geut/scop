#ifndef __VEC3_H__
#define __VEC3_H__

#include <cassert>
#include <cmath>

namespace sglm {
    struct vec3 {
        union {
            struct { float x, y, z; };
            struct { float r, g, b; };
            float data[3];
        };

        // »ý¼ºÀÚ
        vec3() : x(0.0f), y(0.0f), z(0.0f) {}
        explicit vec3(float _s) : x(_s), y(_s), z(_s) {}
        vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

        // ¹è¿­ ÀÎµ¦½º ¿¬»êÀÚ [] ¿À¹ö·Îµù
        float& operator[](const int i) {
            assert(i >= 0);
            assert(i < 3);

            return data[i];
        }
        const float& operator[](const int i) const {
            assert(i >= 0);
            assert(i < 3);

            return data[i];
        }

        // µ¥ÀÌÅÍ Æ÷ÀÎÅÍ ³Ñ±â±â
        const float* value_ptr() const {
            return &data[0];
        }
    };

    // µ¡¼À
    vec3 operator+(const vec3& v1, const vec3& v2) {
        return vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
    }
    // »¬¼À
    vec3 operator-(const vec3& v1, const vec3& v2) {
        return vec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
    }
    // ½ºÄ®¶ó °ö¼À
    vec3 operator*(const vec3& v, float s) {
        return vec3(v.x * s, v.y * s, v.z * s);
    }
    // dot product
    float dot(const vec3& v1, const vec3& v2) {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }
    // cross product
    vec3 cross(const vec3& v1, const vec3& v2) {
        return vec3(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y - v2.x);
    }
    // length
    float length(const vec3& v) {
        return sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
    }
}

#endif