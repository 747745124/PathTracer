// tests/test_mesh_io.cpp
#include "../src/mesh_io/meshLoader.hpp"
#include "../src/primitives/triangleMesh.hpp"
#include <gtest/gtest.h>

TEST(MeshLoader, SimpleTriangle) {
  auto mat = gl::DefaultMaterial;
  auto mesh = loadOBJMesh("./tests/data/minimal.obj", mat);

  // 3 verts, 1 face
  assert(mesh->getMeshData().indices.size() == 1u);
  assert(mesh->getMeshData().positions.size() == 3u);

  Ray r(gl::vec3(0.25f, 0.25f, -1), gl::vec3(0, 0, 1));
  HitRecord rec;
  ASSERT_TRUE(mesh->intersect(r, rec, 0.001f, 100.f));

  EXPECT_FLOAT_EQ(rec.position.x(), 0.25f);
  EXPECT_FLOAT_EQ(rec.position.y(), 0.25f);
  EXPECT_FLOAT_EQ(rec.position.z(), 0.0f);

  auto aabb = mesh->getAABB(0, 1);
  EXPECT_EQ(aabb.get_min(), gl::vec3(0, 0, 0));
  EXPECT_EQ(aabb.get_max(), gl::vec3(1, 1, 0));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
