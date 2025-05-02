# Path Tracer
An importance sampled path tracer, supports Marschner Hair / Phong / Dielectric / Conductor material and various Monte Carlo effects.

![image](images/sample.png)
## Build Instructions:
This project uses CMake to build and is correctly built under M1 macOS environment. The C++ standard is set to C++ 20.

Once CMake is installed, use below commands to build with CMake.

This project uses OpenMP for parallization, for ARM Mac users, please refer to this [post][https://stackoverflow.com/questions/71061894/how-to-install-openmp-on-mac-m1] .

Add GL_SIMD to compile definition to enable SIMD (for vec and matrix class).
```
mkdir build
cd build
cmake ..
make .
```

### MIS+NEE:

The path tracer uses an importance sampling strategy, a mixed PDF of Material PDF and light sampling with NEE (next-event-estimation).

### Monte Carlo and Post-processing effects:

The path tracer supports DoF, motion blur. Image filtering and tone-mapping.

### Materials:
1. Marschner Hair
2. Phong
3. Dielectric
4. Conductor (Microfacet BRDF)
5. Lambertian
6. Kajiya-Kay


