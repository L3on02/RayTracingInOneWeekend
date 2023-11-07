#include "rtweekend.hh"

#include "camera.hh"
#include "color.hh"
#include "hittable_list.hh"
#include "sphere.hh"
#include "material.hh"

void randomSpheres(camera cam) {
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

    cam.render(world);
}

void cornellBox(camera &cam) {
    hittable_list world;

    auto red   = make_shared<lambertian>(color(0.65, 0.05, 0.05));
    auto white = make_shared<lambertian>(color(0.73, 0.73, 0.73));
    auto green = make_shared<lambertian>(color(0.12, 0.45, 0.15));
    //auto light = make_shared<diffuse_light>(color(15, 15, 15));

    world.add(make_shared<sphere>(point3(0,-10000,0), 9990, white));
    world.add(make_shared<sphere>(point3(0, 10000,0), 9990, white));
    world.add(make_shared<sphere>(point3(0, 0,-10000), 9990, red));
    world.add(make_shared<sphere>(point3(0, 0, 10000), 9990, green));
   /* world.add(make_shared<sphere>(point3(1000,0,0), 995, red));
    world.add(make_shared<sphere>(point3(-1000,0,0),995, green));
    world.add(make_shared<sphere>(point3(0,1000,0), 995, white));
    world.add(make_shared<sphere>(point3(0,-1000,0),995, white)); */
    //world.add(make_shared<sphere>(point3(0,0,0),     50,  light));

    cam.render(world);
}

void camInit(camera &cam) {
    cam.aspect_ratio      = 1.0 / 1.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 10;
    cam.max_depth         = 10;

    cam.vfov     = 90;
    cam.lookfrom = point3(-1,0,0);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0.0;
    cam.focus_dist    = 10.0;
}

int main() {
    camera cam;
    camInit(cam);
    switch(2) {
        case 1:
            randomSpheres(cam);
            break;
        case 2:
            cornellBox(cam);
    }
    return 0;
}