#ifndef RENDERABLE_HPP
#define RENDERABLE_HPP

#pragma once

#include "Render_math.hpp"

#include <vector>
#include <stdint.h>

class Renderable {
public:
    virtual ~Renderable() = default;

    // Returns vector<Vec3> with vertices
    virtual const std::vector<Vec3>& get_vertices() const = 0;

    // Returns vector<uint32_t> with indices
    virtual const std::vector<uint32_t>& get_indices() const = 0;

    // Returns model matrix containing rotation, position and scale
    virtual const Mat4 get_model_matrix() const = 0;

};

#endif