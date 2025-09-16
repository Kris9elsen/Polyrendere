#include "Renderer.hpp"

Renderer::Renderer(int width, int height) :
    width(width), height(height) {

    size = width * height; // Store framebuffer size for fast access
    
    // Resize zbuffer for screen size.
    zbuffer.resize(size, std::numeric_limits<float>::infinity());

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

    framebuffer = new uint32_t[size]; // 1D array with all pixels

    ximage = XCreateImage(display, DefaultVisual(display, screen), 24,
                          ZPixmap, 0, (char*)framebuffer,
                          width, height, 32, 0);
}

// Enable SSAA and set factor
void Renderer::enable_ssaa(int factor) {
    // Check if we are enabeling or disabeling SSAA
    if (factor <= 1) {
        ssaa = false;
        zbuffer.resize(size, std::numeric_limits<float>::infinity());
        return;
    }

    ssaa = true;
    ssaa_factor = factor;
    ssaa_width = width * ssaa_factor;
    ssaa_height = height * ssaa_factor;
    ssaa_samples = ssaa_factor * ssaa_factor;
    ssaa_size = ssaa_height * ssaa_width;
    ssaa_buffer = new uint32_t[ssaa_size];
    zbuffer.resize(ssaa_size, std::numeric_limits<float>::infinity());

    return;
}

// Disable SSAA
void Renderer::disable_ssaa() {
    enable_ssaa(0);
    return;
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
    ndc.x = static_cast<int>((ndc.x + 1.0f) * 0.5f * (ssaa ? ssaa_width : width));
    ndc.y = static_cast<int>((1.0f - ndc.y) * 0.5f * (ssaa ? ssaa_height : height));
    ndc.z = (ndc.z + 1.0f) * 0.5f;

    return ndc;
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

    int screen_width = ssaa ? ssaa_width : width;
    int screen_height = ssaa ? ssaa_height : height;

    // Draw edges by connecting indices pairs
    for (size_t i = 0; i + 2 < inds.size(); i += 3) {
        Vec4 v0 = model_to_clip(verts[inds[i]], model);
        Vec4 v1 = model_to_clip(verts[inds[i + 1]], model);
        Vec4 v2 = model_to_clip(verts[inds[i + 2]], model);

        auto clipped = clip_triangle({v0, v1, v2});
        for (auto& triangle : clipped) {
            // Perform perspective division
            Vec3 p0 = triangle[0].homo();
            Vec3 p1 = triangle[1].homo();
            Vec3 p2 = triangle[2].homo();  

            // Check for degenerate triangle (area close to zero)
            Vec3 edge1 = p1 - p0;
            Vec3 edge2 = p2 - p0;
            float area = edge1.x * edge2.y - edge1.y * edge2.x;
            if(fabs(area) < 1e-6f) {
                continue;
            }

            // Convert to screen space
            auto to_screen = [screen_width, screen_height](const Vec3& ndc) {
                return Vec3(
                    (ndc.x + 1.0f) * 0.5f * screen_width,
                    (1.0f - ndc.y) * 0.5f * screen_height,
                    ndc.z
                );
            };

            Vec3 s0 = to_screen(p0);
            Vec3 s1 = to_screen(p1);
            Vec3 s2 = to_screen(p2);

            draw_line(s0, s1, 0xFFFFFFFF);
            draw_line(s1, s2, 0xFF00FFFF);
            draw_line(s2, s0, 0xFFFF00FF);
        }
    }

}

// Render multiple wireframes
void Renderer::render_wireframes() {
    for (auto* obj : objects) {
        render_wireframe(*obj);
    }
}

void Renderer::render_filled(const Renderable& obj) {
    
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
        int x = clamp(int(std::round(x0 + (x1 - x0) * t)), 0, ssaa ? ssaa_width - 1 : width - 1);
        int y = clamp(int(std::round(y0 + (y1 - y0) * t)), 0, ssaa ? ssaa_height - 1 : height - 1);
        float z = z0 + (z1 - z0) * t;
        put_pixel(x, y, z, color);
    }
}

// Set color of pixel with respect to depth
void Renderer::put_pixel(int x, int y, float z, uint32_t color) {
    z = clamp(z, 0.0f, 1.0f);
    if (ssaa) {
        if (x < 0 || x >= ssaa_width || y < 0 || y >= ssaa_height) return;
        int index = y * ssaa_width + x;

        if (z < zbuffer[index]) {
            zbuffer[index] = z;
            ssaa_buffer[index] = color;
        }

    } else {
        if (x < 0 || x >= width || y < 0 || y >= height) return;
        int index = y * width + x;

        if (z < zbuffer[index]) {
            zbuffer[index] = z;
            framebuffer[index] = color;
        }
    }
}

// Set color of pixel with out respect to depth
void Renderer::put_pixel(int x, int y, uint32_t color) {
    if (x < 0 || x >= width || y < 0 || y >= height) return; // Check if pixel is on screen
    framebuffer[y * width + x] = color;
    
}

// Clears display with one color
void Renderer::clear(uint32_t color) {
    if (ssaa) std::fill(ssaa_buffer, ssaa_buffer + (ssaa_size), color);
    else std::fill(framebuffer, framebuffer + (size), color);
    
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

Vec4 Renderer::model_to_clip(const Vec3& vertex, const Mat4& model_matrix) const {
    Vec4 local(vertex.x, vertex.y, vertex.z, 1.0f);
    return projection.transform(view.transform(model_matrix.transform(local)));
}

// Display framebuffer on screen
void Renderer::show() {
    if (ssaa) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                uint64_t a = 0, r = 0, g = 0, b = 0;
                for (int dy = 0; dy < ssaa_factor; ++dy) {
                    for (int dx = 0; dx < ssaa_factor; ++dx) {
                        uint32_t color = ssaa_buffer[(y * ssaa_factor + dy) * ssaa_width + (x * ssaa_factor + dx)];
                        a += (color >> 24) & 0xFF;
                        r += (color >> 16) & 0xFF;
                        g += (color >> 8)  & 0xFF;
                        b += color & 0xFF;
                    }
                }

                uint32_t color = 
                    ((a / ssaa_samples) << 24) |
                    ((r / ssaa_samples) << 16) |
                    ((g / ssaa_samples) << 8)  |
                    (b / ssaa_samples);
                framebuffer[y * width + x] = color;
            }
        }
    }
    
    XPutImage(display, window, gc, ximage, 0, 0, 0, 0, width, height);
}

// Add object to the scene
void Renderer::add_object(Renderable* obj) {
    objects.push_back(obj);
}

// Clamps screen coordinates
inline int Renderer::clamp(int value, int min, int max) {
    return std::max(min, std::min(value, max));
}

// Helper to comute barycentric coordinates
inline bool Renderer::barycentric(const Vec3& p, const Vec3& a, const Vec3& b, const Vec3& c, float& u, float& v, float& w) {
    Vec3 v0 = b - a;
    Vec3 v1 = c - a;
    Vec3 v2 = p - a;

    float d00 = v0.x * v0.x + v0.y * v0.y;
    float d01 = v0.x * v1.x + v0.y * v1.y;
    float d11 = v1.x * v1.x + v1.y * v1.y;
    float d20 = v2.x * v0.x + v2.y * v0.y;
    float d21 = v2.x * v1.x + v2.y * v1.y;

    float denom = d00 * d11 - d01 * d01;
    if (fabs(denom) < 1e-8f) return false; // degenerate

    v = (d11 * d20 - d01 * d21) / denom;
    w = (d00 * d21 - d01 * d20) / denom;
    u = 1.0f - v - w;

    return (u >= 0.0f && v >= 0.0f && w >= 0.0f);
}

// Rasterize a single trinagle
void Renderer::draw_triangle(const Vec3& v0, const Vec3& v1, const Vec3& v2, uint32_t color) {
    int minX = clamp((int)std::floor(std::min({v0.x, v1.x, v2.x})), 0, ssaa ? ssaa_width - 1 : width - 1);
    int maxX = clamp((int)std::ceil(std::max({v0.x, v1.x, v2.x})), 0, ssaa ? ssaa_width - 1 : width -1 );
    int minY = clamp((int)std::floor(std::min({v0.y, v1.y, v2.y})), 0, ssaa ? ssaa_height - 1 : height - 1);
    int maxY = clamp((int)std::ceil(std::max({v0.y, v1.y, v2.y})), 0, ssaa ? ssaa_height - 1 : height - 1);

    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <=maxX; ++x) {
            Vec3 p((float)x + 0.5f, (float)y + 0.5f, 0.0f);
            float u, v, w;
            if (barycentric(p, v0, v1, v2, u, v, w)) {
                // Interpolate depth
                float z = u * v0.z + v * v1.z + w * v2.z;

                // Write pixel with depth test
                put_pixel(x, y, z, color);
            }
        }
    }
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

// CLIP PLANE
// Check if inside clip plane
bool Renderer::inside(const Vec4& v, Renderer::Clip_plane plane) {
    switch (plane) {
        case Clip_plane::Left:   return v.x >= -v.w;
        case Clip_plane::Right:  return v.x <= v.w;
        case Clip_plane::Bottom: return v.y >= -v.w;
        case Clip_plane::Top:    return v.y <= v.w;
        case Clip_plane::Near:   return v.z >= -v.w;
        case Clip_plane::Far:    return v.z <= v.w;
    }
    return false;

}

// Interpolate clipping
Vec4 Renderer::interpolate(const Vec4& a, const Vec4& b, Renderer::Clip_plane plane) {
    float t_num = 0.0f;
    float t_den = 1.0f;

    switch (plane) {
        case Clip_plane::Left:
            t_num = a.x + a.w;
            t_den = (a.x + a.w) - (b.x + b.w);
            break;
        case Clip_plane::Right:
            t_num = a.x - a.w;
            t_den = (a.x - a.w) - (b.x - b.w);
            break;
        case Clip_plane::Bottom:
            t_num = a.y + a.w;
            t_den = (a.y + a.w) - (b.y + b.w);
            break;
        case Clip_plane::Top:
            t_num = a.y - a.w;
            t_den = (a.y - a.w) - (b.y - b.w);
            break;
        case Clip_plane::Near:
            t_num = a.z + a.w;
            t_den = (a.z + a.w) - (b.z + b.w);
            break;
        case Clip_plane::Far:
            t_num = a.z - a.w;
            t_den = (a.z - a.w) - (b.z - b.w);
            break;
    }

    // Prevent division by near zero
    if (fabs(t_den) < 1e-6f) return a;

    float t = t_num / t_den;
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    return a + (b - a) * t;
}

// Clip polygon against a single plane
std::vector<Vec4> Renderer::clip_poly(const std::vector<Vec4>& vertices, Renderer::Clip_plane plane) {
    std::vector<Vec4> output;

    for (size_t i = 0; i < vertices.size(); ++i) {
        size_t j = (i + 1) % vertices.size();
        bool inside_i = inside(vertices[i], plane);
        bool inside_j = inside(vertices[j], plane);

        if (inside_i && inside_j) output.push_back(vertices[j]);
        else if (inside_i && !inside_j) {
            Vec4 clipped_point = interpolate(vertices[i], vertices[j], plane);
            output.push_back(clipped_point);
        } else if (!inside_i && inside_j) {
            Vec4 clipped_point = interpolate(vertices[i], vertices[j], plane);
            output.push_back(clipped_point);
            output.push_back(vertices[j]);
        }

    }
    
    return output;

}

std::vector<std::array<Vec4, 3>> Renderer::clip_triangle(const std::array<Vec4, 3>& triangle) {
    std::vector<Vec4> vertices = {triangle[0], triangle[1], triangle[2]};

    // Clip against all 6 planes sequentially
    vertices = clip_poly(vertices, Clip_plane::Left);
    if (vertices.empty()) return {};
    vertices = clip_poly(vertices, Clip_plane::Right);
    if (vertices.empty()) return {};
    vertices = clip_poly(vertices, Clip_plane::Bottom);
    if (vertices.empty()) return {};
    vertices = clip_poly(vertices, Clip_plane::Top);
    if (vertices.empty()) return {};
    vertices = clip_poly(vertices, Clip_plane::Near);
    if (vertices.empty()) return {};
    vertices = clip_poly(vertices, Clip_plane::Far);
    if (vertices.empty()) return {};

    // Triangulate resulting polygon (fan triangulation)
    std::vector<std::array<Vec4, 3>> output;
    for (size_t i = 1; i + 1 < vertices.size(); ++i) {
        output.push_back({vertices[0], vertices[i], vertices[i + 1]});
    }

    return output;

}