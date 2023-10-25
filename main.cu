#include <iostream>
#include <time.h>
#include <float.h>
#include <curand_kernel.h>

#include "vec3.cuh"
#include "ray.cuh"
#include "sphere.cuh"
#include "hittable_list.cuh"
#include "camera.cuh"
#include "material.cuh"

#include "color.cuh"

// limited version of checkCudaErrors from helper_cuda.h in CUDA examples
#define checkCudaErrors(val) check_cuda((val), #val, __FILE__, __LINE__)

void check_cuda(cudaError_t result, char const *const func, const char *const file, int const line)
{
    if (result)
    {
        std::cerr << "CUDA error = " << static_cast<unsigned int>(result) << " at " << file << ":" << line << " '" << func << "' \n";
        // Make sure we call CUDA Device Reset before exiting
        cudaDeviceReset();
        exit(99);
    }
}

__device__ vec3 color(const ray &r, hittable **world, curandState *local_rand_state)
{
    ray cur_ray = r;
    vec3 cur_attenuation = vec3(1.0, 1.0, 1.0);
    for (int i = 0; i < 50; i++)
    {
        hit_record rec;
        if ((*world)->hit(cur_ray, 0.001f, FLT_MAX, rec))
        {
            ray scattered;
            vec3 attenuation;
            if (rec.mat_ptr->scatter(cur_ray, rec, attenuation, scattered, local_rand_state))
            {
                cur_attenuation *= attenuation;
                cur_ray = scattered;
            }
            else
            {
                return vec3(0.0, 0.0, 0.0);
            }
        }
        else
        {
            vec3 unit_direction = unit_vector(cur_ray.direction());
            float t = 0.5f * (unit_direction.y() + 1.0f);
            vec3 c = (1.0f - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
            return cur_attenuation * c;
        }
    }
    return vec3(0.0, 0.0, 0.0);
}

__global__ void render(vec3 *fb, int max_x, int max_y, int ns, camera **cam, hittable **world, curandState *rand_state)
{
    int i = threadIdx.x + blockIdx.x * blockDim.x;
    int j = threadIdx.y + blockIdx.y * blockDim.y;
    if ((i >= max_x) || (j >= max_y))
        return;
    int pixel_index = j * max_x + i;

    curandState local_rand_state = rand_state[pixel_index];
    vec3 col(0, 0, 0);

    for (int s = 0; s < ns; s++)
    {
        float u = float(i + curand_uniform(&local_rand_state)) / float(max_x);
        float v = float(j + curand_uniform(&local_rand_state)) / float(max_y);
        ray r = (*cam)->get_ray(u, v, &local_rand_state);
        col += color(r, world, &local_rand_state);
    }

    fb[pixel_index] = col / float(ns);
}

__global__ void render_init(int max_x, int max_y, curandState *rand_state)
{
    int i = threadIdx.x + blockIdx.x * blockDim.x;
    int j = threadIdx.y + blockIdx.y * blockDim.y;
    if ((i >= max_x) || (j >= max_y))
        return;
    int pixel_index = j * max_x + i;
    curand_init(1984, pixel_index, 0, &rand_state[pixel_index]);
}

__global__ void create_world(hittable **d_list, hittable **d_world, camera **d_camera, int nx, int ny)
{
    if (threadIdx.x == 0 && blockIdx.x == 0)
    {
        d_list[0] = new sphere(vec3(0, 0, -1), 0.5,
                               new lambertian(vec3(0.1, 0.2, 0.5)));
        d_list[1] = new sphere(vec3(0, -100.5, -1), 100,
                               new lambertian(vec3(0.8, 0.8, 0.0)));
        d_list[2] = new sphere(vec3(1, 0, -1), 0.5,
                               new metal(vec3(0.8, 0.6, 0.2), 0.0));
        d_list[3] = new sphere(vec3(-1, 0, -1), 0.5,
                               new dielectric(1.5));
        d_list[4] = new sphere(vec3(-1, 0, -1), -0.45,
                               new dielectric(1.5));
        *d_world = new hittable_list(d_list, 5);
        vec3 lookfrom(3, 3, 2);
        vec3 lookat(0, 0, -1);
        float vfov = 20.0;
        float dist_to_focus = (lookfrom - lookat).length();
        float aperture = 2.0;
        *d_camera = new camera(lookfrom,
                               lookat,
                               vec3(0, 1, 0),
                               vfov,
                               float(nx) / float(ny),
                               aperture,
                               dist_to_focus);
    }
}

__global__ void free_world(hittable **d_list, hittable **d_world, camera **d_camera)
{
    for (int i = 0; i < 5; i++)
    {
        delete ((sphere *)d_list[i])->mat_ptr;
        delete d_list[i];
    }
    delete *d_world;
    delete *d_camera;
}

int main()
{
    int nx = 1200;
    int ny = 600;
    int ns = 500; // Number of samples
    int tx = 8;
    int ty = 8;

    std::cerr << "Rendering a " << nx << "x" << ny << " image with " << ns << " samples per pixel ";
    std::cerr << "in " << tx << "x" << ty << " blocks.\n";

    int num_pixels = nx * ny;
    size_t fb_size = num_pixels * sizeof(vec3);

    // allocate FB
    vec3 *fb;
    checkCudaErrors(cudaMallocManaged((void **)&fb, fb_size));

    // allocate random state
    curandState *d_rand_state;
    checkCudaErrors(cudaMalloc((void **)&d_rand_state, num_pixels * sizeof(curandState)));

    // make our world of hitables
    hittable **d_list;
    checkCudaErrors(cudaMalloc((void **)&d_list, 2 * sizeof(hittable *)));
    hittable **d_world;
    checkCudaErrors(cudaMalloc((void **)&d_world, sizeof(hittable *)));
    camera **d_camera;
    checkCudaErrors(cudaMalloc((void **)&d_camera, sizeof(camera *)));
    create_world<<<1, 1>>>(d_list, d_world, d_camera, nx, ny);

    checkCudaErrors(cudaGetLastError());
    checkCudaErrors(cudaDeviceSynchronize());

    clock_t start, stop;
    start = clock();
    // Render our buffer
    dim3 blocks(nx / tx + 1, ny / ty + 1);
    dim3 threads(tx, ty);

    render_init<<<blocks, threads>>>(nx, ny, d_rand_state);
    checkCudaErrors(cudaGetLastError());
    checkCudaErrors(cudaDeviceSynchronize());

    render<<<blocks, threads>>>(fb, nx, ny, ns, d_camera, d_world, d_rand_state);
    checkCudaErrors(cudaGetLastError());
    checkCudaErrors(cudaDeviceSynchronize());

    stop = clock();
    double timer_seconds = ((double)(stop - start)) / CLOCKS_PER_SEC;
    std::cerr << "took " << timer_seconds << " seconds.\n";

    write_color(fb, nx, ny);

    // clean up
    checkCudaErrors(cudaDeviceSynchronize());
    free_world<<<1, 1>>>(d_list, d_world, d_camera);
    checkCudaErrors(cudaGetLastError());
    checkCudaErrors(cudaFree(d_list));
    checkCudaErrors(cudaFree(d_world));
    checkCudaErrors(cudaFree(d_camera));
    checkCudaErrors(cudaFree(fb));

    // useful for cuda-memcheck --leak-check full
    cudaDeviceReset();
}