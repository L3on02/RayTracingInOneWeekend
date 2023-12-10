#ifndef CPU_RENDER_HH
#define CPU_RENDER_HH

#include "./point.hh"

void cpu_render(int _image_height, double _aspect_ratio, int _samples_per_pixel, int _max_depth, point t_cam_pos, point t_focal_point, double _vfov, double _defocus_angle, int cpu_count, double &last_render_time);

#endif