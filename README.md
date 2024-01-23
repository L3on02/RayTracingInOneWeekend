# Raytracing in One Weekend with Multi-threading and GPU Support

This repository is a modified version of the "Raytracing in One Weekend" tutorial, featuring additional enhancements, multi-threading, GPU support with CUDA and a simple GUI.

![rendered image](RayTracingImage.png)

## other relevant Branches
- `master`: Plain Raytracing in One Weekend project with output directly to a .ppm file.
- `cpuParallel`: Implementation of an image object that schedules tasks for threads and handles race conditions with locks.
- `cpuContinuous`: A modified version of `cpuParallel` that outputs the image after every iteration to improve it continuously.
- `cpuGUI`: Adds a simple GUI to `cpuParallel` that provides control over render settings, resolution and quality aswell as a live viewer for the rendered images.
- `gpuParallel`: Ported the project to run on NVIDIA CUDA-capable GPUs, providing a 20x performance improvement over `cpuParallel` and more than 80x improvement compared to the plain "Raytracing in One Weekend" tutorial.
- `combinedGUI`: Unites all functionality into one programm that can render on both CPU and GPU with full support of the GUI. (requires GLFW: sudo apt-get install libglfw3 / libglfw3-dev / libglew-dev)
  
(For `gpuParallel` and `combinedGUI`: Make sure to update the GENCODE_FLAGS (-gencode arch=compute_X,code=sm_X) in the Makefile/CMakeList to support your GPU: X=60 for GTX 10-Series, X=70 for RTX 20-Series and X=80 for RTX 30-Series GPUs)

Explore the branches to see the different versions and features
