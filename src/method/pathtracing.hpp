#pragma once
#include "../base/lightList.hpp"
#include "../base/objectList.hpp"
#include "../utils/bvh.hpp"

inline gl::vec3 getRayColor(const Ray &ray, const ObjectList &prims,
                            const LightList &lights,
                            std::shared_ptr<BVHNode> bvh = nullptr) {
  using namespace gl;

  vec3 color(0.0f, 0.0f, 0.0f);
  vec3 bg(0.0f, 0.0f, 0.0f);

  float p = C_rand();
  float p_threshold = 0.01f;
  if (p < p_threshold)
    return bg;

  HitRecord hit_record;
  bool is_hit = false;
  if (bvh == nullptr)
    is_hit = prims.intersect(ray, hit_record);
  else
    is_hit = bvh->intersect(ray, hit_record);

  if (is_hit) {
    Ray out_ray;
    vec3 attenuation;
    if (hit_record.material->scatter(ray, hit_record, attenuation, out_ray)) {
      color += attenuation * getRayColor(out_ray, prims, lights, bvh);
    }
    return color;
  }

  vec3 unit_direction = ray.getDirection().normalize();
  auto t = 0.5 * (unit_direction.y() + 1.0);
  return (1.0 - t) * vec3(1.0, 1.0, 1.0) +
         t * vec3(0.5, 0.7, 1.0) / (1 - p_threshold);
}
