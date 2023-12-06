#ifndef GPU_RENDER_CUH
#define GPU_RENDER_CUH

#include "./point.hh"

void gpu_render(int ny, float aspect_ratio, int ns, int max_depth, point camera_pos, point focal_point, float vfov, float aperture, double &last_render_time);

#endif