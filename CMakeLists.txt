cmake_minimum_required(VERSION 3.19)
project(Whitted_RT)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

include_directories(./src)
set(BASE ./src/utils/scene_io.cpp ./src/base/light.cpp ./src/base/camera.cpp ./src/base/object3D.cpp)
set(SOURCE_FILES ./src/raytracer.cpp)

add_executable(WhiitedRT ${SOURCE_FILES} ${BASE})
