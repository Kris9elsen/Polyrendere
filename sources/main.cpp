#include "Renderer.hpp"
#include "Cube.hpp"
#include "Sphere.hpp"
#include "Render_math.hpp"

#include <unistd.h>

const int WIDTH = 640;
const int HEIGHT = 480;

int main() {
    Renderer renderer(640, 480);
    renderer.init_x11();

    renderer.set_camera(Vec3(0, 0, 5), Vec3(0, 0, 0), Vec3(0, 1, 0));
    renderer.set_projection(3.14159f / 3.0f, 0.1f, 100.0f);

    Cube cube;
    cube.set_scale({1.0f, 1.0f, 1.0f});
    Sphere sphere(3.0f, 16, 16);

    renderer.add_object(&cube);
    renderer.add_object(&sphere);
    
    float angle = 0;

    while(true) {
        cube.set_rotation(Vec3(-angle, -angle, 0));
        sphere.set_rotation(Vec3(0, -angle, 0));
        renderer.render_wireframes();
        renderer.show();
        usleep(16000);
        renderer.clear(0xff000000);
        angle += 0.005f;
        
    }

    return 0;
}

