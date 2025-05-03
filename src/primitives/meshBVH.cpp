
// MeshBVHNode.cpp
#include "./meshBVH.hpp"
#include "../utils/utility.hpp"
#include <algorithm>

MeshBVHNode::MeshBVHNode(const std::vector<gl::vec3> *verts,
                         const std::vector<std::array<int, 3>> *tris,
                         const std::vector<int> &ids, int start, int end)
    : vertices(verts), triangles(tris) {
  // Compute bounding box for triangles[start..end)
  gl::vec3 mn(+INFINITY), mx(-INFINITY);
  for (int i = start; i < end; ++i) {
    auto &t = (*triangles)[ids[i]];
    for (int k : {t[0], t[1], t[2]}) {
      mn = gl::min(mn, (*vertices)[k]);
      mx = gl::max(mx, (*vertices)[k]);
    }
  }
  box = AABB(mn, mx);

  int count = end - start;
  if (count <= 4) {
    // leaf
    tri_ids.assign(ids.begin() + start, ids.begin() + end);
  } else {
    // choose split axis by longest extent
    gl::vec3 ext = mx - mn;
    int axis = ext.x() > ext.y() && ext.x() > ext.z() ? 0
               : ext.y() > ext.z()                    ? 1
                                                      : 2;

    // sort indices by triangle centroid on that axis
    std::vector<int> sorted(ids.begin() + start, ids.begin() + end);
    std::sort(sorted.begin(), sorted.end(), [&](int a, int b) {
      auto ca = (((*vertices)[(*tris)[a][0]] + (*vertices)[(*tris)[a][1]] +
                  (*vertices)[(*tris)[a][2]]) /
                 3.0f);
      auto cb = (((*vertices)[(*tris)[b][0]] + (*vertices)[(*tris)[b][1]] +
                  (*vertices)[(*tris)[b][2]]) /
                 3.0f);
      return ca[axis] < cb[axis];
    });
    int mid = count / 2;
    left = std::make_unique<MeshBVHNode>(verts, tris, sorted, 0, mid);
    right = std::make_unique<MeshBVHNode>(verts, tris, sorted, mid, count);
  }
}

// struct HitRecord {
//     public:
//       float t;
//       gl::vec3 normal;
//       gl::vec3 position;
//       std::shared_ptr<Material> material;
//       gl::vec2 texCoords = gl::vec2(0.0f);
//       gl::vec3 hair_tangent = gl::vec3(0.0f);
//       // the tangent of the hair, used for hair shading

//       // Ref: rt in one weeknd
//       // This is used to determine whether the ray is inside or outside the
//       object
//       // As we want have the normal always point against the ray
//       bool is_inside;
//       void set_normal(const Ray &ray, const gl::vec3 &n) {
//         this->is_inside = dot(ray.getDirection(), n) < 0;
//         this->normal = this->is_inside ? n : -n;
//       }
//     };

bool MeshBVHNode::intersect(const Ray &ray, HitRecord &rec, float tmin,
                            float tmax) const {
  if (!box.intersect(ray, tmin, tmax))
    return false;
  bool hit = false;
  float closest = tmax;

  if (!left && !right) {
    // leaf: test all triangles
    for (int id : tri_ids) {
      if (hitTriangle(id, ray, tmin, closest, rec)) {
        hit = true;
        closest = rec.t;
      }
    }
  } else {
    if (left && left->intersect(ray, rec, tmin, closest)) {
      hit = true;
      closest = rec.t;
    }
    if (right && right->intersect(ray, rec, tmin, closest)) {
      hit = true;
    }
  }
  return hit;
}

bool MeshBVHNode::hitTriangle(int triIdx, const Ray &ray, float tmin,
                              float tmax, HitRecord &rec) const {
  auto &tri = (*triangles)[triIdx];
  const gl::vec3 &p0 = (*vertices)[tri[0]];
  const gl::vec3 &p1 = (*vertices)[tri[1]];
  const gl::vec3 &p2 = (*vertices)[tri[2]];

  gl::vec3 e1 = p1 - p0;
  gl::vec3 e2 = p2 - p0;
  gl::vec3 P = gl::cross(ray.getDirection(), e2);
  float det = gl::dot(e1, P);
  if (fabs(det) < 1e-8f)
    return false;
  float invDet = 1.0f / det;

  gl::vec3 T = ray.getOrigin() - p0;
  float u = gl::dot(T, P) * invDet;
  if (u < 0 || u > 1)
    return false;

  gl::vec3 Q = gl::cross(T, e1);
  float v = gl::dot(ray.getDirection(), Q) * invDet;
  if (v < 0 || u + v > 1)
    return false;

  float t = gl::dot(e2, Q) * invDet;
  if (t < tmin || t > tmax)
    return false;

  rec.t = t;
  rec.position = ray.at(t);
  rec.normal = gl::normalize(gl::cross(e1, e2));
  return true;
}