#ifndef CAMERA_CUH
#define CAMERA_CUH

#include <curand_kernel.h>
#include "ray.cuh"

class camera
{
public:
    __device__ camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect, float defocus_angle, float focus_dist) // vfov is top to bottom in degrees
    {
        float theta = vfov * ((float)M_PI) / 180.0f;
        float half_height = tan(theta / 2.0f);
        float half_width = aspect * half_height;
        origin = lookfrom;
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);
        lower_left_corner = origin - half_width * focus_dist * u - half_height * focus_dist * v - focus_dist * w;
        horizontal = 2.0f * half_width * focus_dist * u;
        vertical = 2.0f * half_height * focus_dist * v;

        float defocus_radius = focus_dist * tan(defocus_angle * (float)M_PI / 360.0f);
        defocus_disk_u = defocus_radius * u;
        defocus_disk_v = defocus_radius * v;
    }

    __device__ ray get_ray(float s, float t, curandState *local_rand_state)
    {
        vec3 rand_in_unit = random_in_unit_disk(local_rand_state);
        vec3 offset = rand_in_unit[0] * defocus_disk_u + rand_in_unit[1] * defocus_disk_v;
        
        vec3 ray_origin = origin + offset;
        vec3 ray_direction =  lower_left_corner + s * horizontal + t * vertical - ray_origin;
        
        return ray(ray_origin, ray_direction);
    }

    __device__ vec3 random_in_unit_disk(curandState *local_rand_state)
    {
        vec3 p;
        do
        {
            p = 2.0f * vec3(curand_uniform(local_rand_state), curand_uniform(local_rand_state), 0) - vec3(1, 1, 0);
        } while (dot(p, p) >= 1.0f);
        return p;
    }

    vec3 origin;
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    vec3 u, v, w;
    vec3 defocus_disk_u, defocus_disk_v;
};

#endif