#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.hh"

#include "color.hh"
#include "hittable.hh"

class camera {
  public:
    double aspect_ratio = 1.0;
    int image_width = 100;

    void render(const hittable& world) {
        initialize();

        // open output file stream
        std::ofstream out("image.ppm");

        // write ppm header
        out.write("P3\n", 3);
        out.write(std::to_string(image_width).c_str(), std::to_string(image_width).length());
        out.write(" ", 1);
        out.write(std::to_string(image_height).c_str(), std::to_string(image_height).length());
        out.write("\n", 1);
        out.write("255\n", 4);

        for (int j = 0; j < image_height; ++j) {
            std::clog << "\rScanlines remaining: " << image_height - j << std::flush;
            for (int i = 0; i < image_width; ++i) {

                // calculate trajectory of ray for current pixel and create ray
                auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v); // current pixel = first pixel + i times x pixel distance + j times y pixel distance
                auto ray_direction = pixel_center - camera_center; // vector from camera center to current pixel is direction for the ray
                ray r = ray(camera_center, ray_direction);

                color pixel_color = ray_color(r, world);
                write_color(&out, pixel_color); // pass output stream as reference to color function
            }
        }
        std::clog << "\nDone.\n";
        out.close();
    }

  private:
    int image_height;   // Rendered image height
    point3 camera_center;         // Camera center
    point3 pixel00_loc;    // Location of pixel 0, 0
    vec3 pixel_delta_u;  // Offset to pixel to the right
    vec3 pixel_delta_v;  // Offset to pixel below

    void initialize() {
        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = image_height >= 1 ? image_height : 1;

        // Camera/viewport settings
        camera_center = point3(0, 0, 0);
        auto focal_length = 1.0;
        auto viewport_height = 2.0;
        auto viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);

        // calculate vectors that "span" the viewport plane
        auto viewport_u = vec3(viewport_width, 0, 0);
        auto viewport_v = vec3(0, -viewport_height, 0);

        // calculate distances between pixels
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // calculate position from first pixel (upper left) and from viewport top left corner
        auto viewport_upper_left = camera_center - vec3(0, 0, focal_length) - (viewport_u + viewport_v)/2;
        pixel00_loc = viewport_upper_left + (pixel_delta_u + pixel_delta_v)/2;
    }

    color ray_color(const ray& r, const hittable& world) const {
        hit_record rec;

        if(world.hit(r, interval(0, infinity), rec)) { // check if ray hits any objects
            return 0.5 * (rec.normal + color(1,1,1)); // shades ray based on normal vector of surface hit
        }

        vec3 unit_direction = unit_vector(r.direction()); // normalize ray direction
        auto a = 0.5 * (unit_direction.y() + 1.0); // scale y component of ray direction to [0, 1] (creates a fade from blue to white)
        return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0); // 1,1,1 is start color and 0.5,0.7,1.0 is end color
    }
};

#endif