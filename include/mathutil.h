// mathutil.h — Lightweight math library for OpenGL 3.3
// Provides vec3, vec4, mat4, and common transformation functions.
// Column-major storage matches the OpenGL convention.
#pragma once

#include <cmath>
#include <cstring>

namespace math {

// ─── Constants ───────────────────────────────────────────────────────
const float PI = 3.14159265358979323846f;

inline float radians(float degrees) { return degrees * PI / 180.0f; }
inline float degrees(float radians) { return radians * 180.0f / PI; }

// ─── vec3 ────────────────────────────────────────────────────────────
struct vec3 {
    float x, y, z;

    vec3() : x(0), y(0), z(0) {}
    vec3(float s) : x(s), y(s), z(s) {}
    vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    vec3  operator+(const vec3& v) const { return { x + v.x, y + v.y, z + v.z }; }
    vec3  operator-(const vec3& v) const { return { x - v.x, y - v.y, z - v.z }; }
    vec3  operator*(float s)       const { return { x * s, y * s, z * s }; }
    vec3  operator/(float s)       const { return { x / s, y / s, z / s }; }
    vec3  operator-()              const { return { -x, -y, -z }; }
    vec3& operator+=(const vec3& v) { x += v.x; y += v.y; z += v.z; return *this; }
    vec3& operator-=(const vec3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
    vec3& operator*=(float s)       { x *= s; y *= s; z *= s; return *this; }

    float length()     const { return sqrtf(x * x + y * y + z * z); }
    float lengthSq()   const { return x * x + y * y + z * z; }
    vec3  normalized() const {
        float l = length();
        return l > 1e-8f ? *this / l : vec3(0.0f);
    }
};

inline float dot(const vec3& a, const vec3& b)   { return a.x * b.x + a.y * b.y + a.z * b.z; }
inline vec3  cross(const vec3& a, const vec3& b)  { return { a.y * b.z - a.z * b.y,
                                                              a.z * b.x - a.x * b.z,
                                                              a.x * b.y - a.y * b.x }; }
inline vec3  normalize(const vec3& v)             { return v.normalized(); }
inline vec3  operator*(float s, const vec3& v)    { return v * s; }

// ─── vec4 ────────────────────────────────────────────────────────────
struct vec4 {
    float x, y, z, w;

    vec4() : x(0), y(0), z(0), w(0) {}
    vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    vec4(const vec3& v, float w) : x(v.x), y(v.y), z(v.z), w(w) {}
};

// ─── mat4 (column-major) ────────────────────────────────────────────
// Storage: m[col * 4 + row]
struct mat4 {
    float m[16];

    mat4() { memset(m, 0, sizeof(m)); }

    static mat4 identity() {
        mat4 r;
        r.m[0] = r.m[5] = r.m[10] = r.m[15] = 1.0f;
        return r;
    }

    // Access element at (row, col)
    float& operator()(int row, int col)       { return m[col * 4 + row]; }
    float  operator()(int row, int col) const { return m[col * 4 + row]; }

    mat4 operator*(const mat4& b) const {
        mat4 r;
        for (int c = 0; c < 4; c++)
            for (int row = 0; row < 4; row++)
                for (int k = 0; k < 4; k++)
                    r(row, c) += (*this)(row, k) * b(k, c);
        return r;
    }

    // Returns pointer to the underlying float array (for glUniformMatrix4fv)
    const float* value_ptr() const { return m; }
};

// ─── Transformation Helpers ──────────────────────────────────────────

// Returns result = mat * Translation(v)
inline mat4 translate(const mat4& mat, const vec3& v) {
    mat4 r = mat;
    for (int i = 0; i < 4; i++)
        r(i, 3) = mat(i, 0) * v.x + mat(i, 1) * v.y + mat(i, 2) * v.z + mat(i, 3);
    return r;
}

// Returns result = mat * Scale(v)
inline mat4 scale(const mat4& mat, const vec3& v) {
    mat4 r = mat;
    for (int i = 0; i < 4; i++) {
        r(i, 0) = mat(i, 0) * v.x;
        r(i, 1) = mat(i, 1) * v.y;
        r(i, 2) = mat(i, 2) * v.z;
    }
    return r;
}

// Returns result = mat * Rotation(angle_radians, axis)
inline mat4 rotate(const mat4& mat, float angle, const vec3& axis) {
    float c = cosf(angle);
    float s = sinf(angle);
    vec3  a = normalize(axis);
    float t = 1.0f - c;

    mat4 rot = mat4::identity();
    rot(0, 0) = c + a.x * a.x * t;
    rot(1, 0) = a.y * a.x * t + a.z * s;
    rot(2, 0) = a.z * a.x * t - a.y * s;

    rot(0, 1) = a.x * a.y * t - a.z * s;
    rot(1, 1) = c + a.y * a.y * t;
    rot(2, 1) = a.z * a.y * t + a.x * s;

    rot(0, 2) = a.x * a.z * t + a.y * s;
    rot(1, 2) = a.y * a.z * t - a.x * s;
    rot(2, 2) = c + a.z * a.z * t;

    return mat * rot;
}

// ─── Projection / View ──────────────────────────────────────────────

// Standard OpenGL perspective projection
inline mat4 perspective(float fovRadians, float aspect, float zNear, float zFar) {
    mat4  r;
    float tanHalf = tanf(fovRadians / 2.0f);
    r(0, 0) = 1.0f / (aspect * tanHalf);
    r(1, 1) = 1.0f / tanHalf;
    r(2, 2) = -(zFar + zNear) / (zFar - zNear);
    r(2, 3) = -2.0f * zFar * zNear / (zFar - zNear);
    r(3, 2) = -1.0f;
    return r;
}

// Standard look-at view matrix
inline mat4 lookAt(const vec3& eye, const vec3& center, const vec3& up) {
    vec3 f = normalize(center - eye);
    vec3 s = normalize(cross(f, up));
    vec3 u = cross(s, f);

    mat4 r = mat4::identity();
    r(0, 0) =  s.x;  r(0, 1) =  s.y;  r(0, 2) =  s.z;
    r(1, 0) =  u.x;  r(1, 1) =  u.y;  r(1, 2) =  u.z;
    r(2, 0) = -f.x;  r(2, 1) = -f.y;  r(2, 2) = -f.z;
    r(0, 3) = -dot(s, eye);
    r(1, 3) = -dot(u, eye);
    r(2, 3) =  dot(f, eye);
    return r;
}

} // namespace math
