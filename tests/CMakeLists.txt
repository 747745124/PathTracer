# tests/CMakeLists.txt
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/include)

add_executable(test_mesh_io test_mesh_io.cpp)
target_link_libraries(test_mesh_io
    PRIVATE
    pathtracer # your main renderer library
    ${GTEST_BOTH_LIBRARIES}
    pthread)

add_executable(test_hair_pdf test_hair_pdf.cpp)
target_link_libraries(test_hair_pdf
    PRIVATE
    pathtracer # your main renderer library
    ${GTEST_BOTH_LIBRARIES}
    pthread)

add_test(NAME MeshIO COMMAND test_mesh_io)
add_test(NAME HairPDF COMMAND test_mesh_io)