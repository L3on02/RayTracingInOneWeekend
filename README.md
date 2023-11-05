# Raytracing in One Weekend with Multi-threading and GPU Support

This repository is a modified version of the "Raytracing in One Weekend" tutorial, featuring additional enhancements, multi-threading, and GPU support with CUDA.

![rendered image](RayTracingImage.png)

## Branches
- `master`: Plain Raytracing in One Weekend project with output directly to a .ppm file.
- `cpuParallel`: Implementation of an image object that schedules tasks for threads and handles race conditions with locks.
- `cpuContinuous`: A modified version of `cpuParallel` that outputs the image after every iteration to improve it continuously.
- `gpuParallel`: Ported the project to run on NVIDIA CUDA-capable GPUs, providing a 20x performance improvement over `cpuParallel` and more than 80x improvement compared to the plain "Raytracing in One Weekend" tutorial.
  (Make sure to update the GENCODE_FLAGS (-gencode arch=compute_X,code=sm_X) to support your GPU: X=60 for GTX 10-Series, X=70 for RTX 20-Series and X=80 for RTX 30-Series GPUs)

Explore the branches to see the different versions and features
