# Raytracing in One Weekend with Multi-threading and GPU Support

This repository is a modified version of the "Raytracing in One Weekend" tutorial, featuring additional enhancements, multi-threading, and GPU support with CUDA.
![rendered image](RayTracingImage.png)

## Branches
- `master`: Plain Raytracing in One Weekend project with output directly to a .ppm file.
- `cpuParallel`: Implementation of an image object that schedules tasks for threads and handles race conditions with locks.
- `cpuContinuous`: A modified version of `cpuParallel` that outputs the image after every iteration to improve it continuously.
- `gpuParallel`: Ported the project to run on NVIDIA CUDA-capable GPUs, providing a 20x performance improvement over `cpuParallel` and more than 80x improvement compared to the plain "Raytracing in One Weekend" tutorial.

Explore the branches to see the different versions and features
