### Assignment 2 - Whitted Style Ray-tracing

#### Build Instruction

* Linux / macOS

1. Use homebrew (macOS), apt-get (Linux)  to install openmp (libomp for Mac)
2. Compile with CMake. `CMakeLists` has been provided.

* Windows

This assignment use Visual Studio 2022 Community Edition to compile, which has native OpenMP support.



#### External Library

This assignment uses below libraries:

* OpenMP (multiprocessing)
* stb_image (Image IO)
* sse2neon (SIMD adapater for arm64)



#### Render Setting:

By default, this assignment rendered at 1500 * 1500 pixels, with 2 * 2 sample per pixel and a max depth of 10.



#### File Structures:

* `src`  - contains source files
  * `base` - basic data structures, primitives etc.
  * `external` - external libraries
  * `method` - raytracing method, in this case, Whitted-style ray tracing
  * `utils` - utility functions, including basic `vec` class, matrix computation etc.
  * `raytracer.cpp` - main function
    * use `GL_SIMD` to enable SIMD optimized `vec` class
    * set `GAMMA` to enable gamma correction
  * `unit_test.cpp`

* `results`  - final rendering results of 5 scenes
* `Scenes` - scene files
* `unit-test` - some intermediate rendering pictures



#### Sample Results

<img src="/Users/naoyuki/Library/Application%20Support/typora-user-images/image-20230504212310253.png" alt="image-20230504212310253" style="zoom:33%;" />

<img src="https://s2.loli.net/2023/05/05/pGgABLS2Qml7hao.png" alt="image-20230504212330808" style="zoom: 33%;" />

#### Optimization

This project uses SIMD (`SSE` instructions) to optimize the `vec` class.

* Setting: 1500*1500 px, Spp 2\*2, Max depth 10

* Before SIMD: 15-17s, After SIMD: 13-16s (macOS, M1 Max)

This project uses OpenMP to parallelize ray casting process.

* The number of threads is set to `num_of_process()`
  * Setting it to  `num_of_process()+1` can be extremely slow for simple scenes.

