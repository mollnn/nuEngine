# Night-up Engine

Real-time rendering engine, built on OpenGL 

![](https://github.com/mollnn/nuEngine/blob/main/docs/images/sponza_gi_rsm.jpg?raw=true)

## Feature Set

- Assets loader based on Assimp
- Classical Blinn-Phong Material Support
- Scene Hierarchy 
- Deferred Rendering
- Shadow Mapping with cube map, supporting at most one point light
- Percentage-Closer Soft Shadow (PCSS) 
- Screen Space Ambient Occlusion (SSAO) 
- Reflective Shadow Mapping (RSM) for diffuse indirect illumination
- Screen Space Reflection (SSR) for gloosy and specular indirect illumination
- Denoising with Joint Bilateral Filtering (JBF)

### WIP

- Prefiltered Image-based Lighting 
- Precomputed Radiance Transfer with SH basis
- Skeleton Animation

## Dependency

GLFW, GLAD, GLM, Assimp, CMake

Building with GCC is recommended. Already tested on Windows 10 and Ubuntu 20.04.