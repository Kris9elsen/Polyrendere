#include "Renderer.hpp"

Renderer::Renderer(int width, int height) :
    width(width), height(height) {

    // Set view and projection to eye mat standard
    view = Mat4::identity();
    projection = Mat4::identity();
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

        // ADD DRAW LINES TO FRAME BUFFER
        // FOR NOW PRINT REMOVE LATER
        std::cout << "Line from (" << v0.x << "," << v0.y << ") to (" << v1.x << "," << v1.y << ")\n";
        std::cout << "Line from (" << v1.x << "," << v1.y << ") to (" << v2.x << "," << v2.y << ")\n";
        std::cout << "Line from (" << v2.x << "," << v2.y << ") to (" << v0.x << "," << v0.y << ")\n";
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