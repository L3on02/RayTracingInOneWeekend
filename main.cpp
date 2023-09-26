#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main() {
    int nx = 200;
    int ny = 100;
    std::cout << "P3\n" << nx << " " << ny << "\n255\n";

    int fd = open("./image.ppm", O_CREAT);
    


    for(int i = ny - 1; i >= 0; i--) {
        for (int j = 0; j < nx; j++) {
            float r = float(i) / float(nx);
            float g = float(j) / float(ny);
            float b = 0.2f;
            int ir = int(255.99 * r);
            int ig = int(255.99 * i);
            int ib = int(255.99 * b);
            std::cout << ir << " " << ig << " " << ib << "\n";
        }
    }
}