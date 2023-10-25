#ifndef COLOR_CUH
#define COLOR_CUH

#include "vec3.cuh"

#include <fstream>


inline float linear_to_gamma(float linear_component)
{
    return sqrt(linear_component);
}

float clamp(double x)
{
    if (x < 0.0)
        return 0.0;
    if (x > 0.999)
        return 0.999;
    return x;
}

void write_color(vec3 *image, int image_width, int image_height)
{
    std::ofstream out("out.ppm");
    // write ppm header
    out.write("P3\n", 3);
    out.write(std::to_string(image_width).c_str(), std::to_string(image_width).length());
    out.write(" ", 1);
    out.write(std::to_string(image_height).c_str(), std::to_string(image_height).length());
    out.write("\n", 1);
    out.write("255\n", 4);

    for (int j = image_height - 1; j >= 0; j--)
    {
        for (int i = 0; i < image_width; i++)
        {
            size_t pixel_index = j * image_width + i;
            
            vec3 pixel_color = image[pixel_index];
            auto r = linear_to_gamma(pixel_color.x());
            auto g = linear_to_gamma(pixel_color.y());
            auto b = linear_to_gamma(pixel_color.z());
            
            // Write the translated [0,255] value of each color component.
            out << static_cast<int>(256 * clamp(r)) << ' '
                << static_cast<int>(256 * clamp(g)) << ' '
                << static_cast<int>(256 * clamp(b)) << '\n';
        }
    }
    out.close();
}
#endif