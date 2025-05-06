cmake_minimum_required(VERSION 3.19)
project(PathTracer LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# find_package(OpenMP)
if(OpenMP_FOUND)
    message(STATUS "✔️ System OpenMP found (C:${OpenMP_C_VERSION}, CXX:${OpenMP_CXX_VERSION})")
    set(USE_OPENMP_SYSTEM TRUE)
    set(USE_OPENMP TRUE)
else()
    # ────────────────────────────────────────────────────────────────────
    # Manual OpenMP support on macOS/Homebrew
    # ────────────────────────────────────────────────────────────────────
    if(APPLE)
        # figure out where brew put libomp
        execute_process(
            COMMAND brew --prefix libomp
            OUTPUT_VARIABLE LIBOMP_ROOT
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )

        if(NOT EXISTS "${LIBOMP_ROOT}/include/omp.h")
            message(FATAL_ERROR "Could not find omp.h under ${LIBOMP_ROOT}/include")
        endif()

        # tell the driver to preprocess for OpenMP
        add_compile_options(-Xpreprocessor -fopenmp)

        # add the include path for omp.h
        include_directories("${LIBOMP_ROOT}/include")

        # link against libomp
        link_directories("${LIBOMP_ROOT}/lib")
        set(OPENMP_LIB "omp") # name of the libomp library
        set(USE_OPENMP TRUE)
    else()
        message(WARNING "⚠️  libomp not found under ${LIBOMP_ROOT}, disabling OpenMP")
        set(USE_OPENMP FALSE)
    endif()
endif()

# ────────────────────────────────────────────────────────────────────
# Your existing subdirectory setup
# ────────────────────────────────────────────────────────────────────
add_subdirectory(src)
enable_testing()
add_subdirectory(tests) # builds and registers unit tests