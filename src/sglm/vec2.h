#ifndef __VEC2_H__
#define __VEC2_H__

#include <cassert>
#include <cmath>

namespace sglm {
    struct vec2 {
        union {
            struct { float x, y; };
            float data[2];
        };

        // »ý¼ºÀÚ
        vec2() : x(0.0f), y(0.0f) {}
        explicit vec2(float _s) : x(_s), y(_s) {}
        vec2(float _x, float _y) : x(_x), y(_y) {}

        // ¹è¿­ ÀÎµ¦½º ¿¬»êÀÚ [] ¿À¹ö·Îµù
        float& operator[](const int i) {
            assert(i >= 0);
            assert(i < 2);

            return data[i];
        }
        const float& operator[](const int i) const {
            assert(i >= 0);
            assert(i < 2);

            return data[i];
        }

        // µ¥ÀÌÅÍ Æ÷ÀÎÅÍ ³Ñ±â±â
        const float* value_ptr() const {
            return &data[0];
        }
    };

    // µ¡¼À
    inline vec2 operator+(const vec2& v1, const vec2& v2) {
        return vec2(v1.x + v2.x, v1.y + v2.y);
    }
    // »¬¼À
    inline vec2 operator-(const vec2& v1, const vec2& v2) {
        return vec2(v1.x - v2.x, v1.y - v2.y);
    }
    // ½ºÄ®¶ó °ö¼À
    inline vec2 operator*(const vec2& v, float s) {
        return vec2(v.x * s, v.y * s);
    }
    // dot product
    inline float dot(const vec2& v1, const vec2& v2) {
        return v1.x * v2.x + v1.y * v2.y;
    }
    // length
    inline float length(const vec2& v) {
        return sqrt(pow(v.x, 2) + pow(v.y, 2));
    }
    // normalize
    inline vec2 normalize(const vec2& v) {
        float l = length(v);

        return vec2(v.x / l, v.y / l);
    }
}

#endif