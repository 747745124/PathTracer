#include "./triangleMesh.hpp"
#include <limits>
#include <numeric>

TriangleMesh::TriangleMesh(MeshData &&data, std::shared_ptr<Material> mat)
    : mesh(std::move(data)), material(std::move(mat)) {
  std::vector<int> ids(mesh.indices.size());
  std::iota(ids.begin(), ids.end(), 0);
  bvh = std::make_unique<MeshBVHNode>(&mesh.positions, &mesh.indices, ids, 0,
                                      (int)ids.size());
  this->objtype = ObjType::MESH_OBJ;
}

bool TriangleMesh::intersect(const Ray &ray, HitRecord &rec, float tmin,
                             float tmax) const {
  if (!bvh->intersect(ray, rec, tmin, tmax))
    return false;
  rec.material = material;
  return true;
}

AABB TriangleMesh::getAABB(float, float) {
  gl::vec3 mn(+INFINITY), mx(-INFINITY);
  for (auto &v : mesh.positions) {
    mn = gl::min(mn, v);
    mx = gl::max(mx, v);
  }
  return AABB(mn, mx);
}