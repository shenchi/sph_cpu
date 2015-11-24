sph_cpu
===
This a simple implemention of sph fluid emulator. It's a part of my graduate project in my university. I think it's better to keep them here. :) Maybe I'll make some improvements to this.

It uses particles to represent the fluid and SPH(Smoothed-Particle Hydrodynamics) method to simulate fluid's movement. The simulation is done on CPU and the particles' positions are transfer to GPU to render, via OpenGL. For now, the particles is rendered as point sprite, and I'm finding a method to render the them like real fluid in real-time. 

There also a CUDA version of the simulater, which don't need the transfer of particles' data which is directly allocated in the video memory. And with the nature of GPU's concurrency, the CUDA version is quite faster than the CPU version. Right now, I don't have a Nvidia device, but I will upload the sources as soon as I get one and adapt the sources to new CUDA version.

build
---
Please make sure you have `glfw3` installed and just `make` or open the Xcode project to build it.
