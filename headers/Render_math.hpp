#ifndef RENDER_MATH_HPP
#define RENDER_MATH_HPP

#pragma once

#include <cmath>

// 2D Vector
struct Vec2 {
    float x, y;

    Vec2() : x(0), y(0) {}
    Vec2(float x, float y) : x(x), y(y) {}

    // Addition Vec
    Vec2 operator+(const Vec2& rhs) const {
        return Vec2(x + rhs.x, y + rhs.y);
    }

    // Subtract vec
    Vec2 operator-(const Vec2& rhs) const {
        return Vec2(x - rhs.x, y - rhs.y);
    }

    // Multiply Vec
    Vec2 operator*(float scalar) const {
        return Vec2(x * scalar, y * scalar);
    }

    // Normalize Vec
    Vec2 norm() const {
        float length = len();
        return length > 0 ? *this * (1.0f / length) : Vec2();
    }

    // Dot product of Vec
    float dot(const Vec2& rhs) const {
        return ((x * rhs.x) + (y * rhs.y));
    }

    // length of Vec
    float len() const {
        return sqrt(x * x + y * y);
    }

    // Croos product of Vec
    float cross(const Vec2& rhs) const {
        return ((x * rhs.y) - (y * rhs.x));
    }

};

// 3D Vector
struct Vec3 {
    float x, y, z;

    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    // Addition Vec
    Vec3 operator+(const Vec3& rhs) const {
        return Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
    }

    // Subtract Vec
    Vec3 operator-(const Vec3& rhs) const {
        return Vec3(x - rhs.x, y - rhs.y, z - rhs.z);
    }

    // Multiply Vec
    Vec3 operator*(float scalar) const {
        return Vec3(x * scalar, y * scalar, z * scalar);
    }

    // Cross product of Vec
    Vec3 cross(const Vec3& rhs) const {
        return Vec3(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);
    }

    // Normalize Vec
    Vec3 norm() const {
        float length = len();
        return length > 0 ? *this * (1.0f / length) : Vec3();
    }

    // Dot product of Vec
    float dot(const Vec3& rhs) const {
        return ((x * rhs.x) + (y * rhs.y) + ( z * rhs.z));
    }

    // Length of Vec
    float len() const {
        return sqrt(x * x + y * y + z * z);
    }

};

// 4D Homo Vector
struct Vec4 {
    float x, y, z, w;

    Vec4() : x(0), y(0), z(0), w(0) {}
    Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

    // Addition Vec
    Vec4 operator+(const Vec4& rhs) const {
        return Vec4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
    }

    // Subtraction Vec
    Vec4 operator-(const Vec4& rhs) const {
        return Vec4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
    }

    // Multiply Vec
    Vec4 operator*(float scalar) const {
        return Vec4(x * scalar, y * scalar, z * scalar, w * scalar);
    }

    // Dot product of Vec
    float dot(const Vec4& rhs) const {
        return ((x * rhs.x) + (y * rhs.y) + (z * rhs.z) + (w * rhs.w));
    }

    Vec3 homo() const {
        return Vec3(x / w, y / w, z / w);
    }

};

// 3x3 Matrix
struct Mat3 {
    float m[3][3];

    // Identity matrix
    static Mat3 indentity() {
        return {{
            {1, 0, 0},
            {0, 1, 0},
            {0, 0, 1}
        }};

    }

    // Rotation matrix around z-axis
    static Mat3 rotation(float angleRadians) {
        float c = std::cos(angleRadians);
        float s = std::sin(angleRadians);

        return {{
            {c, -s, 0},
            {s, c, 0},
            {0, 0, 1}
        }};

    }

    // Translate matrix
    static Mat3 translation(float tx, float ty) {
        return {{
            {1, 0, tx},
            {0, 1, ty},
            {0, 0, 1}
        }};

    }

    // Scale matrix
    static Mat3 scale(float sx, float sy) {
        return {{
            {sx, 0, 0},
            {0, sy, 0},
            {0, 0, 1}
        }};

    }

    // Transform matrix by 2D Vec
    Vec2 transform(const Vec2& v) const {
        return {
            m[0][0]*v.x + m[0][1]*v.y + m[0][2],
            m[1][0]*v.x + m[1][1]*v.y + m[1][2]
        };

    }

    // Multiply 3x3 matrix
    Mat3 operator*(const Mat3& rhs) const {
        Mat3 result;
        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                for (int k = 0; k < 3; ++k) {
                    result.m[row][col] += m[row][k] * rhs.m[k][col];
                }

            }

        }
    
        return result;
    }

};

// 4x4 homo matrix
struct Mat4 {
    float m[4][4];

    Mat4() {
        for (int r = 0; r < 4; ++r) {
            for (int c = 0; c < 4; ++c) {
                m[r][c] = 0;
            }
        }
    }

    // Identity matrix
    static Mat4 identity() {
        Mat4 mat;
        for (int i = 0; i < 4; ++i) mat.m[i][i] = 1.0f;
        return mat;
    }

    // Translation matrix
    static Mat4 translation(float tx, float ty, float tz) {
        Mat4 mat = identity();
        mat.m[0][3] = tx;
        mat.m[1][3] = ty;
        mat.m[2][3] = tz;
        return mat;
    }

    // Scale matrix
    static Mat4 scale(float sx, float sy, float sz) {
        Mat4 mat = identity();
        mat.m[0][0] = sx;
        mat.m[1][1] = sy;
        mat.m[2][2] = sz;
        return mat;
    }

    // Rotation matrix around x-axis
    static Mat4 rot_x(float angleRad) {
        float c = std::cos(angleRad);
        float s = std::sin(angleRad);
        Mat4 mat = identity();
        mat.m[1][1] = c; mat.m[1][2] = -s;
        mat.m[2][1] = s; mat.m[2][2] = c;
        return mat;

    }

    // Rotation matrix around y-axis
    static Mat4 rot_y(float angleRad) {
        float c = std::cos(angleRad);
        float s = std::sin(angleRad);
        Mat4 mat = identity();
        mat.m[0][0] = c; mat.m[0][2] = s;
        mat.m[2][0] = -s; mat.m[2][2] = c;
        return mat;
    }

    // Rotation matrix around z-axis
    static Mat4 rot_z(float angleRad) {
        float c = std::cos(angleRad);
        float s = std::sin(angleRad);
        Mat4 mat = identity();
        mat.m[0][0] = c; mat.m[0][1] = -s;
        mat.m[1][0] = s; mat.m[1][1] = c;
        return mat;
    }

    // Perspective matrix
    static Mat4 perspective(float fov, float aspect, float nearZ, float farZ) {
        Mat4 mat;
        float tan_half_fov = std::tan(fov / 2.0f);
        mat.m[0][0] = 1.0f / (aspect * tan_half_fov);
        mat.m[1][1] = 1.0f / tan_half_fov;
        mat.m[2][2] = -(farZ + nearZ) / (farZ - nearZ);
        mat.m[2][3] = -(2.0f * farZ * nearZ) / (farZ - nearZ);
        mat.m[3][2] = -1.0f;
        return mat;
    }

    // Looking at matrix
    static Mat4 look_at(const Vec3& eye, const Vec3& target, const Vec3& up) {
        Vec3 f = (target - eye).norm();
        Vec3 r = f.cross(up).norm();
        Vec3 u = r.cross(f);

        Mat4 mat = identity();
        mat.m[0][0] = r.x; mat.m[0][1] = r.y; mat.m[0][2] = r.z;
        mat.m[1][0] = u.x; mat.m[1][1] = u.y; mat.m[1][2] = u.z;
        mat.m[2][0] = -f.x; mat.m[2][1] = -f.y; mat.m[2][2] = -f.z;
        mat.m[0][3] = -r.dot(eye);
        mat.m[1][3] = -u.dot(eye);
        mat.m[2][3] = f.dot(eye);
        return mat;
    }

    // Transform Vec4
    Vec4 transform(const Vec4& v) const {
        return Vec4(
            m[0][0]*v.x + m[0][1]*v.y + m[0][2]*v.z + m[0][3]*v.w,
            m[1][0]*v.x + m[1][1]*v.y + m[1][2]*v.z + m[1][3]*v.w,
            m[2][0]*v.x + m[2][1]*v.y + m[2][2]*v.z + m[2][3]*v.w,
            m[3][0]*v.x + m[3][1]*v.y + m[3][2]*v.z + m[3][3]*v.w
        );

    }

    // Multiply 4x4 matrix
    Mat4 operator*(const Mat4& rhs) const {
        Mat4 result;
        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                for (int k = 0; k < 4; ++k) {
                    result.m[row][col] += m[row][k] * rhs.m[k][col];
                }

            }

        }
    
        return result;
    }

};

#endif