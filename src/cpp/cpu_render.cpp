#include "cpu_render.hh"
#include "rtweekend.hh"

#include "camera.hh"
#include "color.hh"
#include "hittable_list.hh"
#include "sphere.hh"
#include "material.hh"

void cpu_render(int _image_height, double _aspect_ratio, int _samples_per_pixel, int _max_depth,
                point t_cam_pos, point t_focal_point, double _vfov, double _defocus_angle, double &last_render_time)
{

    point3 _cam_pos(t_cam_pos.x, t_cam_pos.y, t_cam_pos.z);
    point3 _focal_point(t_focal_point.x, t_focal_point.y, t_focal_point.z);

    camera cam;
    cam.aspect_ratio = _aspect_ratio;
    cam.image_height = _image_height;
    cam.samples_per_pixel = _samples_per_pixel;
    cam.max_depth = _max_depth;
    cam.vfov = _vfov;
    cam.lookfrom = _cam_pos;
    cam.lookat = _focal_point;
    cam.defocus_angle = _defocus_angle;

    cam.vup = vec3(0, 1, 0);
    cam.focus_dist = (_cam_pos - _focal_point).length();

    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a++)
    {
        for (int b = -11; b < 11; b++)
        {
            auto choose_mat = random_double();
            point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9)
            {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8)
                {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95)
                {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
                else
                {
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

    cam.render(world, last_render_time);
}