#include "Cube.hpp"

Cube::Cube() {
    vertices = {
        {-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},
        {-1, -1, 1}, {1, -1, 1}, {1, 1, 1}, {-1, 1, 1}
    };

    indices = {
        0, 1, 2, 2, 3, 0, // Back
        4, 5, 6, 6, 7, 4, // Front
        0, 1, 5, 5, 4, 0, // Bottom
        2, 3, 7, 7, 6, 2, // Top
        0, 3, 7, 7, 4, 2, // Left
        1, 2, 6, 6, 5, 1  // Right
    };
}

// Returns model matrix for position, rotation and scale
Mat4 Cube::get_model_matrix() const {
    Mat4 T = Mat4::translation(pos.x, pos.y, pos.z);
    Mat4 Rx = Mat4::rot_x(rot.x);
    Mat4 Ry = Mat4::rot_y(rot.y);
    Mat4 Rz = Mat4::rot_z(rot.z);
    Mat4 S = Mat4::scale(scale.x, scale.y, scale.z);

    return T * Rz * Ry * Rx * S;
}

// GETTERS

// Returns vector<Vec3> with vertices
const std::vector<Vec3>& Cube::get_vertices() const {
    return vertices;
}

// Returns vector<uint32_t> with indices
const std::vector<uint32_t>& Cube::get_indices() const {
    return indices;
}

// SETTERS

// Set position vector Vec3 {x, y, z}
void Cube::set_position(const Vec3& _pos) {
    pos = _pos;
}

// Set rotation vector Vec3 {rotX, rotY, rotZ}
void Cube::set_rotation(const Vec3& _rot) {
    rot = _rot;
}

// Set scale Vec3 {x scale, y scale, z scale}
void Cube::set_scale(const Vec3& _scale) {
    scale = _scale;
}

