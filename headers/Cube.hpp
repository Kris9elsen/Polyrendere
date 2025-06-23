#ifndef CUBE_HPP
#define CUBE_HPP

#pragma once

#include "Render_math.hpp"
#include "Renderable.hpp"

#include <vector>
#include <cstdint>

class Cube : public Renderable {
public:
    Cube();

    // GETTERS
    // Returns vertices
    const std::vector<Vec3>& get_vertices() const override;

    // Returns indices
    const std::vector<uint32_t>& get_indices() const override;

    // Get Model matrix
    const Mat4 get_model_matrix() const override;

    // SETTERS
    // Set position Vec3 {x, y, z}
    void set_position(const Vec3& _pos);

    // Set rotation Vec3 {rotX, rotY, rotZ}
    void set_rotation(const Vec3& _rot);

    // Set scale Vec3 {x scale, y scale, z scale}
    void set_scale(const Vec3& _scale);

protected:
    std::vector<Vec3> vertices;
    std::vector<uint32_t> indices;

    Vec3 pos = {0, 0, 0};
    Vec3 rot = {0, 0, 0}; // Euler angles
    Vec3 scale = {1, 1, 1};

};

#endif