#ifndef RENDER_MATH_HPP
#define RENDER_MATH_HPP

#pragma once

#include <cmath>

// 2D Vector
struct Vec2 {
    float x, y;

    Vec2();
    Vec2(float x, float y);

    // Addition Vec
    Vec2 operator+(const Vec2& other) const {
        return Vec2(x + other.x, y + other.y);
    };

    // Subtract vec
    Vec2 operator-(const Vec2& other) const {
        return Vec2(x - other.x, y - other.y);
    };

    // Multiply Vec
    Vec2 operator*(const Vec2& other) const {
        return Vec2(x * other.x, y * other.y);
    }; 

    // Normalize Vec
    Vec2 norm() const {
        float length = len();
        return Vec2(x / length, y / length);
    };

    // Dot product of Vec
    float dot(const Vec2& other) const {
        return ((x * other.x) + (y * other.y));
    };

    // length of Vec
    float len() const {
        return sqrt(pow(x, 2) + pow(y, 2));
    };

    // Croos product of Vec
    float cross(const Vec2& other) const {
        return ((x * other.y) - (y * other.x));
    };

};

// 3D Vector
struct Vec3 {
    float x, y, z;

    Vec3();
    Vec3(float x, float y, float z);

    // Addition Vec
    Vec3 operator+(const Vec3& other) const {
        return Vec3(x + other.x, y + other.y, z + other.z);
    };

    // Subtract Vec
    Vec3 operator-(const Vec3& other) const {
        return Vec3(x - other.x, y - other.y, z - other.z);
    };

    // Multiply Vec
    Vec3 operator*(const Vec3& other) const {
        return Vec3(x * other.x, y * other.y, z * other.z);
    };

    // Cross product of Vec
    Vec3 cross(const Vec3& other) const {
        return Vec3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
    };

    // Normalize Vec
    Vec3 norm() const {
        float length = len();
        return Vec3(x / length, y / length, z / length);
    };

    // Dot product of Vec
    float dot(const Vec3& other) const {
        return ((x * other.x) + (y * other.y) + ( z * other.z));
    };

    // Length of Vec
    float len() const {
        return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
    };

};

// 3x3 Matrix
struct Mat3 {
    float m[3][3];

    // Identity matrix
    static Mat3 indentity() {
        
    };

    // Rotate matrix by angle in radians
    static Mat3 rotation(float angleRadians) {

    };

    // Translate matrix
    static Mat3 translation(float tx, float ty) {

    };

    // Scale matrix
    static Mat3 scale(float sx, float sy) {

    };

    // Transform matrix by 2D Vec
    Vec2 transform(const Vec2& v) const {

    };

    // Matrix multiplication
    Mat3 operator*(const Mat3& other) const {

    };
};

#endif