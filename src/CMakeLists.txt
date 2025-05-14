set(SRC_CPP
    utils/objectTransform.cpp
    PDF/hairPDF.cpp
    mesh_io/meshLoader.cpp
    primitives/triangleMesh.cpp
    primitives/meshBVH.cpp
    sampler/sampler.cpp
    external/scene_io.cpp
    material/hairMarsch.cpp
    material/disneyPrincipledBSDF.cpp
    base/camera.cpp
    base/framebuffer.cpp
    texture/texture.cpp
    base/primitive.cpp
    base/object3D.cpp
    base/objectList.cpp
)

add_compile_definitions(GL_SIMD)

# add_compile_definitions(DISCARD_SAMPLING)
add_compile_definitions(BIASED_SAMPLING)

# FBX SDK configuration (optional)
find_package(FBX QUIET)

if(FBX_FOUND)
    message(STATUS "FBX SDK found - enabling FBX support")
    list(APPEND SRC_CPP mesh_io/fbxLoader.cpp)
    add_compile_definitions(HAS_FBX_SDK)
else()
    message(STATUS "FBX SDK not found - FBX support will be disabled")
endif()

add_library(pathtracer STATIC ${SRC_CPP})

# Set up include directories
target_include_directories(pathtracer
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)

if(FBX_FOUND)
    target_include_directories(pathtracer PUBLIC ${FBX_INCLUDE_DIRS})
    target_link_libraries(pathtracer PRIVATE ${FBX_LIBRARIES})
endif()

if(USE_OPENMP)
    target_link_libraries(pathtracer PRIVATE ${OPENMP_LIB})
endif()

add_executable(PathTracer main.cpp)
target_link_libraries(PathTracer PRIVATE pathtracer)

# Copy FBX SDK libraries to build directory if FBX is enabled
if(FBX_FOUND AND APPLE)
    add_custom_command(TARGET PathTracer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "/Applications/Autodesk/FBX SDK/2020.3.7/lib/clang/release"
        $<TARGET_FILE_DIR:PathTracer>
    )
endif()
