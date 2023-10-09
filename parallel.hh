#ifndef MULTITHREAD_HH
#define MULTITHREAD_HH

#include "vec3.hh"

#include <thread>
#include <vector>

#include <mutex>

using color = vec3;

class image_memory {
    public:
        image_memory(int render_width, int render_height) : lines(render_height), rows(render_width) {
            linesLeft = lines;
            shared_storage = new color[lines * rows];
            image = new color[lines * rows];
            iterations = 0;
        }

        void write_pixel(int line, int row, color pixel_color) {
            std::lock_guard<std::mutex> guard(lock_img);
            shared_storage[(line - 1) * rows + row] = pixel_color;
        }

        int get_render_line() {
            std::lock_guard<std::mutex> guard(lock_line);
            return (lines - (--linesLeft));
        }

        void copy_image() {
            double factor = 1.0 / (iterations + 1);
            std::lock_guard<std::mutex> guard(lock_img);
            for(int i = 0; i < lines * rows; i++)
                image[i] = (image[i] * (1 - factor) + shared_storage[i] * factor);
        }

        color* get_image() {
            copy_image();
            iterations++;
            linesLeft = lines;
            return image;
        }

    private:
        int lines;
        int rows;
        std::mutex lock_img;
        color* shared_storage;
        color* image;

        int linesLeft;
        int iterations;
        std::mutex lock_line;
};

#endif