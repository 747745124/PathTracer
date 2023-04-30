cmake_minimum_required(VERSION 3.19)
project(Whitted_RT)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

if(APPLE)
set(CMAKE_C_COMPILER "/opt/homebrew/Cellar/llvm/16.0.2/bin/clang")
set(CMAKE_CXX_COMPILER "/opt/homebrew/Cellar/llvm/16.0.2/bin/clang++")
set(OPENMP_LIBRARIES "/opt/homebrew/Cellar/llvm/16.0.2/lib")
set(OPENMP_INCLUDES "/opt/homebrew/Cellar/llvm/16.0.2/include")

set(OPENMP_LIBRARIES "/usr/local/opt/llvm/lib")
link_directories(${OPENMP_LIBRARIES})
endif()

include_directories(./src)
set(BASE ./src/utils/scene_io.cpp ./src/base/light.cpp ./src/base/camera.cpp ./src/base/object3D.cpp ./src/base/primitive.cpp ./src/base/framebuffer.cpp ./src/base/objectList.cpp)
set(SOURCE_FILES ./src/unit_test.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
add_executable(WhiitedRT ${SOURCE_FILES} ${BASE})
