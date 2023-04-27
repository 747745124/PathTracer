cmake_minimum_required(VERSION 3.19)
project(Whitted_RT)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

include_directories(./src)
set(BASE ./src/utils/scene_io.cpp ./src/base/light.cpp ./src/base/camera.cpp ./src/base/object3D.cpp ./src/base/primitive.cpp ./src/base/framebuffer.cpp)
set(SOURCE_FILES ./src/unit_test.cpp)

add_executable(WhiitedRT ${SOURCE_FILES} ${BASE})
