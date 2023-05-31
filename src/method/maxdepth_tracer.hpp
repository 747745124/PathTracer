#pragma once
#include "../base/lightList.hpp"
#include "../base/objectList.hpp"
#include "../utils/bvh.hpp"

inline gl::vec3 getRayColor(const Ray &ray, const ObjectList &prims,
                            gl::vec3 bg_color, uint max_depth = 40,
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
  vec3 attenuation;
  auto mat = hit_record.material;
  if (mat->scatter(ray, hit_record, attenuation, out_ray))
    return (mat->emit(hit_record.texCoords) +
            attenuation *
                getRayColor(out_ray, prims, bg_color, max_depth - 1, bvh));

    return mat->emit(hit_record.texCoords);
};
