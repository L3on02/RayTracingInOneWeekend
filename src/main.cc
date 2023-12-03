#define GLEW_STATIC

#include <GL/glew.h> // has to be included first!
#include <GLFW/glfw3.h>

#include "rtweekend.hh"

#include "camera.hh"
#include "color.hh"
#include "hittable_list.hh"
#include "sphere.hh"
#include "material.hh"

auto create_window(int image_width, int aspect_ratio) {
    int image_height = static_cast<int>(image_width / aspect_ratio);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 0);
    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_RESIZABLE, true);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

// Anti Aliasing - Multisampling
    glfwWindowHint(GLFW_SAMPLES, 4);
    glEnable(GL_MULTISAMPLE);

    return glfwCreateWindow(image_width, image_height, "ParallelSystems", nullptr, nullptr);
}

void handleEvents(GLFWwindow* window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void frameBufferSizeCallback(GLFWwindow* window, int width, int height) {
    glfwMakeContextCurrent(window);
    glViewport(0, 0, width, height);
}

int main() {
    // World
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    } 

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 1920;
    cam.samples_per_pixel = 10;
    cam.max_depth = 50;

    cam.vfov = 20;
    cam.lookfrom = point3(13,2,3);
    cam.lookat = point3(0,0,0);
    cam.vup = vec3(0,1,0);

    cam.defocus_angle = 0.6;
    cam.focus_dist = 10.0;

    auto window = create_window(1920, 16.0/9.0);
    if (!window)
    {
        printf("Creation of window failed!");
        glfwTerminate();
        return 1;
    }

    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        printf("Init of glew failed! %s\n", glewGetErrorString(err));
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);

    glClearColor(0.0f, 0.1f, 0.2f, 1.0f);

    while (!glfwWindowShouldClose(window)) {
        handleEvents(window);
        glfwMakeContextCurrent(window);
        //render()
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //cam.render(world);
    return 0;
}