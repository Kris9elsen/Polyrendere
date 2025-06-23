#include <X11/Xlib.h>
#include <cstring>
#include <cmath>
#include <cstdint>
#include <unistd.h>

const int WIDTH = 640;
const int HEIGHT = 480;

Display* display;
Window window;
GC gc;
XImage* ximage;
uint32_t* framebuffer;

void init_x11() {
    display = XOpenDisplay(NULL); // Creates display
    int screen = DefaultScreen(display); // Sets default screen for display
    window = XCreateSimpleWindow(display, RootWindow(display, screen),
                                 0, 0, WIDTH, HEIGHT, 1,
                                 BlackPixel(display, screen),
                                 WhitePixel(display, screen));

    XSelectInput(display, window, ExposureMask | KeyPressMask);
    XMapWindow(display, window);
    gc = DefaultGC(display, screen);

    framebuffer = new uint32_t[WIDTH * HEIGHT]; // 1D array with all pixels

    ximage = XCreateImage(display, DefaultVisual(display, screen), 24,
                          ZPixmap, 0, (char*)framebuffer,
                          WIDTH, HEIGHT, 32, 0);
}

// Set color of pixel
void put_pixel(int x, int y, uint32_t color) {
    if (x < 0 || x > WIDTH || y < 0 || y > HEIGHT) return;
    framebuffer[y * WIDTH + x] = color;
}

// Clears display with one color
void clear(uint32_t color) {
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        framebuffer[i] = color;
    }
}

// Render a rotating box
void render_rotating_box(float angle) {
    int cx = WIDTH / 2;
    int cy = HEIGHT / 2;
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

int main() {
    init_x11();

    float angle = 0.0f;

    while(true) {
        angle += 0.02f;
        clear(0xff000000);
        render_rotating_box(angle);

        XPutImage(display, window, gc, ximage, 0, 0, 0, 0, WIDTH, HEIGHT);
        usleep(16000*60);
        
    }

    return 0;
}

