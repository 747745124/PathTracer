#pragma once
#include "../base/lightList.hpp"
#include "../base/objectList.hpp"
#include "../utils/bvh.hpp"

inline gl::vec3 getRayColor(const Ray &ray, const ObjectList &prims,
                            const LightList &lights, gl::vec3 bg_color,
                            std::shared_ptr<BVHNode> bvh = nullptr) {
  using namespace gl;

  vec3 color(0.0f, 0.0f, 0.0f);

  float p = C_rand();
  float p_threshold = 0.05f;
  
  if (p < p_threshold)
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
    return mat->emit(hit_record.texCoords) +
           (attenuation * getRayColor(out_ray, prims, lights, bg_color, bvh) /
            (1.0f - p_threshold));

  return mat->emit(hit_record.texCoords) / (1.0f - p_threshold);
};