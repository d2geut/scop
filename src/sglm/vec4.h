#ifndef __VEC4_H__
#define __VEC4_H__

#include <cassert>
#include <cmath>
#include "vec3.h"

namespace sglm {
    struct vec4 {
        union {
            struct { float x, y, z, w; };
            struct { float r, g, b, a; };
            float data[4];
        };
        
        // »ý¼ºÀÚ
        vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
        explicit vec4(float _s) : x(_s), y(_s), z(_s), w(_s) {}
        vec4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
        vec4(const vec3& _v, float _w) : x(_v.x), y(_v.y), z(_v.z), w(_w) {}

        // ¹è¿­ ÀÎµ¦½º ¿¬»êÀÚ ¿À¹ö·Îµù
        float& operator[](const int i) {
            assert(i >= 0);
            assert(i < 4);

            return data[i];
        }
        const float& operator[](const int i) const {
            assert(i >= 0);
            assert(i < 4);

            return data[i];
        }
    };

    // µ¡¼À
    vec4 operator+(const vec4& v1, const vec4& v2) {
        return vec4(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
    }
    // »¬¼À
    vec4 operator-(const vec4& v1, const vec4& v2) {
        return vec4(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w);
    }
    // ½ºÄ®¶ó °ö¼À
    vec4 operator*(const vec4& v, float s) {
        return vec4(v.x * s, v.y * s, v.z * s, v.w * s);
    }
    // dot product
    float dot(const vec4& v1, const vec4& v2) {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
    }
    // length
    float length(const vec4& v) {
        return sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2) + pow(v.w, 2));
    }
}

#endif