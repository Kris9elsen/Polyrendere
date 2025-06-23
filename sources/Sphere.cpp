#include "Sphere.hpp"

Sphere::Sphere(float radius, int latSegments, int longSegments) {
    generate_mesh(radius, latSegments, longSegments);
}

// Generates vertices and indices for sphere mesh
void Sphere::generate_mesh(float radius, int latSegments, int longSegments) {
    vertices.clear();
    indices.clear();

    // Generate vertices
    for (int lat = 0; lat <= latSegments; ++lat) {
        float theta = lat * M_PI / latSegments;
        float sinTheta = std::sin(theta);
        float cosTheta = std::cos(theta);

        for (int lon = 0; lon <= longSegments; ++lon) {
            float phi = lon * 2.0f * M_PI / longSegments;
            float sinPhi = std::sin(phi);
            float cosPhi = std::cos(phi);

            float x = radius * sinTheta * cosPhi;
            float y = radius * cosTheta;
            float z = radius * sinTheta * sinPhi;

            vertices.emplace_back(x, y, z);
        }
    }

    for (int lat = 0; lat < latSegments; ++lat) {
        for (int lon = 0; lon < longSegments; ++lon) {
            int first = lat * (longSegments + 1) + lon;
            int second = first + longSegments + 1;

            // First triangle
            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            // Second triangle
            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }
}

// GETTERS
// Returns vector<Vec3> with vertices
const std::vector<Vec3>& Sphere::get_vertices() const {
    return vertices;
}

// Returns vector<uint32_t> with indices
const std::vector<uint32_t>& Sphere::get_indices() const {
    return indices;
}

// Returns Mat4 with model matrix
const Mat4 Sphere::get_model_matrix() const {
    Mat4 T = Mat4::translation(pos.x, pos.y, pos.z);
    Mat4 Rx = Mat4::rot_x(rot.x);
    Mat4 Ry = Mat4::rot_y(rot.y);
    Mat4 Rz = Mat4::rot_z(rot.z);
    Mat4 S = Mat4::scale(scale.x, scale.y, scale.z);

    return T * Rz * Ry * Rx * S;
}

// SETTERS
// Set position Vec3 {x, y, z}
void Sphere::set_position(const Vec3& _pos) {
    pos = _pos;
}

// Set rotation Vec3 {rotX, rotY, rotZ}
void Sphere::set_rotation(const Vec3& _rot) {
    rot = _rot;
}

// Set scale Vec3 {scale x, scale y, scale z}
void Sphere::set_scale(const Vec3& _scale) {
    scale = _scale;
}