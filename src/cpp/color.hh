#ifndef COLOR_HH
#define COLOR_HH

#include "vec3.hh"

#include <fstream>
#include <vector>

using color = vec3;

inline double linear_to_gamma(double linear_component)
{
    return sqrt(linear_component);
}

void write_color(color *image, int image_width, int image_height, int samples_per_pixel)
{
    std::ofstream out("out.ppm");
    // write ppm header
    out.write("P3\n", 3);
    out.write(std::to_string(image_width).c_str(), std::to_string(image_width).length());
    out.write(" ", 1);
    out.write(std::to_string(image_height).c_str(), std::to_string(image_height).length());
    out.write("\n", 1);
    out.write("255\n", 4);

    for (int i = 0; i < image_width * image_height; i++)
    {
        color pixel_color = image[i];
        auto r = pixel_color.x();
        auto g = pixel_color.y();
        auto b = pixel_color.z();

        // Divide the color by the number of samples.
        auto scale = 1.0 / samples_per_pixel;
        r *= scale;
        g *= scale;
        b *= scale;

        // transform to gamma values
        r = linear_to_gamma(r);
        g = linear_to_gamma(g);
        b = linear_to_gamma(b);

        // Write the translated [0,255] value of each color component.
        static const interval intensity(0.000, 0.999);
        out << static_cast<int>(256 * intensity.clamp(r)) << ' '
            << static_cast<int>(256 * intensity.clamp(g)) << ' '
            << static_cast<int>(256 * intensity.clamp(b)) << '\n';
    }
    out.close();
}
#endif