#include <iostream>
#include <time.h>
#include <float.h>
#include <curand_kernel.h>

#include "vec3.cuh"
#include "ray.cuh"
#include "sphere.cuh"
#include "hittable_list.cuh"
#include "camera.cuh"

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

#define RANDVEC3 vec3(curand_uniform(local_rand_state), curand_uniform(local_rand_state), curand_uniform(local_rand_state))

__device__ vec3 random_in_unit_sphere(curandState *local_rand_state)
{
    vec3 p;
    do
    {
        p = 2.0f * RANDVEC3 - vec3(1, 1, 1);
    } while (p.squared_length() >= 1.0f);
    return p;
}

__device__ float crossProduct(vec3 a, vec3 b)
{
    return (a.x() * b.y() + a.y() * b.z() + a.z() * b.x());
}

__device__ vec3 color(const ray &r, hittable **world, curandState *local_random_state)
{
    ray cur_ray = r;
    float cur_attenuation = 1.0f;
    vec3 curcol = vec3(0.0, 0.0, 0.0);
    const int bounces = 7;
    hit_record path[bounces];
    int hits = 0;
    for (int i = 0; i < bounces; i++)
    {
        hit_record rec;
        if ((*world)->hit(cur_ray, 0.001f, FLT_MAX, rec))
        {
            vec3 target = rec.p + rec.normal + (random_in_unit_sphere(local_random_state) * ( 1 - rec.reflect));
            curcol = rec.color * cur_attenuation;
            path[i] = rec;
            cur_attenuation *= 0.5;
            cur_ray = ray(rec.p, target - rec.p);
            hits++;
        }
        else
        {
             vec3 unit_direction = unit_vector(cur_ray.direction());
             float t = 0.5f * (unit_direction.y() + 1.0f);
            //  vec3 c = (1.0f - t) * vec3(0.1, 0.0, 0.3) + t * vec3(0.5, 0.7, 1.0);
             vec3 c = (1.0f - t) * vec3(0.5, 0.2, 0.1) + t * vec3(0.2, 0.2, 0.2);
            // return curcol + cur_attenuation * c;
            hit_record hr = hit_record();
            hr.color = c;
            hr.luminance = 1;
            path[i] = hr;
            hits++;
            break;
        }
    }

    vec3 color = vec3(0.0, 0.0, 0.0);
    for (int i = hits - 1; i >= 0; i--)
    {
            color = (vec3(color.x() * path[i].color.x(), color.y() * path[i].color.y(), color.z() * path[i].color.z()) * path[i].reflect) + path[i].color * path[i].luminance;// * path[i].reflect;        
    }

    return color;
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
        ray r = (*cam)->get_ray(u, v);
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

__global__ void create_world(hittable **d_list, hittable **d_world, camera **d_camera)
{
    if (threadIdx.x == 0 && blockIdx.x == 0)
    {
        *(d_list) = new sphere(vec3(0, 0, -1.5), 0.5, vec3(1.0, 0, 0), 0.5, 0.8);               //red
        *(d_list + 1) = new sphere(vec3(0.4, 0.0, -1.0), 0.2, vec3(1.0, 1.0, 1.0), 0.0, 1.0);   //mirror
        *(d_list + 2) = new sphere(vec3(0.5, 0.0, -0.5), 0.1, vec3(0.2, 1.0, 0.3), 0.0, 1.0);   //Green mirror
        *(d_list + 3) = new sphere(vec3(-0.2, -0.1, -0.9), 0.2, vec3(0.0, 0.2, 0.5), 0.0, 0.5); //blue
        *(d_list + 4) = new sphere(vec3(-0.0, -0.1, -0.6), 0.1, vec3(1.0, 1.0, 1.0), 5.0, 0.0); //white / light source
        *(d_list + 5) = new sphere(vec3(0, -100.5, -1), 100, vec3(0.5, 0.5, 0.5), 0.0, 0.5);    //big / floor
        *(d_list + 6) = new sphere(vec3(0.4, 7, -1), 5, vec3(1.0, 1.0, 1.0), 2.0, 1.0);    //light source 2
        *d_world = new hittable_list(d_list, 7);
        *d_camera = new camera();
    }
}

__global__ void free_world(hittable **d_list, hittable **d_world, camera **d_camera)
{
    delete *(d_list);
    delete *(d_list + 1);
    delete *d_world;
    delete *d_camera;
}

int main()
{
    int nx = 1200;
    int ny = 600;
    int ns = 1000; // Number of samples
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
    create_world<<<1, 1>>>(d_list, d_world, d_camera);

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

    // Output FB as Image
    std::cout << "P3\n"
              << nx << " " << ny << "\n255\n";
    for (int j = ny - 1; j >= 0; j--)
    {
        for (int i = 0; i < nx; i++)
        {
            size_t pixel_index = j * nx + i;
            int ir = int(255.99 * fb[pixel_index].r());
            int ig = int(255.99 * fb[pixel_index].g());
            int ib = int(255.99 * fb[pixel_index].b());
            std::cout << ir << " " << ig << " " << ib << "\n";
        }
    }

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