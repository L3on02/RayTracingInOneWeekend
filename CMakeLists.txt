# General
cmake_minimum_required(VERSION 3.16)

set(CMAKE_CUDA_COMPILER /usr/local/cuda/bin/nvcc)
set(CMAKE_CUDA_HOST_COMPILER g++)
set(CMAKE_CUDA_FLAGS "-m64 -gencode arch=compute_80,code=sm_80")
set(CMAKE_CUDA_STANDARD 11)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)
project(ParallelSystems LANGUAGES CXX CUDA)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Add source files
#add_library(${PROJECT_NAME} STATIC main.cu camera.cuh color.cuh hittable_list.cuh hittable.cuh material.cuh ray.cuh sphere.cuh vec3.cuh)


# Define the executable
add_executable(${PROJECT_NAME} main.cu camera.cuh color.cuh hittable_list.cuh hittable.cuh material.cuh ray.cuh sphere.cuh vec3.cuh)
# Define the include DIRs
include_directories("${CMAKE_SOURCE_DIR}")
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES}")

# Set CUDA_ARCHITECTURES property
set_target_properties(${PROJECT_NAME} PROPERTIES CUDA_ARCHITECTURES "80")
set_target_properties(${PROJECT_NAME} PROPERTIES CUDA_SEPARABLE_COMPILATION ON)

target_link_libraries(${PROJECT_NAME} cuda)