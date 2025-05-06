file(GLOB_RECURSE SRC_CPP
    base/*.cpp
    material/*.cpp
    mesh_io/*.cpp
    primitives/*.cpp
    probs/*.cpp
    sampler/*.cpp
    external/scene_io.cpp
    utils/objectTransform.cpp
)

add_compile_definitions(GL_SIMD)

# add_compile_definitions(DISCARD)
# add_compile_definitions(BIASED_SAMPLING)
add_library(pathtracer STATIC ${SRC_CPP})
target_include_directories(pathtracer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(USE_OPENMP)
    target_link_libraries(pathtracer PRIVATE ${OPENMP_LIB})
endif()

add_executable(PathTracer main.cpp)
target_link_libraries(PathTracer PRIVATE pathtracer)
