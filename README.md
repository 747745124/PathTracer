Path Tracer
An importance sampled path tracer, supports Phong / Dielectric / Metal material and various Monte Carlo effects.


Build Instructions:
This project uses CMake to build and is correctly built under M1 macOS environment. The C++ standard is set to C++ 17.

Once CMake is installed, use below commands to build with CMake.

This project uses OpenMP for parallization, for ARM Mac users, please refer to this [post][https://stackoverflow.com/questions/71061894/how-to-install-openmp-on-mac-m1] .

Add GL_SIMD to compile definition to enable SIMD (for vec and matrix class).
```
mkdir build
cd build
cmake ..
make .
File Structures:
./src
├── ./src/base
│   ├── ./src/base/camera.cpp
│   ├── ./src/base/camera.hpp
│   ├── ./src/base/compound.hpp
│   ├── ./src/base/framebuffer.cpp
│   ├── ./src/base/framebuffer.hpp
│   ├── ./src/base/light.cpp
│   ├── ./src/base/light.hpp
│   ├── ./src/base/lightList.hpp
│   ├── ./src/base/material.hpp
│   ├── ./src/base/medium.hpp
│   ├── ./src/base/object3D.cpp
│   ├── ./src/base/object3D.hpp
│   ├── ./src/base/objectList.cpp
│   ├── ./src/base/objectList.hpp
│   ├── ./src/base/primitive.cpp
│   ├── ./src/base/primitive.hpp
│   ├── ./src/base/ray.hpp
│   ├── ./src/base/texture.cpp
│   ├── ./src/base/texture.hpp
│   └── ./src/base/vertex.hpp
├── ./src/deprecated
│   └── ./src/deprecated/raytracer.cpp
├── ./src/external
│   ├── ./src/external/sse2neon.h
│   ├── ./src/external/stb_image.h
│   └── ./src/external/stb_image_write.h
├── ./src/method
│   ├── ./src/method/analytical_illumin.hpp
│   ├── ./src/method/maxdepth_naive.hpp
│   ├── ./src/method/maxdepth_shadowray.hpp
│   ├── ./src/method/roulette_naive.hpp
├── ./src/probs
│   ├── ./src/probs/hittablePDF.hpp
│   ├── ./src/probs/mixedPDF.hpp
│   ├── ./src/probs/pdf.hpp
│   └── ./src/probs/random.hpp
├── ./src/scene_func
│   ├── ./src/scene_func/renderManager.hpp
│   └── ./src/scene_func/scenes.hpp
├── ./src/unit_test.cpp
├── ./src/unit_test.hpp
├── ./src/main.cpp
└── ./src/utils
    ├── ./src/utils/aabb.hpp
    ├── ./src/utils/bvh.hpp
    ├── ./src/utils/colors.hpp
    ├── ./src/utils/commonMaterials.hpp
    ├── ./src/utils/matrix.hpp
    ├── ./src/utils/objectTransform.cpp
    ├── ./src/utils/objectTransform.hpp
    ├── ./src/utils/orthoBasis.hpp
    ├── ./src/utils/pattern.hpp
    ├── ./src/utils/quat.hpp
    ├── ./src/utils/scene_io.cpp
    ├── ./src/utils/scene_io.hpp
    ├── ./src/utils/timeit.hpp
    ├── ./src/utils/transformations.hpp
    └── ./src/utils/utility.hpp
```
src/base contains base classes
src/utils contains common materials, utility functions and classes
src/probs contains PDF related functions and classes
src/method contains rendering methods
src/main.cpp contains the main function
src/scene_func contains a rendering manager, and all scene infos are written in scenes.hpp
All rendering settings except light sampling number, are written in the scenes.hpp files. Light sampling numbers are given in src/method/maxdepth_naive.hpp as macro define.
Features:
Different Rendering Methods:

Analytical Direct Illumination: This one provides an analytical direct lighting solution if the light is polygonal and there’s no occluder.
analytical_box

Max Depth NEE: This one uses next event estimation to sample direct illumination.
Shadowray Path Tracer: This one uses shadowray method to separate direct and indirect illumination.
Max Depth Naive: This one is the basic importance sampled path tracer, with a max depth limit.
Roulette Naive: This one is the basic importance sampled path tracer, uses Russian Roulette method for unbiased estimation.
Whitted: This one is the traditional Whitted-style ray tracing. See src/deprecated/whitted.hpp
cornell_box_modified_2

Importance Sampling:

The path tracer uses an importance sampling strategy, a mixed PDF of cosine and light sampling. The light sampling can be arbitrary numbers.
Veach MIS

Monte Carlo and Post-processing effects:

The path tracer supports DoF, motion blur. Image filtering and tone-mapping.
Whitted

TODO:
Integrate MIS in NEE
Add sampling PDF for Marschner
Bidirectional Path Tracing implementation
Volume interfaces, Volumetric Path Tracing

