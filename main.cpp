#include "color.hh"
#include "vec3.hh"

int main() {
    int nx = 256;
    int ny = 256;
    std::ofstream out("image.ppm");

    out.write("P3\n", 3);
    out.write(std::to_string(nx).c_str(), std::to_string(nx).length());
    out.write(" ", 1);
    out.write(std::to_string(ny).c_str(), std::to_string(ny).length());
    out.write("\n", 1);
    out.write("255\n", 4);

    for (int j = 0; j < ny; ++j) {
        std::clog << "\rScanlines remaining: " << ny - j << std::flush;
        for (int i = 0; i < nx; ++i) {
            color pixel_color(double(i) / (nx-1), double(j) / (ny-1), 0.25);
            out << static_cast<int>(255.999 * pixel_color.x()) << ' '
                << static_cast<int>(255.999 * pixel_color.y()) << ' '
                << static_cast<int>(255.999 * pixel_color.z()) << '\n';
        }
    }
    std::clog << "\nDone.\n";
    out.close();
}