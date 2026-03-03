#ifndef __MAT4_H__
#define __MAT4_H__

#include <cassert>
#include <cmath>
#include "vec4.h"
#include "vec3.h"
#include "sglm_ext.h"

namespace sglm {
    struct mat4 {
        union {
            vec4 value[4];
            float data[16];
        };

        // 생성자
        explicit mat4(float _s = 1.0f) {
            value[0] = vec4(_s, 0, 0, 0);
            value[1] = vec4(0, _s, 0, 0);
            value[2] = vec4(0, 0, _s, 0);
            value[3] = vec4(0, 0, 0, _s);
        }
        mat4(const vec4& v0, const vec4& v1, const vec4& v2, const vec4& v3) {
            value[0] = v0;
            value[1] = v1;
            value[2] = v2;
            value[3] = v3;
        }
        mat4(float m00, float m01, float m02, float m03,
            float m10, float m11, float m12, float m13,
            float m20, float m21, float m22, float m23,
            float m30, float m31, float m32, float m33)
        {
            value[0] = vec4(m00, m01, m02, m03);
            value[1] = vec4(m10, m11, m12, m13);
            value[2] = vec4(m20, m21, m22, m23);
            value[3] = vec4(m30, m31, m32, m33);
        }

        // 배열 인덱스 연산자 오버로딩
        vec4& operator[](const int i) {
            assert(i >= 0);
            assert(i < 4);

            return value[i];
        }
        const vec4& operator[](const int i) const {
            assert(i >= 0);
            assert(i < 4);

            return value[i];
        }
    };

    vec4 operator*(const mat4& m, const vec4& v) {
        return m[0] * v.x + m[1] * v.y + m[2] * v.z + m[3] * v.w;
    }

    mat4 operator*(const mat4& m1, const mat4& m2) {
        mat4 result;

        for (int col = 0; col < 4; col++) {
            result[col] = m1 * m2[col];
        }
        return result;
    }

    mat4 translate(const mat4& m, const vec3& v) {
        return mat4(m.value[0], m.value[1], m.value[2], m.value[3] + vec4(v, 0.0f));
    }
    
    mat4 rotate(const mat4& m, float angle, const vec3& r) {
        float s = std::sin(angle);
        float c = std::cos(angle);

        return mat4(c + pow(r.x, 2) * (1-c), r.y * r.x * (1-c) + r.z * s, r.z * r.x * (1-c) - r.y * s, 0,
            r.x * r.y * (1-c) - r.z * s, c + pow(r.y, 2) * (1-c), r.z * r.y * (1-c) + r.x * s, 0,
            r.x * r.z * (1-c) + r.y * s, r.y * r.z * (1-c) - r.x * s, c + pow(r.z, 2) * (1-c), 0,
            0, 0, 0, 1);
    }

    mat4 scale(const mat4& m, const vec3& v) {
        return mat4(m[0] * v.x, m[1] * v.y, m[2] * v.z, m[3]);
    }
}

#endif