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

add_executable(test_halton test_halton.cpp)
target_link_libraries(test_halton
    PRIVATE
    pathtracer # your main renderer library
    ${GTEST_BOTH_LIBRARIES}
    pthread)

add_executable(test_refract test_refract.cpp)
target_link_libraries(test_refract
    PRIVATE
    pathtracer # your main renderer library
    ${GTEST_BOTH_LIBRARIES}
    pthread)

add_executable(test_dielectric test_dielectric.cpp)
target_link_libraries(test_dielectric
    PRIVATE
    pathtracer # your main renderer library
    ${GTEST_BOTH_LIBRARIES}
    pthread)

add_executable(test_pdf_normalization test_pdf_normalization.cpp)
target_link_libraries(test_pdf_normalization
    PRIVATE
    pathtracer # your main renderer library
    ${GTEST_BOTH_LIBRARIES}
    pthread)

add_test(NAME MeshIO COMMAND test_mesh_io)
add_test(NAME HairPDF COMMAND test_hair_pdf)
add_test(NAME Halton COMMAND test_halton)
add_test(NAME Refract COMMAND test_refract)
add_test(NAME Dielectric COMMAND test_dielectric)
add_test(NAME PDFNormalization COMMAND test_pdf_normalization)