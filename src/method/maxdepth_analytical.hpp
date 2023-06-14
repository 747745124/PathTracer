#pragma once
#include "../base/lightList.hpp"
#include "../base/objectList.hpp"
#include "../utils/bvh.hpp"



static inline gl::vec3
getIrradianceVector(const std::vector<gl::vec3> &vertices,
                    const HitRecord &hit_record) {
  using namespace gl;
  using namespace std;

  vec3 p = hit_record.position;
  vec3 n = hit_record.normal;

  std::vector<vec3> u_k;
  std::vector<float> theta_k;
  std::vector<vec3> gamma_k;
  for (const auto &v : vertices)
    u_k.push_back((v - p).normalize());

  for (int i = 0; i < u_k.size(); i++) {
    float cos_theta = dot(u_k[i], u_k[(i + 1) % u_k.size()]);
    float theta = std::acos(cos_theta);
    theta_k.push_back(theta);
  }

  for (int i = 0; i < u_k.size(); i++) {
    vec3 gamma = cross(u_k[i], u_k[(i + 1) % u_k.size()]).normalize();
    gamma_k.push_back(gamma);
  }

  vec3 irr_vec = vec3(0.0);
  for (int i = 0; i < u_k.size(); i++) {
    irr_vec += 0.5f * gamma_k[i] * theta_k[i];
  }

  return irr_vec;
};

inline gl::vec3 getRayColor(const Ray &ray, const ObjectList &prims,
                            gl::vec3 bg_color,
                            const std::vector<gl::vec3> &l_vertices,
                            uint max_depth = 2,
                            std::shared_ptr<BVHNode> bvh = nullptr) {
  using namespace gl;

  if (max_depth == 0)
    return bg_color;

  HitRecord hit_record;
  bool is_hit = false;
  if (bvh == nullptr)
    is_hit = prims.intersect(ray, hit_record);
  else
    is_hit = bvh->intersect(ray, hit_record);

  if (!is_hit)
    return bg_color;

  Ray out_ray;
  vec3 albedo;
  float pdf = 0.f;
  auto mat = hit_record.material;
  if (mat->scatter(ray, hit_record, albedo, out_ray, pdf)) {
    vec3 irr_vec = getIrradianceVector(l_vertices, hit_record);
    float irr_term = dot(irr_vec, hit_record.normal) > 0.f
                         ? dot(irr_vec, hit_record.normal)
                         : 0;

    return mat->emit(hit_record.texCoords) +
            albedo / M_PI *
                getRayColor(out_ray, prims, bg_color, l_vertices, max_depth - 1,
                            bvh) *
           irr_term;
  }

  return mat->emit(hit_record.texCoords);
};
