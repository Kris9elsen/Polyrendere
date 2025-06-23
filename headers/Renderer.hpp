#ifndef RENDERER_HPP
#define RENDERER_HPP

#pragma once

#include "Render_math.hpp"
#include "Renderable.hpp"

#include <vector>
#include <iostream> // FOR DEBUG REMOVE LATER
#include <X11/Xlib.h>
#include <cstring>
#include <cmath>
#include <cstdint>

class Renderer {
public:
    Renderer(int width, int height);

    // Inits display window
    void init_x11();

    // METHODS

    // Project vertex from model space to screen space
    Vec3 project_vertex(const Vec3& vertex, const Mat4& model_matrix) const;

    // Render wireframe
    void render_wireframe(const Renderable& obj);

    // Render multiple wireframes
    void render_wireframes();

    // Draw line on screen
    void draw_line(int x0, int y0, int x1, int y1, uint32_t color);

    // Set color of pixel
    void put_pixel(int x, int y, uint32_t color);

   // Clears display with one color
    void clear(uint32_t color);

    // Render a rotating box
    void render_rotating_box(float angle);

    // Sends framebuffer to display
    void show();

    // Add object to the scene
    void add_object(Renderable* obj);


    // SETTERS
    
    // Set camera position and direction
    void set_camera(const Vec3& eye, const Vec3& target, const Vec3& up);

    // Set projection fov nearZ and farZ
    void set_projection(float fov, float nearZ, float farZ);

    



protected:
    int width, height;     // Window size
    Display* display;      // Display
    Window window;         // Window
    GC gc;                 // gc
    XImage* ximage;        // Image
    uint32_t* framebuffer; // Framebuffer
    Mat4 view;             // Camera matrix
    Mat4 projection;       // Projection to screen matrix
    std::vector<Renderable*> objects; // Objects to render in scene
};

#endif