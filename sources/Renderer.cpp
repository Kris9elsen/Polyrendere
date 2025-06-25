#include "Renderer.hpp"

Renderer::Renderer(int width, int height) :
    width(width), height(height) {
    
    // Resize zbuffer for screen size.
    zbuffer.resize(width * height, std::numeric_limits<float>::infinity());

    // Set view and projection to eye mat standard
    view = Mat4::identity();
    projection = Mat4::identity();
}

// Inits display window
void Renderer::init_x11() {
    display = XOpenDisplay(NULL); // Creates display
    int screen = DefaultScreen(display); // Sets default screen for display
    window = XCreateSimpleWindow(display, RootWindow(display, screen),
                                 0, 0, width, height, 1,
                                 BlackPixel(display, screen),
                                 WhitePixel(display, screen));

    XSelectInput(display, window, ExposureMask | KeyPressMask);
    XMapWindow(display, window);
    gc = DefaultGC(display, screen);

    framebuffer = new uint32_t[width * height]; // 1D array with all pixels

    ximage = XCreateImage(display, DefaultVisual(display, screen), 24,
                          ZPixmap, 0, (char*)framebuffer,
                          width, height, 32, 0);
}

// METHODS

// Project vertex from model space to screen space
Vec3 Renderer::project_vertex(const Vec3& vertex, const Mat4& model_matrix) const {
    Vec4 local(vertex.x, vertex.y, vertex.z, 1.0f);

    // Transform vector
    Vec4 world = model_matrix.transform(local); // From model to world
    Vec4 camera = view.transform(world); // From world to camera
    Vec4 clip = projection.transform(camera); // From camera to clip

    // Perspective devide
    if (clip.w == 0.0f) clip.w = 1e-5f; // Avoids division by zero
    Vec3 ndc = clip.homo();

    // Convert normalized device coords to  screen coords
    int x_screen = static_cast<int>((ndc.x + 1.0f) * 0.5f * width);
    int y_screen = static_cast<int>((1.0f - ndc.y) * 0.5f * height);

    return Vec3(float(x_screen), float(y_screen), ndc.z);
}

// Render wireframe
void Renderer::render_wireframe(const Renderable& obj) {
    const auto& verts = obj.get_vertices();
    const auto& inds = obj.get_indices();
    Mat4 model = obj.get_model_matrix();

    // Transform all vertices from model space to screen space
    std::vector<Vec3> projectedVerts;
    projectedVerts.reserve(verts.size());
    for (const auto& v : verts) {
        projectedVerts.push_back(project_vertex(v, model));
    }

    // Draw edges by connecting indices pairs
    for (size_t i = 0; i + 2 < inds.size(); i += 3) {
        Vec3 v0 = projectedVerts[inds[i]];
        Vec3 v1 = projectedVerts[inds[i+1]];
        Vec3 v2 = projectedVerts[inds[i+2]];

        draw_line(v0, v1, 0xFFFFFFFF);
        draw_line(v1, v2, 0xff00ffff);
        draw_line(v2, v0, 0xffff00ff);
    }

}

// Render multiple wireframes
void Renderer::render_wireframes() {
    for (auto* obj : objects) {
        render_wireframe(*obj);
    }
}

// Draws a line to the framebuffer between two points 
void Renderer::draw_line(Vec3 v0, Vec3 v1, uint32_t color) {
    int x0 = int(v0.x), y0 = int(v0.y);
    int x1 = int(v1.x), y1 = int(v1.y);

    float z0 = v0.z;
    float z1 = v1.z;

    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int steps = std::max(dx, dy);

    for (int i = 0; i <= steps; ++i) {
        float t = steps > 0 ? float(i) / steps : 0;
        int x = int(x0 + (x1 - x0) * t);
        int y = int(y0 + (y1 - y0) * t);
        float z = z0 + (z1 - z0) * t;
        put_pixel(x, y, z, color);
    }
}

// Set color of pixel with respect to depth
void Renderer::put_pixel(int x, int y, float z, uint32_t color) {
    if (x < 0 || x > width || y < 0 || y > height) return; // Check if pixel is on screen
    int index = y * width + x;

    if (z < zbuffer[index]) {
        zbuffer[index] = z;
        framebuffer[index] = color;
    }
}

// Set color of pixel with out respect to depth
void Renderer::put_pixel(int x, int y, uint32_t color) {
    if (x < 0 || x > width || y < 0 || y > height) return; // Check if pixel is on screen
    framebuffer[y * width + x] = color;
    
}

// Clears display with one color
void Renderer::clear(uint32_t color) {
    std::fill(framebuffer, framebuffer + (width * height), color);
    std::fill(zbuffer.begin(), zbuffer.end(), std::numeric_limits<float>::infinity());

}

// Render a rotating box
void Renderer::render_rotating_box(float angle) {
    int cx = width / 2;
    int cy = height / 2;
    int size = 100;

    // Get Sin and Cos of the angle
    float s = std::sin(angle);
    float c = std::cos(angle);

    // Sets the x and y coords for the pixel of the box
    for (int y = -size; y < size; ++y) {
        for (int x = -size; x < size; ++x) {
            int ry = int(c * x - s * y);
            int rx = int(s * x + c * y);
            put_pixel(cx + rx, cy + ry, 0xff00ffff); // Draws a pixel with color ARGB
        }
    }
}

void Renderer::show() {
    XPutImage(display, window, gc, ximage, 0, 0, 0, 0, width, height);
}

// Add object to the scene
void Renderer::add_object(Renderable* obj) {
    objects.push_back(obj);
}

// SETTERS

// Set camera position and direction
void Renderer::set_camera(const Vec3& eye, const Vec3& target, const Vec3& up) {
    view = Mat4::look_at(eye, target, up);
}

// Set projection
void Renderer::set_projection(float fov, float nearZ, float farZ) {
    float aspect = static_cast<float>(width) / height;
    projection = Mat4::perspective(fov, aspect, nearZ, farZ);
}

