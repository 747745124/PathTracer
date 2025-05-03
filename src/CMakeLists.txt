file(GLOB_RECURSE SRC_CPP
    base/*.cpp
    material/*.cpp
    mesh_io/*.cpp
    primitives/*.cpp
    probs/*.cpp
    external/scene_io.cpp
    utils/objectTransform.cpp
)

add_library(pathtracer STATIC ${SRC_CPP})
target_include_directories(pathtracer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(USE_OPENMP)
    target_link_libraries(pathtracer PRIVATE ${OPENMP_LIB})
endif()

add_executable(PathTracer main.cpp)
target_link_libraries(PathTracer PRIVATE pathtracer)

add_executable(UnitTests unit_test.cpp)
target_link_libraries(UnitTests PRIVATE pathtracer)