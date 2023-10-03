#ifndef COLOR_HH
#define COLOR_HH

#include "vec3.hh"

#include <fstream>

using color = vec3;

void write_color(std::ofstream out, color pixel_color) {
    out << static_cast<int>(255.999 * pixel_color.x()) << ' '
        << static_cast<int>(255.999 * pixel_color.y()) << ' '
        << static_cast<int>(255.999 * pixel_color.z()) << '\n';
}
#endif