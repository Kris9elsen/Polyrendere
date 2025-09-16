#ifndef RENDERER_HPP
#define RENDERER_HPP

#pragma once

#include "Render_math.hpp"
#include "Renderable.hpp"

#include <vector>
#include <array>
#include <iostream> // FOR DEBUG REMOVE LATER
#include <X11/Xlib.h>
#include <cstring>
#include <cmath>
#include <cstdint>
#include <algorithm>

class Renderer {
public:
    Renderer(int width, int height);

    // Inits display window
    void init_x11();

    // Enable SSAA and set factor
    void enable_ssaa(int facotr);

    // Disable SSAA
    void disable_ssaa();

    // METHODS

    // Project vertex from model space to screen space
    Vec3 project_vertex(const Vec3& vertex, const Mat4& model_matrix) const;

    // Render wireframe
    void render_wireframe(const Renderable& obj);

    // Render multiple wireframes
    void render_wireframes();

    // Render filled object
    void render_filled(const Renderable& obj);

    // Draw line on screen
    void draw_line(Vec3 v0, Vec3 v1, uint32_t color);

    // Set color of pixel
    void put_pixel(int x, int y, float z, uint32_t color); // For drendering with respect to depth
    void put_pixel(int x, int y, uint32_t color); // Does not respect depth

   // Clears display with one color
    void clear(uint32_t color);

    // Render a rotating box
    void render_rotating_box(float angle);

    // Sends framebuffer to display
    void show();

    // Add object to the scene
    void add_object(Renderable* obj);

    // Clamps screen coordinates
    inline int clamp(int value, int min, int max);

    // Transform model to clip
    Vec4 model_to_clip(const Vec3& vertex, const Mat4& model_matrix) const;

    // Helper to compute baryentric coordinates
    inline bool Renderer::barycentric(const Vec3& p, const Vec3& a, const Vec3& b, const Vec3& c, float& u, float& v, float& w);

    // Rasterize a single triangle
    void Renderer::draw_triangle(const Vec3& v0, const Vec3& v1, const Vec3& v2, uint32_t color);

    // SETTERS
    
    // Set camera position and direction
    void set_camera(const Vec3& eye, const Vec3& target, const Vec3& up);

    // Set projection fov nearZ and farZ
    void set_projection(float fov, float nearZ, float farZ);

    // CLIP PLANE
    enum class Clip_plane {
        Left, Right, Bottom, Top, Near, Far
    };

    // Clip plane helper methods
    // Check if inside clip plane
    static bool inside(const Vec4& v, Clip_plane plane);

    // Interpolate clipping
    static Vec4 interpolate(const Vec4& a, const Vec4& b, Clip_plane plane);

    // Clip polygon against a single plane
    std::vector<Vec4> clip_poly(const std::vector<Vec4>& vertices, Renderer::Clip_plane plane);

    // Clip triangles
    std::vector<std::array<Vec4, 3>> clip_triangle(const std::array<Vec4, 3>& triangle);

protected:
    int width, height;     // Window size
    int size;              // Size of framebuffer
    Display* display;      // Display
    Window window;         // Window
    GC gc;                 // gc
    XImage* ximage;        // Image
    uint32_t* framebuffer; // Framebuffer
    Mat4 view;             // Camera matrix
    Mat4 projection;       // Projection to screen matrix
    std::vector<Renderable*> objects; // Objects to render in scene
    std::vector<float> zbuffer;       // Z-Buffer for depth perspective

    // SSAA
    bool ssaa = false;     // Enable SSAA
    int ssaa_factor;       // SSAA factor
    int ssaa_width;        // Width of SSAA buffer
    int ssaa_height;       // Height of SSAA buffer
    int ssaa_size;         // Size of SSAA buffer
    int ssaa_samples;      // Sample size of SSAA
    uint32_t* ssaa_buffer; // SSAA framebuffer
};

#endif