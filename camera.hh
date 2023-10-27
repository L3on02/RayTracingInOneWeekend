#ifndef CAMERA_HH
#define CAMERA_HH

#include "rtweekend.hh"

#include "color.hh"
#include "hittable.hh"
#include "material.hh"
#include "parallel.hh"

#include <chrono>
#include <iomanip>

class camera {
  public:
    double aspect_ratio = 1.0; // Ratio of image width over height
    int image_width = 100; // Rendered image width in pixel count
    int max_iterations = 10; // Count of rendering iterations done
    int samples_per_pixel = 10; // Count of random samples for each pixel
    int max_depth = 10; // Maximum bounces that are calculated for each ray

    double vfov = 90; // vertical view angle (field of view)
    point3 lookfrom = point3(0, 0, 1); // where camera is looking "from"
    point3 lookat = point3(0, 0, 0); //  where camera is looking at
    vec3 vup = vec3(0, 1, 0); // Camaera-relative "up" direction

    double defocus_angle = 0; // Variation in angle of rays through each pixel
    double focus_dist = 10; // Distance from Camera "Sensor" to plane of perfect focus (focal point)

    void render(const hittable& world) {
        std::clog << "Starting render ...\n";
        // start timer
        auto start = std::chrono::high_resolution_clock::now();

        // initialize camera and viewport settings
        initialize();

        // create shared memory object with task queue
        image_memory image(image_width, image_height);

        // open threads and start rendering
        std::thread threads[processor_count];
        for(int i = 0; i < max_iterations; i++) {
            for(int i = 0; i < processor_count; i++) {
                threads[i] = std::thread(&camera::render_thread, this, std::ref(world), std::ref(image));
            }
            // wait for all threads to finish
            for(int i = 0; i < processor_count; i++) {
                threads[i].join();
            }
            std::clog << "\rIteration " << i + 1 << " of " << max_iterations << " done." << std::flush;

            // store image to file
            write_color(image.get_image(), image_width, image_height, samples_per_pixel);
        }

        // stop timer
        auto stop = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = stop - start;

        std::clog << "\nDone in " << std::fixed << std::setprecision(2) << elapsed.count() << " seconds.\n";
    }

  private:
    int image_height;   // Rendered image height
    int processor_count; 
    point3 camera_center;         // Camera center
    point3 pixel00_loc;    // Location of pixel 0, 0
    vec3 pixel_delta_u;  // Offset to pixel to the right
    vec3 pixel_delta_v;  // Offset to pixel below
    vec3 u, v, w;   // Camera frame basis vectors
    vec3 defocus_disk_u;  // Defocus disk horizontal radius
    vec3 defocus_disk_v;  // Defocus disk vertical radius


    void initialize() {
        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = image_height >= 1 ? image_height : 1;

        // Read number of available processors
        processor_count = std::thread::hardware_concurrency();
        std::clog << "Using " << processor_count << " threads.\n";

        // Camera/viewport settings
        camera_center = lookfrom;

        auto theta = degrees_to_radians(vfov);
        auto h = tan(theta/2);
        auto viewport_height = 2.0 * h * focus_dist;
        auto viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);

        // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        // calculate vectors that "span" the viewport plane
        auto viewport_u = viewport_width * u; // vector along horizontal edge
        auto viewport_v = viewport_height * -v; // vector down vertical edge

        // calculate distances between pixels
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // calculate position from first pixel (upper left) and from viewport top left corner
        auto viewport_upper_left = camera_center - (focus_dist * w) - (viewport_u + viewport_v)/2;
        pixel00_loc = viewport_upper_left + (pixel_delta_u + pixel_delta_v)/2;

        // calculate defocus disk  basis vectors
        auto defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = defocus_radius * u;
        defocus_disk_v = defocus_radius * v;
    }
    
    // Renders the image as long as there are lines left to render
    void render_thread(const hittable& world, image_memory& image) {
        int j;
        while((j = image.get_render_line()) <= image_height) {

            for (int i = 0; i < image_width; ++i) {
                color pixel_color = color(0, 0, 0);
                for(int sample = 0; sample < samples_per_pixel; sample++) {
                    ray r = get_ray(i, j); // get a slightly randomized ray for the current pixel
                    pixel_color += ray_color(r, max_depth, world); // calculate color for the current pixel
                }
                image.write_pixel(j, i, pixel_color); // write the color to the image
            }
        }
    }

    ray get_ray(int i, int j) const {
        // Get a randomly sampled camera ray for the pixel at location i,j originating from a random point on the defocus disk.
        auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
        auto pixel_sample = pixel_center + pixel_sample_square();

        auto ray_origin = (defocus_angle <= 0) ? camera_center : defocus_disc_sample();
        auto ray_direction = pixel_sample - ray_origin;

        return ray(ray_origin, ray_direction);
    }

    point3 defocus_disc_sample() const {
        // returns a random point on the defocus disk
        auto p = random_in_unit_disk();
        return camera_center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

    vec3 pixel_sample_square() const {
        // returns a random point in the surrounding square
        auto px = -0.5 + random_double();
        auto py = -0.5 + random_double();
        return (px * pixel_delta_u) + (py * pixel_delta_v);
    }

    color ray_color(const ray& r, int depth, const hittable& world) const {
        hit_record rec;

        // If we've exceeded the ray bounce limit, no more light is gathered.
        if (depth <= 0)
            return color(0,0,0);

        if(world.hit(r, interval(0.001, infinity), rec)) { // check if ray hits any objects
            ray scattered;
            color attenuation;
            if(rec.mat->scatter(r, rec, attenuation, scattered))
                return attenuation * ray_color(scattered, depth-1, world);
            return color(0, 0, 0);
        }

        vec3 unit_direction = unit_vector(r.direction()); // normalize ray direction
        auto a = 0.5 * (unit_direction.y() + 1.0); // scale y component of ray direction to [0, 1] (creates a fade from blue to white)
        return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0); // 1,1,1 is start color and 0.5,0.7,1.0 is end color
    }
};

#endif