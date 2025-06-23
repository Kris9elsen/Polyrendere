#ifndef RENDERER_HPP
#define RENDERER_HPP

#pragma once

#include "Render_math.hpp"
#include "Renderable.hpp"

#include <vector>
#include <iostream> // FOR DEBUG REMOVE LATER

class Renderer {
public:
    Renderer(int width, int height);

    // METHODS

    // Project vertex from model space to screen space
    Vec3 project_vertex(const Vec3& vertex, const Mat4& model_matrix) const;

    // Render wireframe
    void render_wireframe(const Renderable& obj);

    // Draw line on screen
    void draw_line(const Vec3& p1, const Vec3& p2, uint32_t color);


    // SETTERS
    
    // Set camera position and direction
    void set_camera(const Vec3& eye, const Vec3& target, const Vec3& up);

    // Set projection fov nearZ and farZ
    void set_projection(float fov, float nearZ, float farZ);



protected:
    int width, height;
    Mat4 view;
    Mat4 projection;
};

#endif